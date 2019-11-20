#version 410 core

#define MAX_TEXTURES 12

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aFaceId;

out VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	flat int TextureId;
} vs_out;

struct	BlockTexture {
	int	textureSide;
	int	textureTop;
	int	textureBottom;
};
uniform BlockTexture[4] blockTexturesInfo;
uniform sampler2D[MAX_TEXTURES] blockTextures;

uniform int blockId;
uniform vec3 size;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec4 pos = vec4(aPos, 1.0);

	// vs_out.FragPos = vec3(aPos);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;

	if (aFaceId == 4.0) {  // top
		vs_out.TextureId = blockTexturesInfo[blockId].textureTop;
	}
	else if  (aFaceId == 5.0) {  // bottom
		vs_out.TextureId = blockTexturesInfo[blockId].textureBottom;
	}
	else {
		vs_out.TextureId = blockTexturesInfo[blockId].textureSide;
	}

	if (aFaceId == 4 || aFaceId == 5) {
		vs_out.TexCoords = aTexCoords * size.xz;
	}
	else if (aFaceId == 0 || aFaceId == 2) {
		vs_out.TexCoords = aTexCoords * size.xy;
	}
	else if (aFaceId == 1 || aFaceId == 3) {
		vs_out.TexCoords = aTexCoords * size.zy;
	}

	gl_Position = projection * view * model * pos;
}
