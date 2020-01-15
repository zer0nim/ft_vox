#include "ImageRender.hpp"
#include "debug.hpp"
#include "Logging.hpp"

ImageRender::ImageRender(uint32_t width, uint32_t height) :
_shader(SHADER_IMAGE_VS, SHADER_IMAGE_FS),
_projection(glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height))) {
	// create VAO & VBO
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * SHADER_IMAGE_ROW_SIZE, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_ROW_SIZE * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, SHADER_IMAGE_ROW_SIZE * sizeof(float),
							reinterpret_cast<void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, SHADER_IMAGE_ROW_SIZE * sizeof(float),
							reinterpret_cast<void*>(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_shader.use();
	_shader.setMat4("projection", _projection);
}


ImageRender::ImageRender(ImageRender const &src) :
_shader(src.getShader()) {
	*this = src;
}

ImageRender::~ImageRender() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
}

ImageRender &ImageRender::operator=(ImageRender const &rhs) {
	(void)rhs;
	if (this != &rhs) {
		_vao = rhs.getVao();
		_vbo = rhs.getVbo();
	}
	return *this;
}

void ImageRender::draw(glm::vec2 pos, glm::vec2 size, int texID, glm::vec4 color) {
	draw(pos.x, pos.y, size.x, size.y, texID, color);
}
void ImageRender::draw(float posx, float posy, float w, float h, int texID, glm::vec4 color) {
    _shader.use();
    _shader.setVec4("color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);
	// set VBO values
	GLfloat vertices[6][SHADER_IMAGE_ROW_SIZE] = {
		{posx,     posy + h,   0.0, 0.0, static_cast<float>(texID)},
		{posx,     posy,       0.0, 1.0, static_cast<float>(texID)},
		{posx + w, posy,       1.0, 1.0, static_cast<float>(texID)},
		{posx,     posy + h,   0.0, 0.0, static_cast<float>(texID)},
		{posx + w, posy,       1.0, 1.0, static_cast<float>(texID)},
		{posx + w, posy + h,   1.0, 0.0, static_cast<float>(texID)},
	};
	// set VBO on shader
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// draw image
	glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

Shader			&ImageRender::getShader() { return _shader; }
Shader const	&ImageRender::getShader() const { return _shader; }
uint32_t		ImageRender::getVao() const { return _vao; }
uint32_t		ImageRender::getVbo() const { return _vbo; }
