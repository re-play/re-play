#include "core/video/videodecoder.h"


namespace RePlay
{
	namespace Core
	{
		namespace Video
		{
			VideoDecoder::VideoDecoder()
				: Decoder(),
				  pVideoResampler(nullptr),
				  pFrame(nullptr),
				  pSyncThreshold(0.01),
				  pNoSyncThreshold(10.0),
				  pFramedupThreshold(0.1)
			{

			}

			bool VideoDecoder::load(VideoStream *stream)
			{
				// - Check if we are loading a video stream
				if (stream->getMediaType() != VideoStream::Video)
				{
					qDebug("Trying to load non video stream in a video decoder.");
					return false;
				}

				return Decoder::load(stream);
			}

			bool VideoDecoder::loadInternal()
			{
				// - Get codec context
				const AVCodecContext *pCodecCtx = getCodecContext();

				// Initialize SWS context for software scaling
				pVideoResampler = sws_getContext
				(
					pCodecCtx->width,
					pCodecCtx->height,
					pCodecCtx->pix_fmt,
					pCodecCtx->width,
					pCodecCtx->height,
					AV_PIX_FMT_RGB24,
					SWS_BILINEAR,
					nullptr,
					nullptr,
					nullptr
				);

				if (!pVideoResampler)
				{
					qDebug("Cannot initialize the svs resampler.");
					return false;
				}

				// - Alloc a frame container
				pFrame = av_frame_alloc();

				if (!pFrame)
				{
					qDebug("Cannot istantiate a frame.");
					return false;
				}

				pFrameTimer = (av_gettime() / 1000000.0);

				emit graphicsReloaded();

				return true;
			}

			void VideoDecoder::setupDecoder()
			{
				auto pCodec = getCodec();
				auto pCodecCtx = getCodecContext();

				/*if(pCodec->capabilities & AV_CODEC_CAP_TRUNCATED)
				{
					pCodecCtx->flags |= AV_CODEC_FLAG_TRUNCATED;
				}

				pCodec->capabilities |= AV_CODEC_CAP_FRAME_THREADS;
				pCodecCtx->flags     |= AV_CODEC_FLAG_LOW_DELAY;
				pCodecCtx->flags2    |= AV_CODEC_FLAG2_FAST;

				pCodecCtx->skip_frame       =  AVDISCARD_NONREF;
				pCodecCtx->skip_loop_filter =  AVDISCARD_ALL;
				pCodecCtx->skip_idct        =  AVDISCARD_ALL;

				pCodecCtx->idct_algo        =  1;
				pCodecCtx->has_b_frames     =  0;
				pCodecCtx->refs             =  1;*/

				pCodecCtx->thread_type      = FF_THREAD_FRAME|FF_THREAD_SLICE;
				pCodecCtx->thread_count     =  QThread::idealThreadCount();

				//av_opt_set(pCodecCtx->priv_data, "preset", "ultrafast", 0);
				//av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);
			}

			void VideoDecoder::processFrame()
			{
				int res = 0;
				double pPts = 0;

				if (getMasterClock()->isPaused() && !pShowFrame)
				{
					av_usleep((int)(0.05 * 1000000.0));
					pFrameTimer -= pFrameTimer - (av_gettime() / 1000000.0);
					return;
				}
				else
					pShowFrame = false;

				// - Pop a packet
				auto packet = getPacket();
				AVCodecContext *ctx = getCodecContext();

				// - Send a packet to decode queue
				res = avcodec_send_packet(ctx, &packet);

				if (res)
				{
					switch (res)
					{
						case AVERROR(EAGAIN):
							break;
						case AVERROR_EOF:
							av_packet_unref(&packet);
							break;
						default:
							// TODO: check when delete packet
							return;
					}
				}

				for ( ; !res ; )
				{
					// - Receive decoded frame
					res = avcodec_receive_frame(ctx, pFrame);

					if (res)
					{
						switch (res)
						{
							case AVERROR(EAGAIN):
								// - decoded frame not ready yet
								continue;
							case AVERROR_EOF:
								// - buffer flushed
							default:
								av_packet_unref(&packet);
								return;
						}
					}

					// - Get current frame pts
					if ((pPts = (double)pFrame->best_effort_timestamp) == AV_NOPTS_VALUE)
						pPts = 0;
					pPts *= getStream()->getTimeBase();

					// - if we have pts, set video clock to it
					if (pPts != 0) setTime(pPts);
					// - if we aren't given a pts, set it to the clock
					else pPts = getSeconds();

					// - update the video clock
					double frameDelay = av_q2d(getCodecContext()->time_base);

					// - if we are repeating a frame, adjust clock accordingly
					frameDelay += pFrame->repeat_pict * (frameDelay * 0.5);
					increment(frameDelay);

					// - Save presentation timestamp for sync
					pCurrentVideoPtsTime = (double)av_gettime();

					// - he pts from last time
					double delay = pPts - pLastPts;
					if(delay <= 0 || delay >= 1.0)
					{
						// - if incorrect delay, use previous one
						delay = pLastDelay;
					}

					// - save for next time
					pLastDelay = delay;
					pLastPts = pPts;

					if (!isInternalClock())
					{
						// - update delay to sync to audio
						double diff = pPts - getMasterClock()->getSeconds();

						// - Skip or repeat the frame. Take delay into account FFPlay still doesn't "know if this is the best guess."
						double syncThreshold = (delay > pSyncThreshold) ? delay : pSyncThreshold;

						if(fabs(diff) < pNoSyncThreshold)
						{
							if(diff <= -syncThreshold)
							{
								delay = 0;
							}
							else if (diff >= syncThreshold && delay > pFramedupThreshold)
							{
								delay = delay + diff;
							}
							else if(diff >= syncThreshold)
							{
								delay = 2 * delay;
							}
						}
					}

					pFrameTimer += delay;

					// - compute the REAL delay
					actualDelay = pFrameTimer - (av_gettime() / 1000000.0);

					if(actualDelay < 0)
					{
                        actualDelay = 0;
                        //av_frame_unref(pFrame);
                        //av_packet_unref(&packet);
                        //return;
					}

					if (actualDelay > 0.1)
					{
						actualDelay = 0.1;
					}

                    av_usleep((int)(actualDelay * 1000000.0 + 500));

					uint8_t *data[1];
					int pitch[1];

					QImage img = QImage(getWidth(), getHeight(), QImage::Format_RGB888);

					data[0] = img.bits();
					pitch[0] = ctx->width * 3;

					// - Convert the image from its native format to RGB
					sws_scale(pVideoResampler, (uint8_t const * const *)pFrame->data,
						pFrame->linesize, 0, ctx->height,
						data, pitch);

					// - Notify new frame
					emit newFrame(img.copy());

					// - Delete packet and frame
					av_frame_unref(pFrame);
					av_packet_unref(&packet);

					// - Frame finished
					return;
				}
			}

			void VideoDecoder::pause()
			{
				Decoder::pause();
			}

			void VideoDecoder::start()
			{
				Decoder::start();
			}

			void VideoDecoder::seek(double time)
			{
				Decoder::seek(time);
			}

			void VideoDecoder::showFrame()
			{
				pShowFrame = true;
			}

			QString VideoDecoder::getCodecName() const
			{
				if (!isLoaded())
					return QString();

				return QString(getCodec()->name);
			}

			void VideoDecoder::unloadInternal()
			{
				// - Delete allocated frame
				av_frame_unref(pFrame);

				// - Delete allocated resampler
				sws_freeContext(pVideoResampler);

				pFrame = nullptr;
				pVideoResampler = nullptr;

				emit graphicsReloaded();
			}

			size_t VideoDecoder::getFramebufferSize() const
			{
				auto buffSize = getWidth() * getHeight();
				return  buffSize * 3;
			}

			int VideoDecoder::getWidth() const
			{
				auto codec = getCodecContext();
				return codec ? codec->width : 0;
			}

			int VideoDecoder::getHeight() const
			{
				auto codec = getCodecContext();
				return codec ? codec->height : 0;
			}
		}
	}
}
