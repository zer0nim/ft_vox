#ifndef TEXTUREMANAGER_HPP_
#define TEXTUREMANAGER_HPP_

#include <map>
#include <vector>

#include "libs/json.hpp"
#include "utils/Shader.hpp"
#include "commonInclude.hpp"

#define NB_TYPE_BLOCKS 42  // number of blocks types

class TextureManager {
	public:
		explicit TextureManager(std::string const &texturesSettings);
		TextureManager(TextureManager const &src);
		virtual ~TextureManager();

		TextureManager &operator=(TextureManager const &rhs);

		struct Texture {
			Texture(): id(0), path("") {}
			Texture(uint32_t const &id, std::string const &path)
			: id(id), path(path) {}
			~Texture() {
				if (id != 0) {
					glDeleteTextures(1, &id);
				}
			}
			uint32_t	id;
			std::string	path;
		};
		struct BlockTexture {
			BlockTexture() {
				side = -1;
				top = -1;
				bottom = -1;
				isTransparent = false;
			};

			int8_t	side;
			int8_t	top;
			int8_t	bottom;
			bool	isTransparent;
		};

		static std::map<std::string, uint8_t>	blocksNames;

		void									setUniform(Shader &sh) const;
		void									activateTextures() const;
		std::array<BlockTexture *, NB_TYPE_BLOCKS> const &	getBlocks() const;

		// Exceptions _______________________________
		class TextureManagerError : public std::exception {
			public:
				virtual const char* what() const throw() = 0;
		};
		class JsonParseException : public TextureManagerError {
			public:
				virtual const char* what() const throw() {
					return ("Failed to parse json texture settings");
				}
		};
		class FailedToOpenException : public TextureManagerError {
			public:
				explicit FailedToOpenException(std::string const &msg) {
					std::string s("Failed to open " + msg);
					s.resize(511);  // limit string size
					s.copy(cstr, s.size() + 1);
					cstr[s.size()] = '\0';
				}
				virtual const char * what() const throw() {
					return cstr;
				}
			private:
				char	cstr[512];
		};
		class missingBlockException : public TextureManagerError {
			public:
				virtual const char* what() const throw() {
					return ("Missing some blocks textures Infos");
				}
		};
		class failed2LoadTextureException : public TextureManagerError {
			public:
				virtual const char* what() const throw() {
					return ("Failed to load texture");
				}
		};

	private:
		void	loadBlocksTextures(nlohmann::json const &data);
		void	drawBlocks() const;


		Texture *	_textureAtlas;
		std::array<BlockTexture *, NB_TYPE_BLOCKS>			_blocks;
};

std::ostream & operator << (std::ostream &out, const TextureManager::Texture &m);
std::ostream & operator << (std::ostream &out, const TextureManager &tm);

#endif  // TEXTUREMANAGER_HPP_
