#ifndef DECODER_H
#define DECODER_H

#include <core/timing/clock.h>
#include <core/videostream.h>

#include <QAtomicInt>
#include <QThread>
#include <QMutex>

#include <utils/qconcurrentqueue.h>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
    #include <libavutil/opt.h>
}

namespace RePlay
{
	namespace Core
	{
		using PacketQueue = QConcurrentQueue<AVPacket>;

		class Decoder : public Timing::Clock
		{

			Q_OBJECT

			Q_PROPERTY(DecoderState state READ getState NOTIFY loaded)
			Q_PROPERTY(QObject* stream READ getStream NOTIFY stateChanged)
			Q_PROPERTY(QObject* masterClock READ pGetMasterClock WRITE pSetMasterClock NOTIFY masterClockChanged)
			Q_PROPERTY(bool queueFull READ isQueueFull NOTIFY queueChanged)
			Q_PROPERTY(double queueTime READ getQueueTime NOTIFY queueChanged)
			Q_PROPERTY(int queueSpace READ getQueueSpace NOTIFY queueChanged)
			Q_PROPERTY(int queueSize READ getQueueSize NOTIFY queueChanged)
			Q_PROPERTY(int queueLoad READ getQueueLoad NOTIFY queueChanged)
			Q_PROPERTY(int queueLimit READ getQueueLimit WRITE setQueueLimit NOTIFY queueChanged)

		public:

			enum DecoderState { Unknow = -1, NotLoaded, Idle, Decoding };
			Q_ENUMS(DecoderState)

		public:

			explicit Decoder();


			// - Stream management
			// ----------------------------------------
			Q_INVOKABLE virtual bool load(VideoStream *stream);
			Q_INVOKABLE virtual void unload();

			// - Management
			// ----------------------------------------
			Q_INVOKABLE virtual void pause() override;
			Q_INVOKABLE virtual void start() override;
			Q_INVOKABLE virtual void seek(double time) override;

			// - Queue management
			// ----------------------------------------
			Q_INVOKABLE void feed(AVPacket &packet, bool copy = true);
			Q_INVOKABLE void clearQueue();

			// - Queue info
			// ----------------------------------------
			int				getQueueSize();
			int				getQueueLoad();
			int				getQueueLimit();
			int				getQueueSpace();
			double			getQueueTime();
			bool			isQueueFull();
			void			setQueueLimit(int limit);

			// - State
			// ----------------------------------------
			VideoStream	  * getStream() const;
			DecoderState	getState() const;
			bool			isLoaded() const;

		protected:

			// - Codec handlers
			// ---------------------------------------
			AVCodecContext	* getCodecContext() const;
			AVCodec			* getCodec() const;

			// - Timing
			// ----------------------------------------
			Timing::Clock	* getMasterClock() const;
			bool			  isInternalClock() const;

			void	setMasterClock(Timing::Clock *clock);

			// - Stream management
			// ----------------------------------------
			bool virtual loadInternal() = 0;
			void virtual setupDecoder() = 0;
			void virtual unloadInternal() = 0;

			// - State
			// ----------------------------------------
			void setState(DecoderState state);
			void setThreaded(bool threaded);

			// - Frame decoding
			// ----------------------------------------
			virtual void processFrame() = 0;

			// - Queue management
			// ----------------------------------------
			AVPacket const &getPacket();


		signals:

			void queueChanged();
			void loaded(bool loaded);
			void masterClockChanged();
			void stateChanged(DecoderState state);

		protected slots:

			void process();

		public slots:


		private:

			// - Codec handlers
			// -------------------------------
			AVCodecContext	* pCodecCtx;
			AVCodec			* pCodec;

			// - Current stream
			// -------------------------------
			VideoStream		* pStream;

			// - State
			// -------------------------------
			QAtomicInt		  pState;

			// - Queue
			// -------------------------------
			PacketQueue		  pQueue;
			QMutex			  pMutex;
			int				  pAdjustableLimit;
			int				  pQueueLimit;

			// - Threading
			// -------------------------------
			QThread			* pThread;
			bool			  pThreaded;

			// - Timing
			// -------------------------------
			Timing::Clock	* pMasterClock;

			// - Private getter and setters
			// -------------------------------
			QObject			* pGetStream() const;
			QObject			* pGetMasterClock() const;
			void			  pSetMasterClock(QObject *clock);

		};

	}
}

#endif // DECODER_H
