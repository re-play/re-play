#include "core/audio/speakers.h"

namespace RePlay
{
	namespace Core
	{
		namespace Audio
		{
			Speakers::Speakers()
			{

			}

			QMap<long, QString> Speakers::pMap =
			QMap<long, QString>{
				{ 0x00000001, "FrontLeft" },
				{ 0x00000002, "FrontRight" },
				{ 0x00000004, "FrontCenter" },
				{ 0x00000008, "LowFrequency" },
				{ 0x00000010, "BackLeft" },
				{ 0x00000020, "BackRight" },
				{ 0x00000040, "FrontLeftCenter" },
				{ 0x00000080, "FrontRightCenter" },
				{ 0x00000100, "BackCenter" },
				{ 0x00000200, "SideLeft" },
				{ 0x00000400, "SideRight" },
				{ 0x00000800, "TopCenter" },
				{ 0x00001000, "TopFrontLeft" },
				{ 0x00002000, "TopFrontCenter" },
				{ 0x00004000, "TopFrontRight" },
				{ 0x00008000, "TopBackLeft" },
				{ 0x00010000, "TopBackCenter" },
				{ 0x00020000, "TopBackRight" },
				{ 0x20000000, "StereoLeft" },
				{ 0x40000000, "StereoRight" },
				{ 0x0000000080000000, "WideLeft" },
				{ 0x0000000100000000, "WidrRight" },
				{ 0x0000000200000000, "SurroundDirectLeft" },
				{ 0x0000000400000000, "SurroundDirectRight" },
				{ 0x0000000800000000, "LowFrequencyTwo" },
				{ 0x8000000000000000, "NativeLyout" },
				{ -1, "Auto" },
				{ FrontCenter, "Mono" },
				{ FrontLeft | FrontRight, "Stereo" },
				{ FrontLeft | FrontRight | LowFrequency, "Surround21" },
				{ FrontLeft | FrontRight | BackCenter, "StereoCBack" },
				{ FrontLeft | FrontRight | FrontCenter, "Surround" },
				{ FrontLeft | FrontRight | SideLeft | SideRight, "Surround22" },
				{ FrontLeft | FrontRight | BackLeft | BackRight, "SurroundQuad" },
				{ FrontLeft | FrontRight | SideLeft | SideRight | FrontLeftCenter| FrontRightCenter, "Surround6Front" },
				{ FrontLeft | FrontRight | SideLeft | SideRight | FrontLeftCenter| FrontRightCenter | LowFrequency, "Surround61Front" },
				{ FrontLeft | FrontRight | FrontCenter | BackCenter, "Surround40" },
				{ FrontLeft | FrontRight | FrontCenter | LowFrequency, "Surround31" },
				{ FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight, "Surround5Back" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight, "Surround5" },
				{ FrontLeft | FrontRight | FrontCenter | BackCenter | LowFrequency, "Surround41" },
				{ FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | BackCenter, "SurroundHexagonal" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | BackCenter, "Surround6" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency, "Surround51" },
				{ FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | LowFrequency, "Surround51Back" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | BackLeft | BackRight, "Surround7" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency | BackCenter, "Surround61" },
				{ FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | LowFrequency | BackCenter, "Surround61Back" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | FrontLeftCenter | FrontRightCenter, "Surround7Front" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | BackLeft | BackCenter | BackRight, "SurroundOctagonal" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency | BackLeft | BackRight, "Surround71" },
				{ FrontLeft | FrontRight | FrontCenter | SideLeft | SideRight | LowFrequency | FrontLeftCenter | FrontRightCenter, "Surround71Wide" },
				{ FrontLeft | FrontRight | FrontCenter | BackLeft | BackRight | LowFrequency | FrontLeftCenter | FrontRightCenter, "Surround71WideBack" },
				{ StereoLeft | StereoRight, "StereoDownmix" }
			};

			long Speakers::fromString(QString val)
			{
				for (auto v : pMap.toStdMap())
				{
					if (v.second == val)
						return static_cast<long>(v.first);
				}

				return Unknow;
			}

			QString	Speakers::toString(long val)
			{
				return pMap[val];
			}

			QMap<long, QString> &Speakers::getValues()
			{
				return pMap;
			}
		}
	}
}
