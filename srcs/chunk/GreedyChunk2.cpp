#include <array>
#include <vector>

#include "GreedyChunk2.hpp"
#include "utils/Material.hpp"

std::unique_ptr<GreedyChunk2::ShaderData>	GreedyChunk2::_shaderData = std::unique_ptr<GreedyChunk2::ShaderData>();

GreedyChunk2::GreedyChunk2(TextureManager const &textureManager, glm::mat4 &projection)
: AChunk(textureManager, projection) {
	_nbVertices = 0;
	_needInitVao = true;
	_meshUpdated = false;
	_vao = 0;
	_vbo = 0;
	if (!_shaderData) {
		_shaderData = std::unique_ptr<ShaderData>(new ShaderData());
		_shaderData->greedyShader->use();
		_shaderData->greedyShader->setMat4("projection", projection);
		sendConstUniforms();
	}
}

GreedyChunk2::GreedyChunk2(GreedyChunk2 const &src) : AChunk(src) {
	*this = src;
}

GreedyChunk2::~GreedyChunk2() {
}

GreedyChunk2 &GreedyChunk2::operator=(GreedyChunk2 const &rhs) {
	// TODO(zer0nim): need to fill the copy operator
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
		backFace is equal to true on the first loop and false on the second
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
				side = backFace ? Direction::LEFT : Direction::RIGHT;
			}
			else if (d == 1) {
				side = backFace ? Direction::BOTTOM : Direction::TOP;
			}
			else if (d == 2) {
				side = backFace ? Direction::BACK : Direction::FRONT;
			}

			// iterate through the dimension from front to back
			for (it[d] = -1; it[d] < dims[d]; ++(it[d])) {
				// compute the mask ___________________________________________
				int n = 0;
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

								++it[d];  // increment curent direction to get correct position

								Quad	quad;
								quad.voxFace = *(mask[n]);
								quad.width = (d != 2) ? h : w;
								quad.height = (d != 2) ? w : h;
								quad.backFace = backFace;
								if (d != 2) {
									quad.bottomLeft = chunkVec3(it[0],					it[1],					it[2]);
									quad.topLeft = chunkVec3(it[0] + du[0],				it[1] + du[1],			it[2] + du[2]);
									quad.topRight = chunkVec3(it[0] + du[0] + dv[0],	it[1] + du[1] + dv[1],	it[2] + du[2] + dv[2]);
									quad.bottomRight = chunkVec3(it[0] + dv[0],			it[1] + dv[1],			it[2] + dv[2]);
								}
								else {
									quad.bottomRight = chunkVec3(it[0],					it[1],					it[2]);
									quad.bottomLeft = chunkVec3(it[0] + du[0],				it[1] + du[1],			it[2] + du[2]);
									quad.topLeft = chunkVec3(it[0] + du[0] + dv[0],	it[1] + du[1] + dv[1],	it[2] + du[2] + dv[2]);
									quad.topRight = chunkVec3(it[0] + dv[0],			it[1] + dv[1],			it[2] + dv[2]);
								}

								--it[d];  // reset curent direction

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

void	GreedyChunk2::fillVectLine(std::vector<float> &vertices, int & i, \
chunkVec3 const &pos, glm::tvec2<int8_t> textUv, Quad const &q) {
	// std::cout << "pos: " << glm::to_string(pos) << std::endl;

	// position
	vertices[++i] = pos.x;
	vertices[++i] = pos.y;
	vertices[++i] = pos.z;
	// texture coords
	vertices[++i] = textUv.x * q.width;
	vertices[++i] = textUv.y * q.height;
	// faceId
	vertices[++i] = static_cast<float>(q.voxFace.side);
	// blockId
	vertices[++i] = static_cast<float>(q.voxFace.type - 1);
}

void	GreedyChunk2::update() {
	if (_data.isModified == false)
		return;  // GreedyChunk2 not modified -> don't update it
	_data.isModified = false;

	// update mesh
	calcGreedyChunk();
	_meshUpdated = true;
}

void	GreedyChunk2::sendMeshData() {
	if (_quads.size() > 0) {
		int const rowSize = 7;
		_nbVertices = _quads.size() * 6;
		int const size = _nbVertices * rowSize;
		std::vector<float> vertices(size);

		// fill vertices array
		int i = -1;
		for (Quad &q : _quads) {
			if (q.backFace) {
				fillVectLine(vertices, i, q.topLeft, glm::tvec2<int8_t>(0, 0), q);
				fillVectLine(vertices, i, q.bottomLeft, glm::tvec2<int8_t>(0, 1), q);
				fillVectLine(vertices, i, q.bottomRight, glm::tvec2<int8_t>(1, 1), q);

				fillVectLine(vertices, i, q.bottomRight, glm::tvec2<int8_t>(1, 1), q);
				fillVectLine(vertices, i, q.topRight, glm::tvec2<int8_t>(1, 0), q);
				fillVectLine(vertices, i, q.topLeft, glm::tvec2<int8_t>(0, 0), q);
			}
			else {
				fillVectLine(vertices, i, q.topLeft, glm::tvec2<int8_t>(0, 0), q);
				fillVectLine(vertices, i, q.topRight, glm::tvec2<int8_t>(1, 0), q);
				fillVectLine(vertices, i, q.bottomRight, glm::tvec2<int8_t>(1, 1), q);

				fillVectLine(vertices, i, q.bottomRight, glm::tvec2<int8_t>(1, 1), q);
				fillVectLine(vertices, i, q.bottomLeft, glm::tvec2<int8_t>(0, 1), q);
				fillVectLine(vertices, i, q.topLeft, glm::tvec2<int8_t>(0, 0), q);
			}
		}

		if (_nbVertices > 0) {
			_shaderData->greedyShader->use();

			// generate vao vbo only on first call
			if (_needInitVao) {
				_needInitVao = false;
				glGenVertexArrays(1, &_vao);
				glGenBuffers(1, &_vbo);
			}

			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

			glBindVertexArray(_vao);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(5 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
			glEnableVertexAttribArray(3);
		}
	}
}

void	GreedyChunk2::_draw(glm::mat4 &view) {
	if (_meshUpdated) {
		_meshUpdated = false;
		_nbVertices = 0;
		sendMeshData();
	}

	if (_nbVertices > 0) {
		_textureManager.activateTextures();

		_shaderData->greedyShader->use();
		_shaderData->greedyShader->setMat4("view", view);

		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(_chunkPos));
		_shaderData->greedyShader->setMat4("model", model);

		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLES, 0, _nbVertices);
	}
}

void	GreedyChunk2::sendConstUniforms() {
	_shaderData->greedyShader->use();

	// set cube material
	Material material;
	_shaderData->greedyShader->setVec3("material.specular", material.specular);
	_shaderData->greedyShader->setFloat("material.shininess", material.shininess);

	// set direction light
	_shaderData->greedyShader->setVec3("dirLight.direction", -0.2f, -0.8f, -0.6f);
	_shaderData->greedyShader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_shaderData->greedyShader->setVec3("dirLight.diffuse", 1.5f, 1.5f, 1.5f);
	_shaderData->greedyShader->setVec3("dirLight.specular", 1, 1, 1);

	// send textures
	_textureManager.setUniform(*_shaderData->greedyShader);
}
