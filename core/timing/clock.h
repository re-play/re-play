#ifndef CLOCK_H
#define CLOCK_H

#include <atomic>

#include <QObject>

namespace RePlay
{
	namespace Core
	{
		namespace Timing
		{
			class Clock : public QObject
			{

				Q_OBJECT

				Q_PROPERTY(double speed READ getSpeed WRITE setSpeed NOTIFY speedChanged)
				Q_PROPERTY(double time READ getSeconds WRITE seek NOTIFY clockTick)
				Q_PROPERTY(double paused READ isPaused NOTIFY stateChange)

			public:

				explicit Clock();

				// - Management
				// ----------------------------------------
				Q_INVOKABLE virtual void pause();
				Q_INVOKABLE virtual void start();
				Q_INVOKABLE virtual void seek(double time);

				// - Time data
				// ----------------------------------------
				virtual double getSeconds() const;
				double getMilliseconds() const;
				double getMinutes() const;
				double getHours() const;
				double getDays() const;

				// - Settings
				// ----------------------------------------
				bool	isPaused() const;
				double	getSpeed() const;
				void	setSpeed(double speed);

			protected:

				void increment(double time);
				void setTime(double time);

			signals:

				void clockTick(double time);
				void speedChanged();
				void stateChange();

			public slots:



			private:

				// - Clock properties
				// ------------------------
				std::atomic<double>	pTime;
				std::atomic<double>	pSpeed;
				std::atomic<bool>	pPaused;


			};
		}
	}
}

#endif // CLOCK_H
