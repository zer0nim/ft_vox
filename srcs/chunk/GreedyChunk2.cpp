#include <array>
#include <vector>

#include "GreedyChunk2.hpp"
#include "utils/Material.hpp"

const float	GreedyChunk2::_cubeData[] = {
	// positions			// normals				// texture coords
	1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f,  // 0l
	0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0.0f,  // 0l
	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f,  // 0l

	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0.0f,  // 0r
	1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,		0.0f,  // 0r
	1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		0.0f,  // 0r


	1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1l
	1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		1.0f,  // 1l
	1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1l

	1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		1.0f,  // 1r
	1.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		1.0f,  // 1r
	1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		1.0f,  // 1r


	0.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		2.0f,  // 2l
	1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,		2.0f,  // 2l
	1.0f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		2.0f,  // 2l

	1.0f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,		2.0f,  // 2r
	0.0f, 1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,		2.0f,  // 2r
	0.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f,		2.0f,  // 2r


	0.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3l
	0.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,		3.0f,  // 3l
	0.0f, 0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3l

	0.0f, 0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,		3.0f,  // 3r
	0.0f, 0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		3.0f,  // 3r
	0.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,		3.0f,  // 3r


	0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4l
	1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,		4.0f,  // 4l
	1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4l

	1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		4.0f,  // 4r
	0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,		4.0f,  // 4r
	0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,		4.0f,  // 4r


	0.0f, 0.0f, 0.0f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f,  // 5l
	1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,		5.0f,  // 5l
	1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5l

	1.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,		5.0f,  // 5r
	0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,		5.0f,  // 5r
	0.0f, 0.0f, 0.0f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,		5.0f  // 5r
};

std::unique_ptr<GreedyChunk2::ShaderData>	GreedyChunk2::_shaderData = std::unique_ptr<GreedyChunk2::ShaderData>();

GreedyChunk2::GreedyChunk2(TextureManager const &textureManager, glm::mat4 &projection)
: AChunk(textureManager, projection) {
	if (!_shaderData) {
		_shaderData = std::unique_ptr<ShaderData>(new ShaderData());
		_shaderData->naiveShader->use();
		_shaderData->naiveShader->setMat4("projection", projection);
		sendCubeData();
	}
}

GreedyChunk2::GreedyChunk2(GreedyChunk2 const &src) : AChunk(src) {
	*this = src;
}

GreedyChunk2::~GreedyChunk2() {
}

GreedyChunk2 &GreedyChunk2::operator=(GreedyChunk2 const &rhs) {
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

/*
	get an instance of VoxelFace containing the attributes for
	one side of a voxel.
	check if the voxel face should be culled.
*/
GreedyChunk2::VoxFace	*GreedyChunk2::getVoxFace(chunkVec3 const pos, Direction const side) {
	VoxFace *voxFace = nullptr;
	uint8_t	type = _data.data[pos.x][pos.y][pos.z];

	if (type == 0) {
		return nullptr;
	}
	voxFace = new VoxFace();
	voxFace->type = type;
	voxFace->transparent = false;  // TODO(zer0nim): set transparency according to face culling
	voxFace->side = side;

	return voxFace;
}

void	GreedyChunk2::calcGreedyChunk() {
	/*
		mask, this will contain the groups of matching voxel faces
		as we proceed through the chunk in 6 directions
		once for each face.
	*/
	std::array<VoxFace *, CHUNK_SZ_Y * CHUNK_SZ_X>	mask = { nullptr };

	std::array<uint8_t, 3> dims = {CHUNK_SZ_X, CHUNK_SZ_Y, CHUNK_SZ_Z};
	uint8_t i, j, k;  // pos
	uint8_t l, w, h;  // size
	Direction	side = Direction::BOTTOM;
	VoxFace	*a = nullptr;
	VoxFace	*b = nullptr;

	/*
		This loop runs twice, and the inner loop 3 times - totally 6 iterations
		one for each voxel face.
	*/
	for (bool backFace = true, b2 = false; b2 != backFace; backFace = backFace && b2, b2 = !b2) {
		// loop over 3-axes
		for (uint8_t d = 0; d < 3; ++d) {
			uint8_t u = (d + 1) % 3;  // direction + 1	example for x direction u = y direction
			uint8_t v = (d + 2) % 3;  // direction + 2	example for x direction u = z direction

			std::array<int8_t, 3> it = {0, 0, 0};  // vec3 iterator, x, y, z
			std::array<uint8_t, 3> dir = {0, 0, 0};  // vec3 direction, x, y, z
			dir[d] = 1;

			// keep track of the side
			if (d == 0) {
				side = backFace ? Direction::WEST : Direction::EAST;
			}
			else if (d == 1) {
				side = backFace ? Direction::BOTTOM : Direction::TOP;
			}
			else if (d == 2) {
				side = backFace ? Direction::SOUTH : Direction::NORTH;
			}

			// iterate through the dimension from front to back
			for (it[d] = -1; it[d] < dims[d]; ++(it[d])) {
				// compute the mask ___________________________________________
				uint8_t n = 0;
				for (it[v] = 0; it[v] < dims[v]; ++(it[v])) {
					for (it[u] = 0; it[u] < dims[u]; ++(it[u]), ++n) {
						// get curent block
						a = (it[d] >= 0)
							? getVoxFace(chunkVec3{ it[0], it[1], it[2] }, side)
							: nullptr;
						// get next block
						b = (it[d] < dims[d] - 1)
							? getVoxFace(chunkVec3{ it[0] + dir[0], it[1] + dir[1], it[2] + dir[2] }, side)
							: nullptr;

						if (a != nullptr && b != nullptr && *a == *b) {
							mask[n] = nullptr;
						}
						else if (backFace) {
							mask[n] = b;
							delete a;
						}
						else {
							mask[n] = a;
							delete b;
						}
					}
				}

				// generate the mesh from the mask ____________________________

				n = 0;
				for (j = 0; j < dims[v]; ++j) {
					for (i = 0; i < dims[u];) {
						if (mask[n] != nullptr) {
							// compute the width
							for (w = 1; i + w < dims[u] && mask[n + w] != nullptr && \
							*(mask[n + w]) == *(mask[n]); ++w) {}

							// compute the height
							bool done = false;
							for (h = 1; j + h < dims[v]; ++h) {
								for (k = 0; k < w; ++k) {
									if (mask[n + k + h * dims[u]] == nullptr || \
										*(mask[n + k + h * dims[u]]) != *(mask[n])) {
										done = true;
										break;
									}
								}
								if (done) {
									break;
								}
							}

							/*
								check the "transparent" attribute in the VoxelFace
								to ensure that we don't mesh any culled faces.
							*/
							if (!mask[n]->transparent) {
								// add quad
								it[u] = i;
								it[v] = j;

								std::array<uint8_t, 3> du = {0, 0, 0};
								du[u] = w;
								std::array<uint8_t, 3> dv = {0, 0, 0};
								dv[v] = h;

								Quad	quad;
								quad.voxFace = *(mask[n]);
								quad.width = w;
								quad.height = h;
								quad.backFace = backFace;
								quad.bottomLeft = chunkVec3(it[0],					it[1],					it[2]);
								quad.topLeft = chunkVec3(it[0] + du[0],			it[1] + du[1],			it[2] + du[2]);
								quad.topRight = chunkVec3(it[0] + du[0] + dv[0],	it[1] + du[1] + dv[1],	it[2] + du[2] + dv[2]);
								quad.bottomRight = chunkVec3(it[0] + dv[0],			it[1] + dv[1],			it[2] + dv[2]);

								_quads.push_back(quad);
							}

							// zero out the mask
							for (l = 0; l < h; ++l) {
								for (k = 0; k < w; ++k) {
									if (mask[n + k + l * dims[u]] != nullptr) {
										delete mask[n + k + l * dims[u]];
										mask[n + k + l * dims[u]] = nullptr;
									}
								}
							}

							// finally increment the counters and continue
							i += w;
							n += w;

						} else {
							++i;
							++n;
						}
					}
				}
			}
		}
	}
}

void	GreedyChunk2::update() {
	if (_data.isModified == false)
		return;  // GreedyChunk2 not modified -> don't update it
	_data.isModified = false;

	calcGreedyChunk();

	std::cout << "_quads: " << _quads.size() << std::endl;
	for (Quad &q : _quads) {
		std::cout << "quads_____" << std::endl;
		std::cout << "type: " << static_cast<int>(q.voxFace.type) << std::endl;
		std::cout << "width: " << static_cast<int>(q.width) << std::endl;
		std::cout << "height: " << static_cast<int>(q.height) << std::endl;
		std::cout << "backFace: " << q.backFace << std::endl;
		std::cout << "bottomLeft: " << glm::to_string(q.bottomLeft) << std::endl;
		std::cout << "topLeft: " << glm::to_string(q.topLeft) << std::endl;
		std::cout << "topRight: " << glm::to_string(q.topRight) << std::endl;
		std::cout << "bottomRight: " << glm::to_string(q.bottomRight) << std::endl;
	}
}

void	GreedyChunk2::_draw(glm::mat4 &view) const {
	_textureManager.activateTextures();

	// _shaderData->naiveShader->use();
	// _shaderData->naiveShader->setMat4("view", view);
	// glBindVertexArray(_shaderData->cubeVao);

	// glm::mat4 baseModel = glm::translate(glm::mat4(1.0), glm::vec3(_chunkPos));
	// for (MeshData const &md : _meshDatas) {
	// 	_shaderData->naiveShader->setInt("blockId", md.blockId - 1);
	// 	_shaderData->naiveShader->setVec3("size", glm::vec3(md.size));

	// 	glm::mat4 model = glm::translate(baseModel, glm::vec3(md.pos));
	// 	model = glm::scale(model, glm::vec3(md.size));
	// 	_shaderData->naiveShader->setMat4("model", model);

	// 	glDrawArrays(GL_TRIANGLES, 0, 36);
	// }
}

void	GreedyChunk2::sendCubeData() {
    glGenVertexArrays(1, &_shaderData->cubeVao);
    glGenBuffers(1, &(_shaderData->cubeVbo));

    glBindBuffer(GL_ARRAY_BUFFER, _shaderData->cubeVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GreedyChunk2::_cubeData), GreedyChunk2::_cubeData, GL_STATIC_DRAW);

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
