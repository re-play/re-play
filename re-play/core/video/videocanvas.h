#ifndef VIDEOCANVAS_H
#define VIDEOCANVAS_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QTextStream>
#include <QQuickItem>
#include <QImage>
#include <QFile>
#include <QMutex>

#include <utils/qconcurrentqueue.h>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

namespace RePlay
{
	namespace Core
	{
		namespace Video
		{

			class VideoCanvas : public QQuickItem, protected QOpenGLFunctions
			{
                Q_OBJECT

			public:

				explicit VideoCanvas(QQuickItem *parent = nullptr);
                ~VideoCanvas();

			protected:

				QSGNode * updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

            signals:

			public slots:

				void setFrame(QImage frame);

			private slots:

				void sizeChanged();

			private:

                QConcurrentQueue<QImage> pFrames;
                QImage pCurrentFrame;
                QMutex m;
			};

		}
	}
}

#endif // VIDEOCANVAS_H
