#ifndef FRAMEGEOMETRY_H
#define FRAMEGEOMETRY_H

#include <rendering/shaders/yuvshader.h>
#include <rendering/textures/frametextures.h>

namespace RePlay
{
	namespace Rendering
	{
		namespace Geometry
		{

			class FrameGeometry : public QSGGeometryNode
			{

			public:

				FrameGeometry();
				~FrameGeometry();

			private:

				QSGGeometry pGeometry;

			};

		}
	}
}

#endif // FRAMEGEOMETRY_H
