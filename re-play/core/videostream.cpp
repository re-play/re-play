#include "core/videostream.h"


namespace RePlay
{
	namespace Core
	{
		VideoStream::VideoStream(const AVStream *stream)
			: QObject(),
			  pStream(stream)
		{

		}

		VideoStream::VideoStream()
			: QObject(),
			  pStream(nullptr)
		{

		}

		VideoStream::MediaType VideoStream::getMediaType() const
		{
			return static_cast<MediaType>(pStream->codecpar->codec_type);
		}

		QString VideoStream::getName() const
		{
			auto res = av_dict_get(pStream->metadata, "language", nullptr, 0);
			return QString(res->value);
		}

		int VideoStream::getIndex() const
		{
			return pStream->index;
		}

		double VideoStream::getTimeBase() const
		{
			return av_q2d(pStream->time_base);
		}

		const AVStream * VideoStream::getHandler() const
		{
			return pStream;
		}

	}
}
