#include "core/audio/speaker.h"

namespace RePlay
{
	namespace Core
	{
		namespace Audio
		{
			Speaker::Speaker()
				: QObject()
			{

			}

			void Speaker::openAudioDevice(int sampleRate, int channels, int sampleSize, int buffSize)
			{
                pFormat.setSampleRate(sampleRate <= 0 ? 44100 : sampleRate);
				pFormat.setChannelCount(channels);
				pFormat.setSampleSize(sampleSize);
				pFormat.setCodec("audio/pcm");
				pFormat.setByteOrder(QAudioFormat::LittleEndian);
				pFormat.setSampleType(QAudioFormat::SignedInt);

				auto deviceinfo = QAudioDeviceInfo::defaultOutputDevice();

				if(!deviceinfo.isFormatSupported(pFormat))
				{
                    qDebug("Audio format not supported");
                    pFormat = deviceinfo.nearestFormat(pFormat);
				}

                pAudioOut = new QAudioOutput(deviceinfo, pFormat, this);
				//pAudioOut->setBufferSize(1024 * 2 * 2 * 20);
                auto time = (1 / (double)sampleRate) * 1024 * 1000;
                pAudioOut->setNotifyInterval(time);
                connect(pAudioOut, SIGNAL(notify()), this, SLOT(doggo()));
                connect(pAudioOut, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

                pIo = pAudioOut->start();

                pIo->write(QByteArray(new char[pAudioOut->periodSize()]{0}, pAudioOut->periodSize()));
			}

			void Speaker::receiveSample(QByteArray samples)
			{
                pIo->write((char*)samples.data(), pAudioOut->bytesFree());
			}

			void Speaker::closeAudioDevice()
			{

			}

            void Speaker::doggo()
            {
                emit frameRequest(pIo, pAudioOut->periodSize());
            }

			void Speaker::handleStateChanged(QAudio::State newState)
			  {
				  switch (newState) {
					  case QAudio::IdleState:
						  // Finished playing (no more data)
						  //audio->stop();
						  //sourceFile.close();
						  break;

					  case QAudio::StoppedState:
						  // Stopped for other reasons

						  if (pAudioOut->error() != QAudio::NoError) {
							  // Error handling
							  auto e = pAudioOut->error();
							  qDebug(std::to_string(e).c_str());
							  //qDebug(e);
						  }
						  break;

					  default:
						  // ... other cases as appropriate
						  break;
				  }
			  }
		}
	}
}
