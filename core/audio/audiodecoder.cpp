#include "core/audio/audiodecoder.h"

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#define SAMPLE_CORRECTION_PERCENT_MAX 10
#define AUDIO_DIFF_AVG_NB 20
#define AV_NOSYNC_THRESHOLD 10.0

namespace RePlay
{
	namespace Core
	{
		namespace Audio
		{
			AudioDecoder::AudioDecoder()
				: Decoder(),
				  pAudioResampler(nullptr),
				  pFrame(nullptr),
				  pAudioBuffer(nullptr),
				  pAudioBufferIndex(0),
				  pAudioBufferSize(0),
				  pSamplesPerFrame(1024),
				  pSyncThreshold(0.01),
				  pLastDelay(0),
				  pFrameTimer(0),
				  pAudioDiffCoef(0),
				  pAudioDiffTreshold(0),
				  pAudioDiffCum(0),
				  pAudioDiffAvgCount(0),
				  pAudioLayout(Speakers::Stereo)
			{
				setThreaded(false);
			}

			bool AudioDecoder::load(VideoStream *stream)
			{
				// - Check if we are loading an audio stream
				if (stream->getMediaType() != VideoStream::Audio)
				{
					qDebug("Trying to load non audio stream in a audio decoder.");
					return false;
				}

				return Decoder::load(stream);
			}

			bool AudioDecoder::loadInternal()
			{
				// - Get codec context
				const AVCodecContext *pCodecCtx = getCodecContext();

				// - Alloc a new resampler
				pAudioResampler = swr_alloc();

				if (!pAudioResampler)
				{
					qDebug("Unable to load audio resampler.");
					return false;
				}

				int layout = pAudioLayout < 0 ? pCodecCtx->channel_layout : pAudioLayout;

				// - Set up SWR context once you've got codec information
				av_opt_set_int(pAudioResampler, "in_channel_layout",  pCodecCtx->channel_layout, 0);
				av_opt_set_int(pAudioResampler, "out_channel_layout", layout,  0);
				av_opt_set_int(pAudioResampler, "in_sample_rate", pCodecCtx->sample_rate, 0);
				av_opt_set_int(pAudioResampler, "out_sample_rate", pCodecCtx->sample_rate, 0);
				av_opt_set_sample_fmt(pAudioResampler, "in_sample_fmt", pCodecCtx->sample_fmt, 0);
				av_opt_set_sample_fmt(pAudioResampler, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

				swr_init(pAudioResampler);

				pAudioBuffer = new uint8_t[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
				if (!pAudioBuffer)
				{
					qDebug("Cannot allocate an audio buffer.");
					swr_free(&pAudioResampler);
					return false;
				}

				pFrameTimer = (av_gettime() / 1000000.0);
				pLastDelay = 40e-3;

				pFrame = av_frame_alloc();
				if (!pFrame)
				{
					qDebug("Cannot allocate a new frame.");
					swr_free(&pAudioResampler);
					return false;
				}

				memset(&pPacket, 0, sizeof(pPacket));

				return true;
			}

			void AudioDecoder::setupDecoder()
			{

			}

			void AudioDecoder::unloadInternal()
			{
				// - Delete frame
				av_frame_free(&pFrame);

				// - Delete audio resampler
				swr_free(&pAudioResampler);

				// - Free audio buffer
				delete[] pAudioBuffer;
			}

			int AudioDecoder::getSamplesPerFrame() const
			{
				return pSamplesPerFrame;
			}

			Speakers::Layout AudioDecoder::getChannelLayout() const
			{
				if (!isLoaded())
					return Speakers::UnknowLayout;

				int layout = pAudioLayout < 0 ? getCodecContext()->channel_layout : pAudioLayout;

				return static_cast<Speakers::Layout>(layout);
			}

			int AudioDecoder::getChannelCount() const
			{
				int layout = getChannelLayout();

				if (layout == 0)
					return 0;

				auto a = av_get_channel_layout_nb_channels(layout);
				return a;
			}

			int AudioDecoder::getBufferSize() const
			{
				return getSamplesPerFrame() * getChannelCount() * (getBitDepth() / 8);
			}

			int AudioDecoder::getSampleRate() const
			{
				if (!isLoaded())
					return 0;

				int rate = getCodecContext()->sample_rate;

				return rate;
			}

			int AudioDecoder::getBitDepth() const
			{
				return 16;
			}

			void AudioDecoder::pause()
			{
				Decoder::pause();
			}

			void AudioDecoder::start()
			{
				Decoder::start();
			}

			void AudioDecoder::seek(double time)
			{
				Decoder::seek(time);
			}

			double AudioDecoder::getSeconds() const
			{
                double apts = Clock::getSeconds();
				int hwBufferSize = pAudioBufferSize - pAudioBufferIndex;
				int freq = getSampleRate() * getChannelCount() * 2;

				if(freq)
				{
					apts -= (double)hwBufferSize / freq;
				}

				return apts;
			}

            void AudioDecoder::processData(QIODevice *samples, int size)
            {
                int len = size, len1;

                char * data = new char[size];
                uint8_t *b = (uint8_t*)data;

                // - Check if paused
                if (getMasterClock()->isPaused())
                {
					memset(data, 0, size);
					samples->write(data, size);
					delete[] data;
                    return;
                }

                while(len > 0)
                {
                    if(pAudioBufferIndex >= pAudioBufferSize)
                    {
                        // - We have already sent all our data; get more
                        int audioSize = 0;

                        audioSize = decodeFrame(pAudioBuffer, (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2);

                        if(audioSize < 0)
                        {
							// - If error, output silence
							pAudioBufferSize = size;
							memset(pAudioBuffer, 0, pAudioBufferSize);
                        }
                        else
                        {
                            audioSize = syncronizeAudio((int16_t*)pAudioBuffer, audioSize);
                            pAudioBufferSize = audioSize;
                        }

                        pAudioBufferIndex = 0;
                    }

                    len1 = pAudioBufferSize - pAudioBufferIndex;

                    if(len1 > len)
                        len1 = len;

                    memcpy(b, pAudioBuffer + pAudioBufferIndex, len1);

                    len -= len1;
                    b += len1;
                    pAudioBufferIndex += len1;
                }

                samples->write(data, size);
				delete[] data;
            }

			int AudioDecoder::decodeFrame(uint8_t *audioBuffer, int bufferSize)
			{
				AVCodecContext *pCodecCtx = getCodecContext();
				AVPacket *pkt = &pPacket;

				int audioFrameFinish = 0;
				int audioDataSize = 0;
				double pts = 0;

				int maxBufferSize = av_samples_get_buffer_size(NULL, getChannelCount(), pSamplesPerFrame, AV_SAMPLE_FMT_S16, 1);
				uint8_t *buffer = (uint8_t*)av_malloc(maxBufferSize);

				for (;;)
				{
					while(pAudioPacketSize > 0)
					{

						// - Decode audio frame
						int size = avcodec_decode_audio4(pCodecCtx, pFrame, &audioFrameFinish, pkt);

						if(size < 0)
						{
							// if error, skip frame
							pAudioPacketSize = 0;
							break;
						}

						int tdata = 0;

						if(audioFrameFinish)
						{
							// convert input frame to output buffer
							int samples = swr_convert(
								pAudioResampler,
								&buffer,
								pSamplesPerFrame,
								(const uint8_t **)pFrame->data,
								pFrame->nb_samples
							);

							if (samples < 0)
							{
								continue;
							}

							while (samples > 0)
							{
								audioDataSize = av_samples_get_buffer_size(
									NULL,
									getChannelCount(),
									samples,
									AV_SAMPLE_FMT_S16,
									1
								);

								if (!(tdata + audioDataSize <= bufferSize))
								{
									qDebug("Critical error on audio decode.");
									av_freep(&buffer);
									exit(0);
								}

								memcpy(audioBuffer + tdata, buffer, audioDataSize);
								tdata += audioDataSize;

								// process samples buffered inside swr context
								samples = swr_convert(pAudioResampler, &buffer, pSamplesPerFrame, NULL, 0);
								if (samples < 0)
								{
									continue;
								}
							}
						}

						pAudioPacketData += size;
						pAudioPacketSize -= size;

						pts = getSeconds();
						increment((double)tdata / (double)(getChannelCount() * 2 * getSampleRate()));

						av_freep(&buffer);
						return tdata;
					}

					if (pkt->data)
						av_free_packet(pkt);

					pPacket = getPacket();
					if (pPacket.pos < 0)
					{
						av_freep(&buffer);
						return -1;
					}

					// if update, update the audio clock w/pts
					if (pkt->pts != AV_NOPTS_VALUE)
					{
						setTime(getStream()->getTimeBase() * pkt->pts);
					}

					pAudioPacketSize = pkt->size;
					pAudioPacketData = pkt->data;
				}

				av_freep(&buffer);
			}

			int AudioDecoder::syncronizeAudio(short *samples, int sampleSize)
			{
				AVCodecContext *pCodecCtx = getCodecContext();
				int n; double ref_clock;

				n = 2 * getChannelCount();

				if (!isInternalClock())
				{
					double diff, avgDiff;
					int wantedSize, minSize, maxSize;

                    ref_clock = getMasterClock()->getSeconds();
                    diff = getSeconds() - ref_clock;

					if (fabs(diff) < AV_NOSYNC_THRESHOLD)
					{
						// accumulate the diffs
						pAudioDiffCum = diff + pAudioDiffCoef * pAudioDiffCum;
						if (pAudioDiffAvgCount < AUDIO_DIFF_AVG_NB)
						{
							pAudioDiffAvgCount++;
						}
						else
						{
							avgDiff = pAudioDiffCum * (1.0 - pAudioDiffCoef);
							if (fabs(avgDiff) >= pAudioDiffTreshold)
							{
								wantedSize = sampleSize + ((int)(diff * pCodecCtx->sample_rate) * n);
								minSize = sampleSize * ((100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100);
								maxSize = sampleSize * ((100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100);
								if (wantedSize < minSize)
								{
									wantedSize = minSize;
								}
								else if (wantedSize > maxSize)
								{
									wantedSize = maxSize;
								}
								if (wantedSize < sampleSize)
								{
									/* remove samples */
									sampleSize = wantedSize;
								}
								else if (wantedSize > sampleSize)
								{
									uint8_t *samples_end, *q;
									int nb;

									/* add samples by copying final sample*/
									nb = (sampleSize - wantedSize);
									samples_end = (uint8_t*)samples + sampleSize - n;
									q = samples_end + n;
									while (nb > 0)
									{
										memcpy(q, samples_end, n);
										q += n;
										nb -= n;
									}
									sampleSize = wantedSize;
								}
							}
						}
					}
					else
					{
						/* difference is TOO big; reset diff stuff */
						pAudioDiffAvgCount = 0;
						pAudioDiffCum = 0;
					}
				}
				return sampleSize;
			}

			void AudioDecoder::waitForNextSamples()
			{
				pFrameTimer += pSamplesPerFrame / (double)getSampleRate();

				// - compute the REAL delay
				double actualDelay = pFrameTimer - (av_gettime() / 1000000.0);

				if (actualDelay < 0)
					actualDelay = 0;

				av_usleep((int)(actualDelay * 1000000.0));
			}


		}
	}
}
