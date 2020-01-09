#include <array>
#include <vector>
#include <algorithm>

#include "GreedyChunk3.hpp"
#include "ChunkManager.hpp"
#include "utils/Material.hpp"

GreedyChunk3::GreedyChunk3(TextureManager const &textureManager, ChunkManager &chunkManager)
: AChunk(textureManager, chunkManager) {
	_nbVertices = 0;
	_needInitVao = true;
	_meshUpdated = false;
	_vao = 0;
	_vbo = 0;
}

GreedyChunk3::GreedyChunk3(GreedyChunk3 const &src) : AChunk(src) {
	*this = src;
}

GreedyChunk3::~GreedyChunk3() {
	if (_needInitVao == false) {
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
	}
}

GreedyChunk3 &GreedyChunk3::operator=(GreedyChunk3 const &rhs) {
	// TODO(zer0nim): need to fill the copy operator
	(void)rhs;
	// if (this != &rhs) {}
	return *this;
}

void	GreedyChunk3::initShader(glm::mat4 &projection, TextureManager const &textureManager) {
	if (!_shaderData) {
		_shaderData = std::unique_ptr<ShaderData>(new AChunk::ShaderData(
			"./shaders/greedyChunk3_vs.glsl", "./shaders/naive_fs.glsl", \
			"./shaders/greedyChunk3_gs.glsl"));
		_shaderData->shader->use();
		_shaderData->shader->setMat4("projection", projection);
		sendConstUniforms(textureManager);
	}
}

uint8_t	GreedyChunk3::getBlockOutside(wordIVec3 voxPos, std::array<AChunk*, 2> &nearbyChunks, int d, int sz) {
	uint8_t		val = UNKNOW_BLOCK;

	// chunk before
	if (voxPos[d] < 0) {  // chunk not loaded
		if (nearbyChunks[0] == nullptr) {
			val = UNKNOW_BLOCK;
		}
		else {  // chunk loaded
			voxPos[d] += sz;
		    { std::lock_guard<std::mutex>	guard(nearbyChunks[0]->mutexChunk);
				val = nearbyChunks[0]->getData().data[voxPos.x][voxPos.y][voxPos.z];
			}
		}
	}
	// chunk after
	else if (voxPos[d] >= sz) {
		if (nearbyChunks[1] == nullptr) {  // chunk not loaded
			val = UNKNOW_BLOCK;
		}
		else {  // chunk loaded
			voxPos[d] -= sz;
		    { std::lock_guard<std::mutex>	guard(nearbyChunks[1]->mutexChunk);
				val = nearbyChunks[1]->getData().data[voxPos.x][voxPos.y][voxPos.z];
			}
		}
	}
	// current chunk
	else {
		val = _data.data[voxPos.x][voxPos.y][voxPos.z];
	}

	return val;
}

/*
	generate faces by merging consecutive identicals blocks
*/
void	GreedyChunk3::calcGreedyChunk(bool firstUpdate) {
	chunkVec3 chunkSz = {CHUNK_SZ_X, CHUNK_SZ_Y, CHUNK_SZ_Z};

	_faces.clear();

	std::array<uint8_t, CHUNK_SZ_Y * CHUNK_SZ_X>	mask = { 0 };
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
	for (uint8_t flipFlop = 0; flipFlop < 2; ++flipFlop, backFace = !backFace) {
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

				std::array<AChunk*, 2>	nearbyChunks = {nullptr};  // contains the chunk before and the chunk after (if exists)
				wordIVec3 nearbyPos = _chunkPos;
				// chunk before
				nearbyPos[d] -= chunkSz[d];
			    {
					if (firstUpdate)
						std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
					if (_chunkManager.isChunkExist(nearbyPos)) {
						nearbyChunks[0] = _chunkManager.getChunkMap()[nearbyPos];
					}
				}
				// chunk after
				nearbyPos[d] += chunkSz[d] * 2;
			    {
					if (firstUpdate)
						std::lock_guard<std::mutex>	guard(s.mutexChunkMap);
					if (_chunkManager.isChunkExist(nearbyPos)) {
						nearbyChunks[1] = _chunkManager.getChunkMap()[nearbyPos];
					}
				}

				int n = 0;  // mask iterator
				for (it[v] = 0; it[v] < chunkSz[v]; ++it[v]) {
					for (it[u] = 0; it[u] < chunkSz[u]; ++it[u], ++n) {
						wordIVec3 voxPos = {it[0], it[1], it[2]};

						// get curent block
						uint8_t a;
						if (voxPos[d] >= 0) {
							a = _data.data[voxPos.x][voxPos.y][voxPos.z];
						}
						else {
							a = getBlockOutside(voxPos, nearbyChunks, d, chunkSz[d]);
						}
						// get next block
						voxPos[d] += 1;  // next block
						uint8_t b;
						if (voxPos[d] < chunkSz[d]) {
							b = _data.data[voxPos.x][voxPos.y][voxPos.z];
						}
						else {
							b = getBlockOutside(voxPos, nearbyChunks, d, chunkSz[d]);
						}

						// fill the mask
						if (a != 0 && b != 0 && a != UNKNOW_BLOCK && b != UNKNOW_BLOCK
						&& _textureManager.getBlocks()[a - 1]->isTransparent == _textureManager.getBlocks()[b - 1]->isTransparent) {
							mask[n] = 0;
						}
						else if (backFace) {
							mask[n] = b;
						}
						else {
							mask[n] = a;
						}
						if (mask[n] == UNKNOW_BLOCK) {
							mask[n] = 0;
						}
					}
				}

				// generate the mesh from the mask ____________
				n = 0;
				for (uint8_t j = 0; j < chunkSz[v]; ++j) {
					for (uint8_t i = 0; i < chunkSz[u];) {
						if (mask[n] != 0) {
							uint8_t w, h;  // face size

							// compute the width
							for (w = 1; i + w < chunkSz[u] && mask[n + w] != 0 && \
							mask[n + w] == mask[n]; ++w) {}

							// compute the height
							bool done = false;
							for (h = 1; j + h < chunkSz[v]; ++h) {
								for (uint8_t k = 0; k < w; ++k) {
									if (mask[n + k + h * chunkSz[u]] == 0 || \
										mask[n + k + h * chunkSz[u]] != mask[n]) {
										done = true;
										break;
									}
								}
								if (done) {
									break;
								}
							}

							// add quad
							it[u] = i;
							it[v] = j;

							std::array<uint8_t, 3> du = {0, 0, 0};
							du[u] = w;
							std::array<uint8_t, 3> dv = {0, 0, 0};
							dv[v] = h;

							++it[d];  // increment curent direction to get correct position

							// create and save the face
							chunkVec3 pos = {0, 0, 0};
							if (d != 2) {
								if (side == Direction::FRONT || side == Direction::RIGHT) {
									pos.x = it[0] + dv[0];
									pos.y = it[1] + dv[1];
									pos.z = it[2] + dv[2];
								}
								else if (side == Direction::BACK || side == Direction::LEFT || \
									side == Direction::BOTTOM) {
									pos.x = it[0];
									pos.y = it[1];
									pos.z = it[2];
								}
								else if (side == Direction::TOP) {
									pos.x = it[0] + du[0];
									pos.y = it[1] + du[1];
									pos.z = it[2] + du[2];
								}
							}
							else {
								if (side == Direction::FRONT || side == Direction::RIGHT) {
									pos.x = it[0];
									pos.y = it[1];
									pos.z = it[2];
								}
								else if (side == Direction::BACK || side == Direction::LEFT || \
									side == Direction::BOTTOM) {
									pos.x = it[0] + du[0];
									pos.y = it[1] + du[1];
									pos.z = it[2] + du[2];
								}
								else if (side == Direction::TOP) {
									pos.x = it[0] + du[0] + dv[0];
									pos.y = it[1] + du[1] + dv[1];
									pos.z = it[2] + du[2] + dv[2];
								}
							}

							// add the quad to the result array
							_faces.insert(_faces.end(), {
								// position
								static_cast<float>(pos.x),
								static_cast<float>(pos.y),
								static_cast<float>(pos.z),
								// size
								static_cast<float>((d != 2) ? h : w),
								static_cast<float>((d != 2) ? w : h),
								// faceId
								static_cast<float>(side),
								// blockId
								static_cast<float>(mask[n] - 1)
							});

							--it[d];  // reset curent direction

							// zero out the mask
							for (uint8_t l = 0; l < h; ++l) {
								for (uint8_t k = 0; k < w; ++k) {
									if (mask[n + k + l * chunkSz[u]] != 0) {
										mask[n + k + l * chunkSz[u]] = 0;
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

void	GreedyChunk3::update(bool firstUpdate) {
	if (_data.isModified == false)
		return;  // GreedyChunk3 not modified -> don't update it
	_data.isModified = false;

	// update mesh
	calcGreedyChunk(firstUpdate);
	_meshUpdated = true;
}

void	GreedyChunk3::sendMeshData() {
	if (_faces.size() > 0) {
		int const rowSize = 7;
		_nbVertices = _faces.size() / rowSize;

		if (_nbVertices > 0) {
			_shaderData->shader->use();

			// generate vao vbo only on first call
			if (_needInitVao) {
				_needInitVao = false;
				glGenVertexArrays(1, &_vao);
				glGenBuffers(1, &_vbo);
			}

			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, _faces.size() * sizeof(float), &_faces[0], GL_STATIC_DRAW);

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

void	GreedyChunk3::_draw(glm::mat4 &view, glm::vec3 &pos) {
	if (_meshUpdated) {
		_meshUpdated = false;
		_nbVertices = 0;
		sendMeshData();
	}

	if (_nbVertices > 0) {
		_shaderData->shader->use();
		_textureManager.activateTextures();

		_shaderData->shader->setVec3("viewPos", pos);
		_shaderData->shader->setMat4("view", view);

		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(_chunkPos));
		_shaderData->shader->setMat4("model", model);

		glBindVertexArray(_vao);
		glDrawArrays(GL_POINTS, 0, _nbVertices);
	}
}

void	GreedyChunk3::sendConstUniforms(TextureManager const &textureManager) {
	_shaderData->shader->use();

	// set cube material
	Material material;
	_shaderData->shader->setVec3("material.specular", material.specular);
	_shaderData->shader->setFloat("material.shininess", material.shininess);

	// set direction light
	_shaderData->shader->setVec3("dirLight.direction", -0.2f, -0.8f, 0.6f);
	_shaderData->shader->setVec3("dirLight.ambient", 0.4f, 0.4f, 0.4f);
	_shaderData->shader->setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
	_shaderData->shader->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

	// send textures
	textureManager.setUniform(*_shaderData->shader);

	_shaderData->shader->setBool("enableTransparency", s.g.screen.enableTransparency);

	// set fog settings
	int dist = s.g.renderDist * std::min(CHUNK_SZ_X, CHUNK_SZ_Z);

	_shaderData->shader->setBool("fog.enabled", s.g.fog.enabled);
	_shaderData->shader->setInt("fog.maxDist", dist);
	_shaderData->shader->setInt("fog.minDist", dist - s.g.fog.width);
	_shaderData->shader->setVec4("fog.color", s.g.fog.color);
}

uint32_t	GreedyChunk3::getNbSquareRendered() const {
    { std::lock_guard<std::mutex>	guard(mutexChunk);
		return _faces.size();
	}
}
