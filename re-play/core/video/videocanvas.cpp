#include "core/video/videocanvas.h"

#include <rendering/geometry/framegeometry.h>
#include <rendering/textures/frametextures.h>
#include <rendering/textures/frametexture.h>
#include <rendering/shaders/yuvshader.h>

#include <QSGTexture>
#include <QOpenGLTexture>
#include <QSGSimpleRectNode>

using namespace RePlay::Rendering::Geometry;
using namespace RePlay::Rendering::Textures;

namespace RePlay
{
	namespace Core
	{
		namespace Video
		{

			VideoCanvas::VideoCanvas(QQuickItem *parent)
				: QQuickItem(parent)
			{
				setFlag(QQuickItem::ItemHasContents);

				connect(this, &QQuickItem::widthChanged, this, &VideoCanvas::sizeChanged);
				connect(this, &QQuickItem::heightChanged, this, &VideoCanvas::sizeChanged);
			}

			VideoCanvas::~VideoCanvas()
			{

            }

			void VideoCanvas::sizeChanged()
			{
				this->update();
            }

			void VideoCanvas::setFrame(QImage frame)
            {
                m.lock();
                pFrames.enqueue(frame);
				update();
                m.unlock();
			}

			QSGNode *VideoCanvas::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
			{
				// - This will be the background node, you'll see it as a border color.
				QSGSimpleRectNode *colorNode = static_cast<QSGSimpleRectNode*>(node);
				if (!colorNode) colorNode = new QSGSimpleRectNode(boundingRect(), QColor(Qt::black));
				else colorNode->setRect(boundingRect());

				// - Now we make the texture node a child of the colorNode so it appears in front of the color node.
				FrameGeometry * textureNode;
				if (colorNode->childCount() == 0)
				{
					// - If there are no children of colorNode that means our texture node hasn't been created yet
					textureNode = new FrameGeometry();
					colorNode->appendChildNode(textureNode);
				}
				else
				{
					// If colorNode has a child, that means we have already added the TextureNode to colorNode.
					textureNode = static_cast<FrameGeometry*>(colorNode->firstChild());
				}

                // - Check if we have a new frame
                if (pFrames.size() <= 0)
                {
                    delete textureNode;
                    return colorNode;
                }

                // - Set current frame
                m.lock();
                while (pFrames.size() > 1)
                    pFrames.dequeue();
                pCurrentFrame = pFrames.dequeue();
                m.unlock();


                if (pCurrentFrame.isNull())
				{
					qDebug() << "Unable to load frame";
					delete textureNode;
					return colorNode;
				}

				// Center geometry, with a slight border.
				QRect b = boundingRect().toRect();
				b.adjust(0, 0, 0, 0);
				QSGGeometry::updateTexturedRectGeometry(textureNode->geometry(), b, QRectF(0, 0, 1, 1));
				//qDebug() << b.width();
				textureNode->geometry()->markVertexDataDirty();
				textureNode->geometry()->markIndexDataDirty();

				// Set the texture data and update the texture.
				FrameTexture *myTexture = static_cast<QSGSimpleMaterial<FrameTextures>*>(textureNode->material())->state()->texture;
                myTexture->setTextureInfo(QOpenGLTexture::RGBA8_UNorm, pCurrentFrame);

				colorNode->markDirty(QSGNode::DirtyForceUpdate);
				return colorNode;
			}
		}
	}
}
