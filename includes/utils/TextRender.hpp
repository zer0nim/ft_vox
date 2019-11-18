#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>
#include <map>
#include "commonInclude.hpp"
#include "Shader.hpp"

class TextRender {
	public:
		explicit TextRender(Shader &sh);
		TextRender(TextRender const &src);
		virtual ~TextRender();

		TextRender &operator=(TextRender const &rhs);
		void write(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);


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

	private:
		TextRender();
		struct Character {
			GLuint		textureID;
			glm::ivec2	size;
			glm::ivec2	bearing;
			int64_t		advance;
		};
		std::map<GLchar, Character> _characters;

		Shader		&_shader;
		glm::mat4	_projection;
		GLuint		_vao;
		GLuint		_vbo;
};
