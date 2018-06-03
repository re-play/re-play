#include "core/videoreader.h"

using namespace RePlay::Core::Video;

namespace RePlay
{
	namespace Core
	{
		VideoReader::VideoReader()
			: QObject(),
			  pFormatCtx(nullptr),
			  pReaderThread(nullptr),
			  pVideoStreams(),
			  pAudioStreams(),
			  pSubsStreams()
		{
		}

		VideoReader::~VideoReader()
		{
			unloadVideo();
			pDecoders.clear();
		}

		bool VideoReader::loadVideo(QString path)
		{
			// - If a video is already loaded we must dispose it
			if (isLoaded())
				unloadVideo();

			pPath = path;

			// - Alloc a new context
			if (!(pFormatCtx = avformat_alloc_context()))
			{
				qDebug("Unable to alloc format context.");

				pState = NotLoaded;
				emit videoLoaded(isLoaded());
				return false;
			}

			// - Load the source in the context
			if (avformat_open_input(&pFormatCtx, path.toLocal8Bit().data(), nullptr, nullptr) < 0)
			{
				qDebug("Unable to load " + path.toLocal8Bit());
				avformat_free_context(pFormatCtx);

				pState = NotLoaded;
				emit videoLoaded(isLoaded());
				return false;
			}

			// - Inject metadata
			av_format_inject_global_side_data(pFormatCtx);

			// - Get stream informations
			if(avformat_find_stream_info(pFormatCtx, nullptr) < 0)
			{
				qDebug("Cannot retrive file informations.");
				avformat_close_input(&pFormatCtx);
				avformat_free_context(pFormatCtx);

				pState = NotLoaded;
				emit videoLoaded(isLoaded());
				return false;
			}

			// - Find the first video stream
			for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
			{
				VideoStream *currStream = new VideoStream(pFormatCtx->streams[i]);

				if (currStream->getMediaType() == VideoStream::MediaType::Video)
					pVideoStreams << currStream;
				if (currStream->getMediaType() == VideoStream::MediaType::Audio)
					pAudioStreams << currStream;
				if (currStream->getMediaType() == VideoStream::MediaType::Subtitle)
					pSubsStreams << currStream;
			}

			// - Emit loaded event and set loaded
			pState = Paused;
			emit videoLoaded(isLoaded());

			// - Make the loader thread and start
			pReaderThread = new QThread();
			connect(pReaderThread, SIGNAL(started()), this, SLOT(process()), Qt::DirectConnection);
			connect(pReaderThread, SIGNAL(finished()), this, SLOT(processEnd()));
			pReaderThread->start();

			return isLoaded();
		}

		void VideoReader::unloadVideo()
		{
			if (!isLoaded())
				return;

			// - Stop reading thread
			pState = NotLoaded;
			pReaderThread->wait(5000);
			delete pReaderThread;

			for (auto decoder : pDecoders)
			{
				decoder->clearQueue();
			}

			// - Close avformat
			avformat_close_input(&pFormatCtx);
			avformat_free_context(pFormatCtx);

			// - Delete all streams
			for (auto stream : pVideoStreams)
				delete stream;
			for (auto stream : pAudioStreams)
				delete stream;
			for (auto stream : pSubsStreams)
				delete stream;

			pVideoStreams.clear();
			pAudioStreams.clear();
			pSubsStreams.clear();

			emit videoLoaded(isLoaded());
		}

		void VideoReader::seek(int ms)
		{
			// TODO: Check if we must throw an exception
			if (pState == NotLoaded || pState == Stopped)
				return;

			if (ms >= 0 && ms <= getDuration() / 1000)
				pSeekRequest = ms;
		}

		void VideoReader::stop()
		{
			pState = Stopped;
		}

		void VideoReader::play()
		{
			// TODO: Check if we must throw an exception
			if (pState == NotLoaded)
				return;

			// - If alredy playing do nothing
			if (pState == Playing)
				return;

			// - If video was stopped return to 0
			if (pState == Stopped)
			{
				pState = Playing;
				seek(0);
			}

			// - If video was paused set to playing
			if (pState == Paused)
				pState = Playing;
		}

		void VideoReader::process()
		{
			AVPacket packet;
			bool seekCycle = false;

			while (isLoaded())
			{
				int res = AVERROR_EOF;
				bool wait = false;

				pMutex.lock();

				// - If video stopped we wait for something
				if (pState == Stopped)
				{
					QThread::usleep(10000);
					pMutex.unlock();
					continue;
				}

				if (pSeekRequest >= 0)
				{
					auto videoStream = pVideoStreams[0]->getHandler();

					if (av_seek_frame(pFormatCtx, videoStream->index, pSeekRequest * videoStream->time_base.den / videoStream->time_base.num,AVSEEK_FLAG_ANY) >= 0)
					{
						for (auto decoder : pDecoders)
						{
							decoder->clearQueue();
							decoder->seek(pSeekRequest);
						}
					}

					pSeekRequest = -1;
					seekCycle = true;
				}

				// - Check if a queue is full
				for (auto decoder : pDecoders)
					if (decoder->isQueueFull())
						wait = true;

				// - If we have a decode queue full we should wait
				if (wait)
				{
					QThread::usleep(10000);
					pMutex.unlock();
					continue;
				}

				// - Read a packet from file
				if ((res = av_read_frame(pFormatCtx, &packet)) >= 0)
				{
					for (auto decoder : pDecoders)
					{
						if (decoder->isLoaded() && decoder->getStream()->getIndex() == packet.stream_index)
						{
							// - We can send this packet to this decoder
							decoder->feed(packet);
						}
					}

					// - Free the packet that was allocated by av_read_frame
					av_packet_unref(&packet);
				}
				else if (res == AVERROR_EOF)
				{
					// - Handle video end
					pState = Stopped;
					emit videoEnd();
				}

				if (seekCycle)
				{
					for (auto decoder : pDecoders)
					{
						auto video = dynamic_cast<VideoDecoder*>(decoder);
						if (video)
							video->showFrame();
					}

					seekCycle = false;
				}

				pMutex.unlock();
			}

			pReaderThread->exit();
		}

		void VideoReader::registerDecoder(Decoder *decoder)
		{
			pMutex.lock();
			pDecoders.append(decoder);
			// TODO: check this
			//connect(decoder, SIGNAL(destroyed()), this, SLOT(cancelDecoder(decoder)));
			pMutex.unlock();
			emit decoderRegistered();
		}

		void VideoReader::cancelDecoder(Decoder *decoder)
		{
			pMutex.lock();
			pDecoders.removeOne(decoder);
			decoder->clearQueue();
			pMutex.unlock();
		}

		void VideoReader::processEnd()
		{
		}

		QString VideoReader::getPath() const
		{
			return pPath;
		}

		QString VideoReader::getName() const
		{
			return QString(pFormatCtx->filename);
		}

		int VideoReader::getDuration() const
		{
			return pFormatCtx ? pFormatCtx->duration : 0;
		}

		bool VideoReader::isRealtime() const
		{
			auto format = pFormatCtx->iformat;
			if (!strcmp(format->name, "rtp") || !strcmp(format->name, "rtsp") || !strcmp(format->name, "sdp"))
				return true;

			if (pFormatCtx->pb && (!strncmp(pFormatCtx->url, "rtp:", 4) || !strncmp(pFormatCtx->url, "udp:", 4)))
				return true;

			return false;
		}

		bool VideoReader::isLoaded() const
		{
			return pState != NotLoaded && pState != Unknow;
		}

		QList<Decoder*> const &VideoReader::getDecoderSubscriptions() const
		{
			return pDecoders;
		}

		QList<VideoStream*> const &VideoReader::getVideoStreams() const
		{
			return pVideoStreams;
		}

		QList<VideoStream*> const &VideoReader::getAudioStreams() const
		{
			return pAudioStreams;
		}

		QList<VideoStream*> const &VideoReader::getSubsStreams() const
		{
			return pSubsStreams;
		}

		QList<QObject*> VideoReader::pGetVideoStreams() const
		{
			return *reinterpret_cast<const QList<QObject *>*>(&pVideoStreams);
		}

		QList<QObject*> VideoReader::pGetAudioStreams() const
		{
			return *reinterpret_cast<const QList<QObject *>*>(&pAudioStreams);
		}

		QList<QObject*> VideoReader::pGetSubsStreams() const
		{
			return *reinterpret_cast<const QList<QObject *>*>(&pSubsStreams);
		}

		QList<QObject*> VideoReader::pGetDecoders() const
		{
			return *reinterpret_cast<const QList<QObject *>*>(&pDecoders);
		}

	}
}
