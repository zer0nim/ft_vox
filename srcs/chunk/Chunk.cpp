#include "Chunk.hpp"
#include "utils/Material.hpp"

const float	Chunk::_cubeData[] = {
	// positions			// normals				// texture coords
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		0.0f,  // 0r
	0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		0.0f,  // 0r
	0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		0.0f,  // 0r

	0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		0.0f,  // 0l
	-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		0.0f,  // 0l
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		0.0f,  // 0l

	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		2.0f,  // 2l
	0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		2.0f,  // 2l
	0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		2.0f,  // 2l

	0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		2.0f,  // 2r
	-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,		2.0f,  // 2r
	-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		2.0f,  // 2r

	-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3r
	-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		3.0f,  // 3r
	-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3r

	-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3l
	-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		3.0f,  // 3l
	-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3l

	0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		1.0f,  // 1l
	0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1l
	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		1.0f,  // 1l

	0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		1.0f,  // 1r
	0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1r
	0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		1.0f,  // 1r

	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f,  // 5r
	0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,		5.0f,  // 5r
	0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5r

	0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5l
	-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,		5.0f,  // 5l
	-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f,  // 5l

	-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4r
	0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		4.0f,  // 4r
	0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4r

	0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4l
	-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		4.0f,  // 4l
	-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f  // 4l
};

Chunk::Chunk(TextureManager const &textureManager) : AChunk(textureManager) {
	_naiveShader = new Shader("shaders/naive_vs.glsl", "shaders/naive_fs.glsl");
	sendCubeData();
}

Chunk::Chunk(Chunk const &src) : AChunk(src) {
	*this = src;
}

Chunk::~Chunk() {
	delete _naiveShader;
}

Chunk &Chunk::operator=(Chunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void	Chunk::_createChunk() {
	srand(1);

	for (uint8_t x = 0; x < CHUNK_SZ_X; x++) {
		for (uint8_t y = 0; y < CHUNK_SZ_Y; y++) {
			for (uint8_t z = 0; z < CHUNK_SZ_Z; z++) {
				_data.data[x][y][z] = 0;
				if (rand() % 300 == 0)
					_data.data[x][y][z] = rand() % 3;
			}
		}
	}
}

// TODO(zer0nim): remove
void	Chunk::oldCreateChunk() {
	_createChunk();
}


void	Chunk::update() {
	if (_data.isModified == false)
		return;  // chunk not modified -> don't update it
	_data.isModified = false;

	// std::cout << "update: " << this << std::endl << std::endl;

	// for (uint8_t x = 0; x < CHUNK_SZ_X; ++x) {
	// 	for (uint8_t y = 0; y < CHUNK_SZ_Y; ++y) {
	// 		for (uint8_t z = 0; z < CHUNK_SZ_Z; ++z) {
	// 			// std::cout << "_" << _data.data[x][y][z];
	// 			std::cout << "_" << static_cast<uint16_t>(
	// 				_data.data[x][y][z]);
	// 		}
	// 		std::cout << std::endl;
	// 	}
	// 	std::cout << "____________" << std::endl;
	// }
}

void	Chunk::draw(glm::mat4 &view, wordIVec3 &startPos) {
	_naiveShader->use();
	_naiveShader->setMat4("view", view);
	glBindVertexArray(_cubeVao);

	// std::cout << "_textureManager: " << _textureManager << std::endl;

	// loop throught chunk cubes and draw them
	glm::mat4 baseModel = glm::translate(glm::mat4(1.0), glm::vec3(startPos));
	for (uint8_t x = 0; x < CHUNK_SZ_X; ++x) {
		for (uint8_t y = 0; y < CHUNK_SZ_Y; ++y) {
			for (uint8_t z = 0; z < CHUNK_SZ_Z; ++z) {
				if (_data.data[x][y][z] != 0) {
					_naiveShader->setInt("cubeId", _data.data[x][y][z] - 1);

					glm::mat4 model = glm::translate(baseModel, glm::vec3(x, y, z));
					_naiveShader->setMat4("model", model);

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
	}
}

void	Chunk::sendCubeData() {
    glGenVertexArrays(1, &_cubeVao);
    glGenBuffers(1, &_cubeVbo);

    glBindBuffer(GL_ARRAY_BUFFER, _cubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Chunk::_cubeData), Chunk::_cubeData, GL_STATIC_DRAW);

    glBindVertexArray(_cubeVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

	_naiveShader->use();
	// set cube material
	Material material;
	_naiveShader->use();
	_naiveShader->setBool("material.specular.isTexture", false);
	_naiveShader->setVec3("material.specular.color", material.specular);
	_naiveShader->setFloat("material.shininess", material.shininess);

	// set direction light
	_naiveShader->setVec3("dirLight.direction", -0.2f, -0.8f, -0.6f);
	_naiveShader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_naiveShader->setVec3("dirLight.diffuse", 1.5f, 1.5f, 1.5f);
	_naiveShader->setVec3("dirLight.specular", 1, 1, 1);

	_textureManager.setUniform(*_naiveShader);
}

void	Chunk::setProjection(glm::mat4 &projection) {
	_naiveShader->use();
	_naiveShader->setMat4("projection", projection);
}
