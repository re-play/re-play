#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <QAudioBuffer>
#include <QIODevice>

#include <core/decoder.h>
#include <core/audio/speakers.h>

extern "C"
{
	#include <libswresample/swresample.h>
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/time.h>
	#include <libavutil/opt.h>
}

namespace RePlay
{
	namespace Core
	{
		namespace Audio
		{

			class AudioDecoder : public Decoder
			{
				Q_OBJECT

				Q_PROPERTY(int bufferSize READ getBufferSize NOTIFY loaded)

			public:

				explicit AudioDecoder();

				// - Stream management
				// ----------------------------------------
				Q_INVOKABLE bool load(VideoStream *stream) override;

				// - Management
				// ----------------------------------------
				Q_INVOKABLE void pause() override;
				Q_INVOKABLE void start() override;
				Q_INVOKABLE void seek(double time) override;

				// - Time data
				// ----------------------------------------
				double getSeconds() const override;

				// - Audio info
				// ----------------------------------------
				Speakers::Layout getChannelLayout() const;
				Q_INVOKABLE int getSamplesPerFrame() const;
				Q_INVOKABLE int getChannelCount() const;
				Q_INVOKABLE int getBufferSize() const;
				Q_INVOKABLE int getSampleRate() const;
				Q_INVOKABLE int getBitDepth() const;

			protected:

				// - Stream management
				// ----------------------------------------
				bool loadInternal() override;
				void setupDecoder() override;
				void unloadInternal() override;

				// - Frame decoding
				// ---------------------------------------
				void processFrame() override {}

			signals:

				void loaded(bool loaded);
				void newSample(QByteArray samples);

			public slots:

                void processData(QIODevice *samples, int size);


			private:

				// - AVLibs audio references
				// ----------------------------------------
				SwrContext	* pAudioResampler;
				AVFrame		* pFrame;

				// - Packet frammentation
				// ----------------------------------------
				AVPacket	  pPacket;
				uint8_t		* pAudioPacketData = NULL;
				int			  pAudioPacketSize = 0;

				// - Audio buffer
				// ----------------------------------------
				uint8_t		* pAudioBuffer;
				int			  pAudioBufferIndex;
				int			  pAudioBufferSize;
				int			  pSamplesPerFrame;

				// - Timing
				// ----------------------------------------
				double		  pFrameTimer;
				double		  pLastDelay;

				double		  pAudioDiffCoef;
				double		  pAudioDiffTreshold;
				double		  pAudioDiffCum;
				double		  pAudioDiffAvgCount;

				double		  pSyncThreshold;

				// - Audio layout
				// ----------------------------------------
				Speakers::Layout	pAudioLayout;

				// - Process routines
				// ----------------------------------------
				int decodeFrame(uint8_t *buffer, int size);
				int syncronizeAudio(short *samples, int size);
				void waitForNextSamples();

			};

		}
	}
}

#endif // AUDIODECODER_H
