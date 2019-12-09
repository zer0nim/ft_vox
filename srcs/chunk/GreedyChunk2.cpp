#include <array>
#include <vector>

#include "GreedyChunk2.hpp"
#include "utils/Material.hpp"

GreedyChunk2::GreedyChunk2(TextureManager const &textureManager)
: AChunk(textureManager) {
	_nbVertices = 0;
	_needInitVao = true;
	_meshUpdated = false;
	_vao = 0;
	_vbo = 0;
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

void	GreedyChunk2::initShader(glm::mat4 &projection, TextureManager const &textureManager) {
	if (!_shaderData) {
		_shaderData = std::unique_ptr<ShaderData>(new AChunk::ShaderData(
			"./shaders/greedyChunk2_vs.glsl", "./shaders/naive_fs.glsl"));
		_shaderData->shader->use();
		_shaderData->shader->setMat4("projection", projection);
		sendConstUniforms(textureManager);
	}
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

/*
	generate faces by merging consecutive identicals blocks
*/
void	GreedyChunk2::calcGreedyChunk() {
	chunkVec3 chunkSz = {CHUNK_SZ_X, CHUNK_SZ_Y, CHUNK_SZ_Z};

	std::array<VoxFace *, CHUNK_SZ_Y * CHUNK_SZ_X>	mask = { nullptr };
	/*
		the greedy chunk algorithm work with a mask.
		the mask will contain the faces position, size, and type for the current
		working direction.

		example with chunk size { 3, 3, 3 }:

		chunk content:
		(number define block type, 0 mean empty block)
								|	2  2  0
								|	 2  2  0
		    7 Y-axis			|	  0  0  0
		   /					|
		  /						|	2  2  0
		 /						|	 2  2  0
		+- - - - - - > X-axis	|	  0  0  0
		 \						|
		  \						|	2  2  0
		   \					|	 2  2  0
		    v Z-axis			|	  1  0  0


		example of mask usage when working on the LEFTS faces:
		d == 0 (x),   u == 1 (y),   v == 2 (z)

					u (y)
				- - - - >
		mask = {2, 2, 2,	|
				2, 2, 2,	|	v (z)
				1, 0, 0}	v

		that means with have two faces stored in this mask:
			{ type: 2, width: 3, height: 2, pos: { 0(d), 0(u), 0(v) } }
		and
			{ type: 1, width: 1, height: 1, pos: { 0(d), 0(u), 2(v) } }
	*/

	/*
		flip flop loop, run twice:
		1st loop: backFace == true, 2nd loop backFace == false

		The inner loop 3 times, totally 6 iterations, one for each voxel face.
	*/
	bool backFace = true;
	for (uint8_t i = 0; i < 2; ++i, backFace = !backFace) {
		// loop over 3-axes
		for (uint8_t d = 0; d < 3; ++d) {
			uint8_t u = (d + 1) % 3;  // d+1, if d == 0(x)  =>  u == 1(y)
			uint8_t v = (d + 2) % 3;  // d+2, if d == 0(x)  =>  v == 2(z)

			// get the side we are currently working on
			Direction	side = Direction::BOTTOM;
			if (d == 0) {
				side = backFace ? Direction::LEFT : Direction::RIGHT;
			}
			else if (d == 1) {
				side = backFace ? Direction::BOTTOM : Direction::TOP;
			}
			else if (d == 2) {
				side = backFace ? Direction::BACK : Direction::FRONT;
			}

			// one iterator per dimension
			std::array<int16_t, 3> it = {0, 0, 0};

			// iterate through the dimension from front to back
			for (it[d] = -1; it[d] < chunkSz[d]; ++(it[d])) {
				// compute the mask ___________________________
				int n = 0;  // mask iterator
				for (it[v] = 0; it[v] < chunkSz[v]; ++it[v]) {
					for (it[u] = 0; it[u] < chunkSz[u]; ++it[u], ++n) {
						chunkVec3 voxPos = {it[0], it[1], it[2]};

						// get curent block
						VoxFace	*a = (it[d] >= 0)
							? getVoxFace(voxPos, side)
							: nullptr;

						// get next block
						voxPos[d] += 1;  // next block
						VoxFace	*b = (it[d] < chunkSz[d] - 1)
							? getVoxFace(voxPos, side)
							: nullptr;

						// fill the mask
						if (a != nullptr && b != nullptr && *a == *b) {
							mask[n] = nullptr;
							delete a;
							delete b;
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

				// generate the mesh from the mask ____________
				n = 0;
				for (uint8_t j = 0; j < chunkSz[v]; ++j) {
					for (uint8_t i = 0; i < chunkSz[u];) {
						if (mask[n] != nullptr) {
							uint8_t w, h;  // face size

							// compute the width
							for (w = 1; i + w < chunkSz[u] && mask[n + w] != nullptr && \
							*(mask[n + w]) == *(mask[n]); ++w) {}

							// compute the height
							bool done = false;
							for (h = 1; j + h < chunkSz[v]; ++h) {
								for (uint8_t k = 0; k < w; ++k) {
									if (mask[n + k + h * chunkSz[u]] == nullptr || \
										*(mask[n + k + h * chunkSz[u]]) != *(mask[n])) {
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

								// create and save the face
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
							for (uint8_t l = 0; l < h; ++l) {
								for (uint8_t k = 0; k < w; ++k) {
									if (mask[n + k + l * chunkSz[u]] != nullptr) {
										delete mask[n + k + l * chunkSz[u]];
										mask[n + k + l * chunkSz[u]] = nullptr;
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

	// update mesh
	calcGreedyChunk();
	_meshUpdated = true;
}

void	GreedyChunk2::fillVectLine(std::vector<float> &vertices, int & i, \
chunkVec3 const &pos, glm::tvec2<int8_t> textUv, Quad const &q) {
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
			_shaderData->shader->use();

			// generate vao vbo only on first call
			if (_needInitVao) {
				_needInitVao = false;
				glGenVertexArrays(1, &_vao);
				glGenBuffers(1, &_vbo);
			}

			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

			glBindVertexArray(_vao);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, rowSize * sizeof(float), reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, rowSize * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, rowSize * sizeof(float), reinterpret_cast<void*>(5 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, rowSize * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
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
		_shaderData->shader->use();
		_textureManager.activateTextures();
		_shaderData->shader->setMat4("view", view);

		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(_chunkPos));
		_shaderData->shader->setMat4("model", model);

		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLES, 0, _nbVertices);
	}
}

void	GreedyChunk2::sendConstUniforms(TextureManager const &textureManager) {
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

	// send textures
	textureManager.setUniform(*_shaderData->shader);
}
