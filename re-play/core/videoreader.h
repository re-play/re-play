#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include <core/video/videodecoder.h>
#include <core/videostream.h>
#include <core/decoder.h>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

namespace RePlay
{
	namespace Core
	{

		class VideoReader : public QObject
		{
			Q_OBJECT

			Q_PROPERTY(QString path READ getPath NOTIFY videoLoaded)
			Q_PROPERTY(QString name READ getName NOTIFY videoLoaded)
			Q_PROPERTY(int length READ getDuration NOTIFY videoLoaded)
			Q_PROPERTY(bool realtime READ isRealtime NOTIFY videoLoaded)
			Q_PROPERTY(bool loaded READ isLoaded NOTIFY videoLoaded)
			Q_PROPERTY(QList<QObject*> decoders READ pGetDecoders NOTIFY decoderRegistered)
			Q_PROPERTY(QList<QObject*> videoStreams READ pGetVideoStreams NOTIFY videoLoaded)
			Q_PROPERTY(QList<QObject*> audioStreams READ pGetAudioStreams NOTIFY videoLoaded)
			Q_PROPERTY(QList<QObject*> subsStreams READ pGetSubsStreams NOTIFY videoLoaded)

		public:

			enum VideoState { Unknow = -1, NotLoaded, Playing, Paused, Stopped };
			Q_ENUMS(VideoState)

		public:

			explicit VideoReader();
			~VideoReader();

			// - Video management
			// -----------------------------------------
			Q_INVOKABLE bool loadVideo(QString path);
			Q_INVOKABLE void unloadVideo();
			Q_INVOKABLE void seek(int ms);
			Q_INVOKABLE void stop();
			Q_INVOKABLE void play();

			// - Metadata
			// -----------------------------------------
			QString getPath() const;
			QString getName() const;
			int		getDuration() const;
			bool	isRealtime() const;
			bool	isLoaded() const;

			// - Current state
			// -----------------------------------------
			VideoState getState() const;

			// - Decoders
			// -----------------------------------------
			Q_INVOKABLE void registerDecoder(Decoder *d);
			Q_INVOKABLE void cancelDecoder(Decoder *d);

			QList<Decoder*> const &getDecoderSubscriptions() const;

			// - Streams
			// -----------------------------------------
			QList<VideoStream*> const &getVideoStreams() const;
			QList<VideoStream*> const &getAudioStreams() const;
			QList<VideoStream*> const &getSubsStreams() const;

		signals:

			void decoderRegistered();
			void videoLoaded(bool loaded);
			void videoEnd();

		public slots:

		private slots:

			void process();
			void processEnd();

		private:

			// - State
			QString pPath;
			QAtomicInt pState;
			QAtomicInt pSeekRequest;

			// - FFmpeg video
			AVFormatContext *pFormatCtx;

			// - Video streams
			QList<VideoStream*> pVideoStreams;
			QList<VideoStream*> pAudioStreams;
			QList<VideoStream*> pSubsStreams;

			// - Decoders
			QList<Decoder*> pDecoders;

			// - Private getters
			// -----------------------------------------
			QList<QObject*> pGetVideoStreams() const;
			QList<QObject*> pGetAudioStreams() const;
			QList<QObject*> pGetSubsStreams() const;
			QList<QObject*> pGetDecoders() const;

		private:

			// - Thread
			QThread		* pReaderThread;
			QMutex		  pMutex;


		};
	}
}

#endif // VIDEOREADER_H
