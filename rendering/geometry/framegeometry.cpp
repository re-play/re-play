#include "rendering/geometry/framegeometry.h"

using namespace RePlay::Rendering::Textures;
using namespace RePlay::Rendering::Shaders;

namespace RePlay
{
	namespace Rendering
	{
		namespace Geometry
		{

			FrameGeometry::FrameGeometry()
				: pGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
			{
				setGeometry(&pGeometry);
				pGeometry.setIndexDataPattern(QSGGeometry::AlwaysUploadPattern);

				QSGSimpleMaterial<FrameTextures> *material = YuvShader::createMaterial();
				material->state()->texture = new FrameTexture();

				setMaterial(material);
				setFlag(OwnsMaterial);
			}

			FrameGeometry::~FrameGeometry()
			{
				static_cast<QSGSimpleMaterial<FrameTextures>*>(this->material())->state()->texture->deleteLater();
			}

		}
	}
}
