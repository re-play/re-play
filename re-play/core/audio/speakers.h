#ifndef SPEAKERS_H
#define SPEAKERS_H

#include <QObject>
#include <QMap>

namespace RePlay
{
	namespace Core
	{
		namespace Audio
		{
			class Speakers
			{
				Q_GADGET

			public:

				enum Speaker
				{
					Unknow =				0x00000000,
					FrontLeft =				0x00000001,
					FrontRight =			0x00000002,
					FrontCenter =			0x00000004,
					LowFrequency =			0x00000008,
					BackLeft =				0x00000010,
					BackRight =				0x00000020,
					FrontLeftCenter =		0x00000040,
					FrontRightCenter =		0x00000080,
					BackCenter =			0x00000100,
					SideLeft =				0x00000200,
					SideRight =				0x00000400,
					TopCenter =				0x00000800,
					TopFrontLeft =			0x00001000,
					TopFrontCenter =		0x00002000,
					TopFrontRight =			0x00004000,
					TopBackLeft =			0x00008000,
					TopBackCenter =			0x00010000,
					TopBackRight =			0x00020000,
					StereoLeft =			0x20000000,
					StereoRight =			0x40000000,
					WideLeft =				0x80000000,
					WidrRight =				0x0000000100000000,
					SurroundDirectLeft =	0x0000000200000000,
					SurroundDirectRight =	0x0000000400000000,
					LowFrequencyTwo =		0x0000000800000000,
					NativeLyout =			0x8000000000000000
				};

				enum Layout
				{
					Auto =					-1,
					UnknowLayout =			Unknow,
					Mono =					FrontCenter,
					Stereo =				FrontLeft | FrontRight,
					Surround21 =			FrontLeft | FrontRight | LowFrequency,
					StereoCBack =			FrontLeft | FrontRight | BackCenter,
					Surround =				FrontLeft | FrontRight | FrontCenter,
					Surround22 =			FrontLeft | FrontRight | SideLeft | SideRight,
					SurroundQuad =			FrontLeft | FrontRight | BackLeft | BackRight,
					Surround6Front =		FrontLeft | FrontRight | SideLeft | SideRight | FrontLeftCenter| FrontRightCenter,
					Surround61Front =		FrontLeft | FrontRight | SideLeft | SideRight | FrontLeftCenter| FrontRightCenter | LowFrequency,
					Surround40 =			FrontLeft | FrontRight | FrontCenter | BackCenter,
					Surround31 =			FrontLeft | FrontRight | FrontCenter | LowFrequency,
					Surround5Back =			FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight,
					Surround5 =				FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight,
					Surround41 =			FrontLeft | FrontRight | FrontCenter | BackCenter | LowFrequency,
					SurroundHexagonal =		FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | BackCenter,
					Surround6 =				FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | BackCenter,
					Surround51 =			FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency,
					Surround51Back =		FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | LowFrequency,
					Surround7 =				FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | BackLeft | BackRight,
					Surround61 =			FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency | BackCenter,
					Surround61Back =		FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | LowFrequency | BackCenter,
					Surround7Front =		FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | FrontLeftCenter | FrontRightCenter,
					SurroundOctagonal =		FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | BackLeft | BackCenter | BackRight,
					Surround71 =			FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency | BackLeft | BackRight,
					Surround71Wide =		FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency | FrontLeftCenter | FrontRightCenter,
					Surround71WideBack =	FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | LowFrequency | FrontLeftCenter | FrontRightCenter,
					StereoDownmix =			StereoLeft | StereoRight
				};

				Q_DECLARE_FLAGS(Spks, Speaker)
				Q_ENUMS(Layout)

			public:

				explicit Speakers();

				// - Conversion
				// ---------------------------------------------------
				static long					fromString(QString val);
				static QString				toString(long val);
				static QMap<long, QString> & getValues();

			private:

				static QMap<long, QString> pMap;


			};
		}
	}
}

#endif
