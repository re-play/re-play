#include "core/timing/clock.h"

namespace RePlay
{
	namespace Core
	{
		namespace Timing
		{

			Clock::Clock()
				: QObject(),
				  pTime(0),
				  pSpeed(1),
				  pPaused(false)
			{

			}

			void Clock::pause()
			{
				pPaused = true;
				emit stateChange();
			}

			void Clock::start()
			{
				pPaused = false;
				emit stateChange();
			}

			void Clock::seek(double time)
			{
				pTime = time;
			}

			double Clock::getMilliseconds() const
			{
				return pTime * 1000;
			}

			double Clock::getSeconds() const
			{
				return pTime;
			}

			double Clock::getMinutes() const
			{
				return pTime / 60;
			}

			double Clock::getHours() const
			{
				return getMinutes() / 60;
			}

			double Clock::getDays() const
			{
				return getHours() / 24;
			}

			bool Clock::isPaused() const
			{
				return pPaused;
			}

			double Clock::getSpeed() const
			{
				return pSpeed;
			}

			void Clock::setSpeed(double speed)
			{
				pSpeed = speed;
				emit speedChanged();
			}

			void Clock::increment(double time)
			{
				// TODO: Fix race condition
				pTime = pTime + time;
				emit clockTick(pTime);
			}

			void Clock::setTime(double time)
			{
				pTime = time;
				emit clockTick(pTime);
			}
		}
	}
}
