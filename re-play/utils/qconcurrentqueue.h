#ifndef QCONCURRENTQUEUE_H
#define QCONCURRENTQUEUE_H

#include <QQueue>
#include <QReadWriteLock>
#include <QWaitCondition>

template <typename T>
class QConcurrentQueue : public QQueue<T>
{

public:

	QConcurrentQueue()
		: QQueue<T>()
	{

	}

	T dequeue()
	{
		pMutex.lockForWrite();

		while(QQueue<T>::empty())
		{
			pWait.wait(&pMutex);
		}

		T val = QQueue<T>::dequeue();

		pMutex.unlock();

		return val;
	}

	void enqueue(const T &val)
	{
		pMutex.lockForRead();

		QQueue<T>::enqueue(val);

		pWait.notify_one();

		pMutex.unlock();
	}

	int size()
	{
		pMutex.lockForWrite();

		auto s = QQueue<T>::size();

		pMutex.unlock();

		return s;
	}

private:

	// - Concurrency
	// ----------------------
	QWaitCondition pWait;
	QReadWriteLock pMutex;

};

#endif // QCONCURRENTQUEUE_H
