#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <QObject>
#include <QQmlListProperty>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

namespace RePlay
{
	namespace Core
	{

		class VideoStream : public QObject
		{
			Q_OBJECT

			Q_PROPERTY(MediaType mediaType READ getMediaType)
			Q_PROPERTY(QString name READ getName)
			Q_PROPERTY(int id READ getIndex)

		public:

			typedef enum { Unknown = -1, Video, Audio, Data, Subtitle, Attachment, Nb } MediaType;
			Q_ENUMS(MediaType)

		public:

			explicit VideoStream(const AVStream *stream);
			explicit VideoStream();
			~VideoStream() {}

			// - Metadata
			// -------------------------------------
			MediaType	getMediaType() const;
			int			getIndex() const;
			QString		getName() const;

			// - Timing
			// -------------------------------------
			double		getTimeBase() const;

			// - Content
			// -------------------------------------
			const AVStream	*getHandler() const;

		signals:


		public slots:


		private:

			// - FFmpeg stream
			const AVStream *pStream;

		};

	}
}

#endif // VIDEOSTREAM_H
