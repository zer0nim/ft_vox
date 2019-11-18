#include "TextRender.hpp"

TextRender::TextRender(Shader &sh) :
_shader(sh),
_projection(glm::ortho(0, SCREEN_W, 0, SCREEN_H)) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		throw TextRender::LoadTextRenderException();
	}
	FT_Face face;
	if (FT_New_Face(ft, "fonts/minecraft_normal.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Done_FreeType(ft);
		throw TextRender::LoadTextRenderException();
	}
	FT_Set_Pixel_Sizes(face, 0, 48);  // size

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) {
		// load char
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << c << std::endl;
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
			GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// set textures options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// save char args for future write
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		_characters.insert(std::pair<GLchar, Character>(c, character));
	}
    glBindTexture(GL_TEXTURE_2D, 0);

	// delete freetype objects
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// create VAO & VBO
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_shader.use();
	_shader.setMat4("projection", _projection);
}

TextRender::TextRender(TextRender const &src) :
_shader(src.getShader()) {
	*this = src;
}

TextRender::~TextRender() {
}

TextRender &TextRender::operator=(TextRender const &rhs) {
	(void)rhs;
	if (this != &rhs) {
		_vao = rhs.getVao();
		_vbo = rhs.getVbo();
	}
	return *this;
}

void TextRender::write(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    _shader.use();
    _shader.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {  // foreach chars
        Character ch = _characters[*c];
        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        // set VBO values
        GLfloat vertices[6][4] = {
            {xpos,     ypos + h,   0.0, 0.0},
            {xpos,     ypos,       0.0, 1.0},
            {xpos + w, ypos,       1.0, 1.0},
            {xpos,     ypos + h,   0.0, 0.0},
            {xpos + w, ypos,       1.0, 1.0},
            {xpos + w, ypos + h,   1.0, 0.0},
		   };
        // bind char texture
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // set VBO on shader
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // draw char
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // move cursor to the next character
        x += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Shader		&TextRender::getShader() { return _shader; }
Shader		&TextRender::getShader() const { return _shader; }
uint32_t	TextRender::getVao() const { return _vao; }
uint32_t	TextRender::getVbo() const { return _vbo; }
