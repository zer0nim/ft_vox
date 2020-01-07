#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "Logging.hpp"

u_int32_t	textureFromFile(const std::string path, bool inSpaceSRGB) {
	u_int32_t	textureID;
    int			nrComponents;
    int			width;
    int			height;
	u_char		*data;
	GLint		intFormat;
	GLenum		format;


	data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

	if (data) {
		glGenTextures(1, &textureID);
		if (nrComponents == 1) {
			intFormat = GL_RED;
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			intFormat = inSpaceSRGB ? GL_SRGB : GL_RGB;
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			intFormat = inSpaceSRGB ? GL_SRGB_ALPHA : GL_RGBA;
			format = GL_RGBA;
		}
		else {
			logErr("failed to load texture " << path);
			throw TextureFailToLoad();
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, width, height, 0, format, \
		GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 5);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}
	else {
		logErr("Texture failed to load at path: " << path);
		stbi_image_free(data);
		throw TextureFailToLoad();
	}

    return textureID;
}
