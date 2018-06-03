#include "core/decoder.h"

using namespace RePlay::Core::Timing;

namespace RePlay
{
	namespace Core
	{

		Decoder::Decoder()
			: Clock(),
			  pCodecCtx(nullptr),
			  pStream(nullptr),
			  pCodec(nullptr),
			  pThread(nullptr),
			  pMasterClock(this),
			  pThreaded(true)
		{

		}

		bool Decoder::load(VideoStream *stream)
		{
			// - If there's a codec already loaded we must unload
			if (pStream == stream)
				return true;

			if (isLoaded())
				unload();

			const AVStream *st = stream->getHandler();

			// - Alloc a new video context
			pCodecCtx = avcodec_alloc_context3(nullptr);

			if (!pCodecCtx)
			{
				qDebug("Cannot alloc codec context.");
				return false;
			}

			// - Set context data
			if (avcodec_parameters_to_context(pCodecCtx, st->codecpar) < 0)
			{
				qDebug("Cannot set codec context parameters.");
				avcodec_free_context(&pCodecCtx);
				pCodecCtx = nullptr;
				return false;
			}

			// - Set context base time
			pCodecCtx->pkt_timebase = st->time_base;

			// - Get decoder for this stream
			pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

			if (!pCodec)
			{
				qDebug("Cannot load codec.");
				avcodec_free_context(&pCodecCtx);
				pCodecCtx = nullptr;
				return false;
			}

			setupDecoder();

			// - Open the loaded codec
			if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
			{
				qDebug("Cannot open codec.");
				avcodec_free_context(&pCodecCtx);
				pCodecCtx = nullptr;
				pCodec = nullptr;
				return false;
			}

			// - Load subclasses
			if (!loadInternal())
			{
				avcodec_free_context(&pCodecCtx);
				pCodecCtx = nullptr;
				pCodec = nullptr;
				return false;
			}

			pStream = stream;
			setState(Idle);

			// - Open worker thread
			if (pThreaded)
			{
				pThread = new QThread();
				connect(pThread, SIGNAL(started()), this, SLOT(process()), Qt::DirectConnection);
				pThread->start();
			}

			connect(stream, SIGNAL(destroyed()), this, SLOT(unload()));

			emit loaded(isLoaded());
			return isLoaded();
		}

		void Decoder::unload()
		{
			if (!isLoaded())
				return;

			setState(NotLoaded);

			AVPacket pkt;
			feed(pkt, false);

			// - Wait thread end
			if (pThreaded)
			{
				pThread->wait(5000);
				delete pThread;
			}

			// - Clear packet queue
			pQueue.clear();
			emit queueChanged();

			// - Unload subclasses
			unloadInternal();

			// - Close codec
			avcodec_close(pCodecCtx);

			// - Free codec context
			avcodec_free_context(&pCodecCtx);

			pCodecCtx = nullptr;
			pCodec = nullptr;
			pStream = nullptr;

			seek(0);

			emit loaded(isLoaded());
		}

		void Decoder::process()
		{
			while (isLoaded())
			{
				pMutex.lock();

				processFrame();

				pMutex.unlock();
			}

			pThread->exit();
		}

		VideoStream *Decoder::getStream() const
		{
			return pStream;
		}

		Decoder::DecoderState Decoder::getState() const
		{
			return static_cast<Decoder::DecoderState>(pState.load());
		}

		void Decoder::setThreaded(bool threaded)
		{
			pThreaded = threaded;
		}

		bool Decoder::isLoaded() const
		{
			return getState() != Unknow && getState() != NotLoaded;
		}

		void Decoder::pause()
		{
			Clock::pause();
		}

		void Decoder::start()
		{
			Clock::start();
		}

		void Decoder::seek(double time)
		{
			Clock::seek(time);
		}

		void Decoder::feed(AVPacket &packet, bool copy)
		{
			if (copy)
			{
				AVPacket newpacket;
				av_init_packet(&newpacket);

				// - Duplicate the packet
				av_packet_ref(&newpacket, &packet);

				pQueue.enqueue(newpacket);
			}
			else
			{
				AVPacket newPacket;

				// - Move the packet
				av_packet_move_ref(&newPacket, &packet);

				pQueue.enqueue(newPacket);
			}

			emit queueChanged();
		}

		AVPacket const &Decoder::getPacket()
		{
			return pQueue.dequeue();
		}

		void Decoder::clearQueue()
		{
			while (pQueue.size() > 0)
			{
				auto packet = pQueue.dequeue();

				// - Remove packet data
				av_packet_unref(&packet);
			}

			emit queueChanged();
		}

		Clock * Decoder::getMasterClock() const
		{
			return pMasterClock;
		}

		bool Decoder::isInternalClock() const
		{
			return pMasterClock == this;
		}

		void Decoder::setMasterClock(Clock *clock)
		{
			pMasterClock = clock;
			emit masterClockChanged();
		}

		int Decoder::getQueueSize()
		{
			return pQueue.size();
		}

		double Decoder::getQueueTime()
		{
			if (getQueueSize() <= 0)
				return 0;

			return (pQueue.last().pts - pQueue.first().pts) * getStream()->getTimeBase();
		}

		int Decoder::getQueueSpace()
		{
			if (getQueueSize() <= 0)
				return 0;

			return pQueue.first().size * getQueueSize();
		}

		int Decoder::getQueueLoad()
		{
			return getQueueSize() / pQueueLimit;
		}

		bool Decoder::isQueueFull()
		{
			auto size = getQueueSize();
			return size >= pAdjustableLimit || size >= pQueueLimit;
		}

		int Decoder::getQueueLimit()
		{
			return pQueueLimit;
		}

		void Decoder::setQueueLimit(int limit)
		{
			pQueueLimit = limit;

			// TODO: set it automatically
			pAdjustableLimit = limit;
		}

		AVCodecContext *Decoder::getCodecContext() const
		{
			return pCodecCtx;
		}

		AVCodec *Decoder::getCodec() const
		{
			return pCodec;
		}

		void Decoder::setState(DecoderState state)
		{
			pState = state;
			emit stateChanged(state);
		}

		QObject *Decoder::pGetMasterClock() const
		{
			return static_cast<QObject*>(getMasterClock());
		}

		QObject *Decoder::pGetStream() const
		{
			return static_cast<QObject*>(getStream());
		}

		void Decoder::pSetMasterClock(QObject *obj)
		{
			setMasterClock(static_cast<Clock*>(obj));
		}

	}
}
