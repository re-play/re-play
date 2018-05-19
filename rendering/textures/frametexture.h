#ifndef FRAMETEXTURE_H
#define FRAMETEXTURE_H

#include <QSGSimpleMaterialShader>
#include <QSGDynamicTexture>
#include <QSGGeometryNode>
#include <QOpenGLTexture>
#include <QColor>

namespace RePlay
{
	namespace Rendering
	{
		namespace Textures
		{
			using TextureFormat = QOpenGLTexture::TextureFormat;

			class FrameTexture : public QSGDynamicTexture
			{

			public:

				FrameTexture();
				~FrameTexture();

				// - Managing
				// -------------------------------------------
				void bind() Q_DECL_OVERRIDE;

				// - Informations
				// -------------------------------------------
				bool	hasAlphaChannel() const Q_DECL_OVERRIDE;
				bool	hasMipmaps() const Q_DECL_OVERRIDE;
				int		textureId() const Q_DECL_OVERRIDE;
				QSize	textureSize() const Q_DECL_OVERRIDE;

				// - Handler
				// -------------------------------------------
				QOpenGLTexture * texture();

				// - Change texture
				// -------------------------------------------
				void setTextureInfo(TextureFormat format, QImage image);

				// - Update on GPU
				// -------------------------------------------
				bool updateTexture() Q_DECL_OVERRIDE;

			private:

				QOpenGLTexture	* pTexture;
				TextureFormat	  pFormat;
				QImage			  pImage;
				QSize			  pSize;
			};
		}
	}
}

#endif // FRAMETEXTURE_H
