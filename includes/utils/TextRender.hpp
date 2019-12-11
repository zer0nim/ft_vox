#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>
#include <map>
#include "commonInclude.hpp"
#include "Shader.hpp"

class TextRender {
	public:
		explicit TextRender(Shader &sh, uint32_t width, uint32_t height);
		TextRender(TextRender const &src);
		virtual ~TextRender();

		TextRender &operator=(TextRender const &rhs);
		void loadFont(std::string name, std::string const &filename, uint32_t size);
		void write(std::string const &fontName, std::string text, GLfloat x = 0, GLfloat y = 0, GLfloat scale = 1,
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));


		Shader		&getShader();
		Shader		&getShader() const;
		uint32_t	getVao() const;
		uint32_t	getVbo() const;

		class TextRenderError : public std::exception {
			public:
				virtual const char* what() const throw() = 0;
		};
		class LoadTextRenderException : public TextRenderError {
			public:
				virtual const char* what() const throw() {
					return ("Fail to load text");
				}
		};
		struct Character {
			GLuint		textureID;
			glm::ivec2	size;
			glm::ivec2	bearing;
			int64_t		advance;
		};
		std::map<std::string, std::map<GLchar, Character> > font;

	private:
		TextRender();

		Shader		&_shader;
		glm::mat4	_projection;
		GLuint		_vao;
		GLuint		_vbo;
};
