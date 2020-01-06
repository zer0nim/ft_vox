#version 410 core

#define NB_BLOCK_TYPES 8

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aSize;
layout (location = 2) in float aFaceId;
layout (location = 3) in float blockId;

out GS_IN {
	flat int FaceId;
	vec2 FSize;
	vec3 Normal;
	flat int TextureId;
} vs_out;

struct	BlockTexture {
	int	textureSide;
	int	textureTop;
	int	textureBottom;
};
uniform BlockTexture[NB_BLOCK_TYPES] blockTexturesInfo;
uniform sampler2D	textureAtlas;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// TODO(zer0nim): need to compare speed with uniform
const vec3 normals[6] = vec3[6](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f),
	vec3(0.0f, -1.0f, 0.0f)
);

void main() {
	vs_out.FaceId = int(aFaceId);
	vs_out.FSize = aSize;
	vs_out.Normal = mat3(transpose(inverse(model))) * normals[int(aFaceId)];

	BlockTexture bt = blockTexturesInfo[int(blockId)];
	if (aFaceId == 4.0) {  // top
		vs_out.TextureId = bt.textureTop;
	}
	else if  (aFaceId == 5.0) {  // bottom
		vs_out.TextureId = bt.textureBottom;
	}
	else {
		vs_out.TextureId = bt.textureSide;
	}

	gl_Position = vec4(aPos, 1.0);
}
