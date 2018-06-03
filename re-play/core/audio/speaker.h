#ifndef SPEAKER_H
#define SPEAKER_H

#include <QObject>

#include <QBuffer>
#include <QIODevice>
#include <QAudioInput>
#include <QAudioOutput>
#include <QAudioBuffer>

namespace RePlay
{
	namespace Core
	{
		namespace Audio
		{

			class Speaker : public QObject
			{
				Q_OBJECT

			public:

				explicit Speaker();

				// - Device management
				// --------------------------------------
				Q_INVOKABLE void openAudioDevice(int sampleRate, int channels, int sampleSize, int buffSize);
				Q_INVOKABLE void closeAudioDevice();

			signals:

				void sourceChanged();
                void frameRequest(QIODevice *buffer, int size);

			public slots:

				// - Manage audio samples
				// -------------------------------------------------
				void receiveSample(QByteArray samples);

				void handleStateChanged(QAudio::State newState);
                void doggo();


			private:

				// - Audio api
				// -------------------------------------------------
				QAudioOutput	* pAudioOut;
				QIODevice		* pIo;
				QAudioFormat	  pFormat;

			};

		}
	}
}


#endif // SPEAKER_H
