#pragma once

#include "commonInclude.hpp"
#include "Shader.hpp"

#define SHADER_IMAGE_VS "./shaders/image_vs.glsl"
#define SHADER_IMAGE_FS "./shaders/image_fs.glsl"
#define SHADER_IMAGE_ROW_SIZE 5

class ImageRender {
	public:
		explicit ImageRender(uint32_t width, uint32_t height);
		ImageRender(ImageRender const &src);
		virtual ~ImageRender();

		ImageRender &operator=(ImageRender const &rhs);
		void		draw(glm::vec2 pos, glm::vec2 size, int texID, \
						glm::vec4 color = glm::vec4(1, 1, 1, 1));
		void		draw(float posx, float posy, float w, float h, int texID, \
						glm::vec4 color = glm::vec4(1, 1, 1, 1));

		Shader			&getShader();
		Shader const	&getShader() const;

		class ImageRenderError : public std::exception {
			public:
				virtual const char* what() const throw() = 0;
		};
		class LoadImageException : public ImageRenderError {
			public:
				virtual const char* what() const throw() {
					return ("Fail to load image");
				}
		};

	private:
		ImageRender();

		Shader		_shader;
		glm::mat4	_projection;
		GLuint		_vao;
		GLuint		_vbo;
};
