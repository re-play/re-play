#ifndef YUVSHADER_H
#define YUVSHADER_H

#include <QSGSimpleMaterialShader>
#include <QSGGeometryNode>

#include <rendering/textures/frametextures.h>

namespace RePlay
{
	namespace Rendering
	{
		namespace Shaders
		{

			class YuvShader : public QSGSimpleMaterialShader<Textures::FrameTextures>
			{
				QSG_DECLARE_SIMPLE_SHADER(YuvShader, Textures::FrameTextures)

			public:

				YuvShader();
				~YuvShader();

				// - Vertex shader
				// - This is a virtual function that Qt internals will call when needed.
				const char *vertexShader() const {
					return
							"attribute highp vec4 aVertex;                              \n"
							"attribute highp vec2 aTexCoord;                            \n"
							"uniform highp mat4 qt_Matrix;                              \n"
							"varying highp vec2 texCoord;                               \n"
							"void main() {                                              \n"
							"    gl_Position = qt_Matrix * aVertex;                     \n"
							"    texCoord = aTexCoord;                                  \n"
							"}";
				}

				//  - Fragment shader
				//  - This is a virtual function that Qt internals will call when needed.
				const char *fragmentShader() const {
					return
							"uniform lowp sampler2D myTexture;                          \n"
							"uniform lowp float qt_Opacity;                             \n"
							"varying highp vec2 texCoord;                               \n"
							"void main() {                                              \n"
							  "vec3 tex = texture2D(myTexture,texCoord).rgb;            \n"
							  "gl_FragColor=vec4(tex,1.0) * qt_Opacity;                 \n"
							"}";
				}

				// - This is what you use to configure your vertex shader attributes.
				// - This is a virtual function that Qt internals will call when needed.
				QList<QByteArray> attributes() const
				{
					return QList<QByteArray>() << "aVertex" << "aTexCoord";
				}

				// This updates the state of classes used by the Material, in this case the "Textures" struct.
				// This is a virtual function that Qt internals will call when needed.
				void updateState(const Textures::FrameTextures *state, const Textures::FrameTextures *);

				// Any OpenGL uniforms need to be set here.
				// This is a virtual function that Qt internals will call when needed.
				void resolveUniforms();
			};

		}
	}
}

#endif // YUVSHADER_H
