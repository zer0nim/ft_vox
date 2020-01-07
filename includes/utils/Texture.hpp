#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <stdexcept>
#include "commonInclude.hpp"

u_int32_t	textureFromFile(const std::string path, bool inSpaceSRGB);
u_int32_t	textureAtlasFromFile(const std::string path, bool inSpaceSRGB, \
	int tileSize, int layerCount);

class TextureFailToLoad : public std::exception {
	public:
		char const * what() const throw() {
			return "failed to load texture";
		}
};

#endif  // TEXTURE_HPP_
