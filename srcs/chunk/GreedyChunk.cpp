#include "GreedyChunk.hpp"
#include "utils/Material.hpp"

const float	GreedyChunk::_cubeData[] = {
	// positions			// normals				// texture coords
	1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f,  // 0l
	0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0.0f,  // 0l
	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f,  // 0l

	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f,  // 0r
	1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,		0.0f,  // 0r
	1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f,  // 0r


	1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1l
	1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1l
	1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		1.0f,  // 1l

	1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1r
	1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1r
	1.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		1.0f,  // 1r


	0.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		2.0f,  // 2l
	1.0f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		2.0f,  // 2l
	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		2.0f,  // 2l

	1.0f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		2.0f,  // 2r
	0.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		2.0f,  // 2r
	0.0f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		2.0f,  // 2r


	0.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3l
	0.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		3.0f,  // 3l
	0.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3l

	0.0f, 0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3r
	0.0f, 0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		3.0f,  // 3r
	0.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3r


	0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4l
	1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4l
	1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		4.0f,  // 4l

	1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4r
	0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4r
	0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		4.0f,  // 4r


	0.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f,  // 5l
	1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,		5.0f,  // 5l
	1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5l

	1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5r
	0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,		5.0f,  // 5r
	0.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f   // 5r
};

std::unique_ptr<GreedyChunk::ShaderData>	GreedyChunk::_shaderData = std::unique_ptr<GreedyChunk::ShaderData>();
std::array< std::array< std::array<bool, CHUNK_SZ_Z>, CHUNK_SZ_Y >, CHUNK_SZ_X >	\
	GreedyChunk::_processedVox = {{{{{0}}}}};

GreedyChunk::GreedyChunk(TextureManager const &textureManager, glm::mat4 &projection)
: AChunk(textureManager, projection) {
	if (!_shaderData) {
		_shaderData = std::unique_ptr<ShaderData>(new ShaderData());
		_shaderData->naiveShader->use();
		_shaderData->naiveShader->setMat4("projection", projection);
		sendCubeData();
	}
}

GreedyChunk::GreedyChunk(GreedyChunk const &src) : AChunk(src) {
	*this = src;
}

GreedyChunk::~GreedyChunk() {
}

GreedyChunk &GreedyChunk::operator=(GreedyChunk const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/*
	search the quad mesh starting from this pos
*/
void	GreedyChunk::searchQuadMesh(chunkVec3 const &startI) {
	uint8_t const	blockId = _data.data[startI.x][startI.y][startI.z];
	uint8_t	w = 1, l = 1, h = 1;

	// width
	for (uint8_t x = startI.x + 1; x < CHUNK_SZ_X && \
	_data.data[x][startI.y][startI.z] == blockId; ++x) {
		++w;
	}

	// length
	bool validLength = true;
	for (uint8_t z = startI.z + 1; z < CHUNK_SZ_Z && validLength; ++z) {
		validLength = false;
		uint8_t x = startI.x;
		while (x < startI.x + w && _data.data[x][startI.y][z] == blockId) {
			++x;
		}
		if (x == startI.x + w) {
			validLength = true;
			++l;
		}
	}

	// height
	bool validHeight = true;
	for (uint8_t y = startI.y + 1; y < CHUNK_SZ_Y && validHeight; ++y) {
		validHeight = false;

		bool validLength = true;
		uint8_t z = startI.z;
		while (z < startI.z + l && validLength) {
			validLength = false;

			uint8_t x = startI.x;
			while (x < startI.x + w && _data.data[x][y][z] == blockId) {
				++x;
			}
			if (x == startI.x + w) {
				validLength = true;
			}

			++z;
		}
		if (validLength) {
			validHeight = true;
			++h;
		}
	}

	// update _processedVox
	for (uint8_t x = startI.x; x < startI.x + w; ++x) {
		for (uint8_t z = startI.z; z < startI.z + l; ++z) {
			for (uint8_t y = startI.y; y < startI.y + h; ++y) {
				_processedVox[x][y][z] = true;
			}
		}
	}

	// save mesh data
	_meshDatas.push_back({startI, chunkVec3(w, h, l), blockId});
}

void	GreedyChunk::update() {
	if (_data.isModified == false)
		return;  // GreedyChunk not modified -> don't update it
	_data.isModified = false;

	// reset _processedVox
	GreedyChunk::_processedVox = {{{{{0}}}}};

	// loop through unprocessed index
	for (uint8_t y = 0; y < CHUNK_SZ_Y; ++y) {
		for (uint8_t z = 0; z < CHUNK_SZ_Z; ++z) {
			for (uint8_t x = 0; x < CHUNK_SZ_X; ++x) {
				if (!_processedVox[x][y][z]) {
					// unprocessed index
					if (_data.data[x][y][z] != 0) {
						searchQuadMesh(chunkVec3(x, y, z));
					}
				}
			}
		}
	}
}

void	GreedyChunk::_draw(glm::mat4 &view) {
	_textureManager.activateTextures();

	_shaderData->naiveShader->use();
	_shaderData->naiveShader->setMat4("view", view);
	glBindVertexArray(_shaderData->cubeVao);

	glm::mat4 baseModel = glm::translate(glm::mat4(1.0), glm::vec3(_chunkPos));
	for (MeshData const &md : _meshDatas) {
		_shaderData->naiveShader->setInt("blockId", md.blockId - 1);
		_shaderData->naiveShader->setVec3("size", glm::vec3(md.size));

		glm::mat4 model = glm::translate(baseModel, glm::vec3(md.pos));
		model = glm::scale(model, glm::vec3(md.size));
		_shaderData->naiveShader->setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void	GreedyChunk::sendCubeData() {
    glGenVertexArrays(1, &_shaderData->cubeVao);
    glGenBuffers(1, &(_shaderData->cubeVbo));

    glBindBuffer(GL_ARRAY_BUFFER, _shaderData->cubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GreedyChunk::_cubeData), GreedyChunk::_cubeData, GL_STATIC_DRAW);

    glBindVertexArray(_shaderData->cubeVao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

	_shaderData->naiveShader->use();
	// set cube material
	Material material;
	_shaderData->naiveShader->use();
	_shaderData->naiveShader->setBool("material.specular.isTexture", false);
	_shaderData->naiveShader->setVec3("material.specular.color", material.specular);
	_shaderData->naiveShader->setFloat("material.shininess", material.shininess);

	// set direction light
	_shaderData->naiveShader->setVec3("dirLight.direction", -0.2f, -0.8f, -0.6f);
	_shaderData->naiveShader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_shaderData->naiveShader->setVec3("dirLight.diffuse", 1.5f, 1.5f, 1.5f);
	_shaderData->naiveShader->setVec3("dirLight.specular", 1, 1, 1);

	_textureManager.setUniform(*_shaderData->naiveShader);
}
