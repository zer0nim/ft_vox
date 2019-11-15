#ifndef TEXTUREMANAGER_HPP_
#define TEXTUREMANAGER_HPP_

#include <array>
#include <vector>
#include <map>

#include "lib/json.hpp"
#include "commonInclude.hpp"

class TextureManager {
	public:
		explicit TextureManager(std::string const &texturesSettings);
		TextureManager(TextureManager const &src);
		virtual ~TextureManager();

		TextureManager &operator=(TextureManager const &rhs);

		struct Texture {
			Texture() {}
			Texture(u_int32_t const &id, std::string const &path)
			: id(id), path(path) {}
			u_int32_t	id;
			std::string	path;
		};
		struct BlockTexture {
			BlockTexture() {
				side = nullptr;
				top = nullptr;
				bottom = nullptr;
			};

			Texture	*side;
			Texture	*top;
			Texture	*bottom;
		};

		std::vector<Texture *> const &			getTexturesLoaded() const;
		std::array<BlockTexture *, 4> const &	getBlocks() const;
		void									drawBlocks() const;

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
				explicit FailedToOpenException(std::string const &msg) : _msg(msg) {
					std::string s("Failed to open " + _msg);
					s.resize(511);  // limit string size
					s.copy(cstr, s.size() + 1);
					cstr[s.size()] = '\0';
				}
				virtual const char * what() const throw() {
					return cstr;
				}
			private:
				std::string const _msg;
				char	cstr[512];
		};

	private:
		Texture	*loadTextures(std::string const &path);
		void	loadBlocksTextures(nlohmann::json const &data);


		std::vector<Texture *>						_texturesLoaded;
		std::array<BlockTexture *, 4>				_blocks;
		static const std::map<std::string, int8_t>	_blocksId;
};

std::ostream & operator << (std::ostream &out, const TextureManager::Texture &m);
std::ostream & operator << (std::ostream &out, const TextureManager::BlockTexture &m);

#endif  // TEXTUREMANAGER_HPP_
