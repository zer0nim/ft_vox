#include "Chunk.hpp"
#include "utils/Material.hpp"

const float	Chunk::_cubeData[] = {
	// positions			// normals				// texture coords
	0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f,  // 0l
	-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0.0f,  // 0l
	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f,  // 0l

	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f,  // 0r
	0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,		0.0f,  // 0r
	0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f,  // 0r


	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1l
	0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1l
	0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		1.0f,  // 1l

	0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1r
	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1r
	0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		1.0f,  // 1r


	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		2.0f,  // 2l
	0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		2.0f,  // 2l
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		2.0f,  // 2l

	0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		2.0f,  // 2r
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		2.0f,  // 2r
	-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		2.0f,  // 2r


	-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3l
	-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		3.0f,  // 3l
	-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3l

	-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3r
	-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		3.0f,  // 3r
	-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3r


	-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4l
	0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4l
	0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		4.0f,  // 4l

	0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4r
	-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4r
	-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		4.0f,  // 4r


	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f,  // 5l
	0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,		5.0f,  // 5l
	0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5l

	0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5r
	-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,		5.0f,  // 5r
	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f   // 5r
};

Chunk::Chunk(TextureManager const &textureManager): AChunk(textureManager) {}

Chunk::Chunk(Chunk const &src) : AChunk(src) {
	*this = src;
}

Chunk::~Chunk() {
}

Chunk &Chunk::operator=(Chunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void	Chunk::initShader(glm::mat4 &projection, TextureManager const &textureManager) {
	if (!_shaderData) {
		_shaderData = std::unique_ptr<ShaderData>(new AChunk::ShaderData(
			"./shaders/naive_vs.glsl", "./shaders/naive_fs.glsl"));
		_shaderData->shader->use();
		_shaderData->shader->setMat4("projection", projection);
		sendCubeData(textureManager);
		_shaderData->vaoUsed = true;
	}
}

void	Chunk::update() {
	if (_data.isModified == false)
		return;  // chunk not modified -> don't update it
	_data.isModified = false;
}

void	Chunk::_draw(glm::mat4 &view, glm::vec3 &pos) {
	_textureManager.activateTextures();

	_shaderData->shader->use();
	_shaderData->shader->setVec3("viewPos", pos);
	_shaderData->shader->setMat4("view", view);
	glBindVertexArray(_shaderData->vao);

	// loop throught chunk cubes and draw them
	glm::mat4 baseModel = glm::translate(glm::mat4(1.0), glm::vec3(_chunkPos));
	for (uint8_t x = 0; x < CHUNK_SZ_X; ++x) {
		for (uint8_t y = 0; y < CHUNK_SZ_Y; ++y) {
			for (uint8_t z = 0; z < CHUNK_SZ_Z; ++z) {
				if (_data.data[x][y][z] > 0) {
					_shaderData->shader->setInt("cubeId", _data.data[x][y][z] - 1);

					glm::mat4 model = glm::translate(baseModel, glm::vec3(x, y, z));
					_shaderData->shader->setMat4("model", model);

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
	}
}

void	Chunk::sendCubeData(TextureManager const &textureManager) {
    glGenVertexArrays(1, &_shaderData->vao);
    glGenBuffers(1, &(_shaderData->vbo));

    glBindBuffer(GL_ARRAY_BUFFER, _shaderData->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Chunk::_cubeData), Chunk::_cubeData, GL_STATIC_DRAW);

    glBindVertexArray(_shaderData->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

	_shaderData->shader->use();
	// set cube material
	Material material;
	_shaderData->shader->setVec3("material.specular", material.specular);
	_shaderData->shader->setFloat("material.shininess", material.shininess);

	// set direction light
	_shaderData->shader->setVec3("dirLight.direction", -0.2f, -0.8f, -0.6f);
	_shaderData->shader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_shaderData->shader->setVec3("dirLight.diffuse", 1.5f, 1.5f, 1.5f);
	_shaderData->shader->setVec3("dirLight.specular", 1, 1, 1);

	textureManager.setUniform(*_shaderData->shader);
}
