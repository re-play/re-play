#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QImage>

#include <core/decoder.h>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/time.h>
}

namespace RePlay
{
	namespace Core
	{
		namespace Video
		{

			class VideoDecoder : public Decoder
			{
				Q_OBJECT

				Q_PROPERTY(QString codecName READ getCodecName NOTIFY graphicsReloaded)
				Q_PROPERTY(size_t framebufferSize READ getFramebufferSize NOTIFY graphicsReloaded)
				Q_PROPERTY(int width READ getWidth NOTIFY graphicsReloaded)
				Q_PROPERTY(int height READ getHeight NOTIFY graphicsReloaded)

			public:

				explicit VideoDecoder();

				// - Stream management
				// ----------------------------------------
				Q_INVOKABLE bool load(VideoStream *stream) override;

				// - Management
				// ----------------------------------------
				Q_INVOKABLE void pause() override;
				Q_INVOKABLE void start() override;
				Q_INVOKABLE void seek(double time) override;
				Q_INVOKABLE void showFrame();

				// - Informations
				// ----------------------------------------
				QString getCodecName() const;
				size_t	getFramebufferSize() const;
				int		getWidth() const;
				int		getHeight() const;

			protected:

				// - Stream management
				// ----------------------------------------
				bool loadInternal() override;
				void setupDecoder() override;
				void unloadInternal() override;

				// - Frame decoding
				// ---------------------------------------
				void processFrame() override;

			signals:

				void graphicsReloaded();
				void newFrame(QImage frame);

			public slots:


			private:

				// - FFmpeg video
				// ---------------------------------------
				SwsContext	* pVideoResampler;
				AVFrame		* pFrame;

				// - Timing
				// ---------------------------------------
				double pFrameTimer = 0;
				double pLastPts = 0;
				double pLastDelay = 0;
				double actualDelay = 0;
				double pCurrentVideoPts = 0;
				double pCurrentVideoPtsTime = 0;

				double pSyncThreshold;
				double pNoSyncThreshold;
				double pFramedupThreshold;

				// - Frame skip
				// ----------------------------------------
				bool pShowFrame = false;
			};

		}
	}
}

#endif // VIDEODECODER_H
