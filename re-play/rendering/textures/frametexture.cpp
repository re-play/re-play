#include "rendering/textures/frametexture.h"

namespace RePlay
{
	namespace Rendering
	{
		namespace Textures
		{

			FrameTexture::FrameTexture()
			{
				pTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
			}

			FrameTexture::~FrameTexture()
			{
				delete pTexture;
			}

			void FrameTexture::bind()
			{
				pTexture->bind();
			}

			bool FrameTexture::hasAlphaChannel() const
			{
				return false;
			}

			bool FrameTexture::hasMipmaps() const
			{
				return pTexture->mipLevels();
			}

			int FrameTexture::textureId() const
			{
				return pTexture ? pTexture->textureId() : 0;
			}

			QSize FrameTexture::textureSize() const
			{
				return pTexture ? QSize(pTexture->width(), pTexture->height()) : QSize();
			}

			QOpenGLTexture * FrameTexture::texture()
			{
				return pTexture;
			}

			void FrameTexture::setTextureInfo(TextureFormat format, QImage image)
			{
				pFormat = format;
				pSize = QSize(image.size());
				pImage = image;
				updateTexture();
			}

			bool FrameTexture::updateTexture()
			{
				if (pTexture->isCreated())
				{
					pTexture->destroy();
				}

				// Configure texture object.
				pTexture->create();
				pTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
				pTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
				// pTexture->setSize(m_image.width(), m_image.height());
				pTexture->setMipLevels(0);
				// pTexture->setFormat(m_format);
				// pTexture->allocateStorage();


				// setData actually allocates the memory for the image and fills the texture with the image data.
				// There are also ways to pass raw data (or YUV and many other formats which can be useful).
				if (pTexture->isCreated())
				{
					pTexture->setData(pImage);
				}
				else
				{
					qDebug() << "Texture not created, cannot set data to it.";
					return false;
				}

				return true;
			}
		}
	}
}
