#include "rendering/shaders/yuvshader.h"

#include <QOpenGLFunctions>

using namespace RePlay::Rendering::Textures;

namespace RePlay
{
	namespace Rendering
	{
		namespace Shaders
		{
			YuvShader::YuvShader()
			{

			}

			YuvShader::~YuvShader()
			{

			}

			void YuvShader::updateState(const FrameTextures *state, const FrameTextures *)
			{
				if (!program()->bind())
					qDebug() << "Error binding shader program in" << Q_FUNC_INFO;

				QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

				f->glActiveTexture(GL_TEXTURE0);
				state->texture->bind();
			}

			void YuvShader::resolveUniforms()
			{
				if (!program()->bind())
					qDebug() << "Error binding shader program in" << Q_FUNC_INFO;

				program()->setUniformValue("myTexture", 0); // GL_TEXTURE0
			}
		}
	}
}
