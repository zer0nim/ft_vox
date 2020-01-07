#version 410 core

#define TEXTURE_ATLAS_WIDTH 16
#define TILE_SIZE 0.0625 // 1.0 / TEXTURE_ATLAS_WIDTH;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in GS_IN {
	flat int FaceId;
	vec2 FSize;
	vec3 Normal;
	flat int TextureId;
} gs_in[];

out VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec2 AtlasOffset;
	flat int TextureId;
} gs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void buildFace(vec4 bottomLeft, vec4 tl, vec4 br) {
	// calculate texture atlas offset
	ivec2 offset = ivec2(
		mod(gs_in[0].TextureId, TEXTURE_ATLAS_WIDTH), \
			gs_in[0].TextureId / TEXTURE_ATLAS_WIDTH);
	vec2 atlasOffset = vec2(offset.x * TILE_SIZE, offset.y * TILE_SIZE);

	// topLeft
	gl_Position = projection * view * model * (bottomLeft + tl);
	gs_out.TexCoords = vec2(0, 0);
	gs_out.FragPos = vec3(model * (bottomLeft + tl));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	gs_out.AtlasOffset = atlasOffset;
	EmitVertex();
	// bottomLeft
	gl_Position = projection * view * model * bottomLeft;
	gs_out.TexCoords = vec2(0, gs_in[0].FSize.y * TILE_SIZE);
	gs_out.FragPos = vec3(model * bottomLeft);
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	gs_out.AtlasOffset = atlasOffset;
	EmitVertex();
	// topRight
	gl_Position = projection * view * model * (bottomLeft + tl + br);
	gs_out.TexCoords = vec2(gs_in[0].FSize.x * TILE_SIZE, 0);
	gs_out.FragPos = vec3(model * (bottomLeft + tl + br));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	gs_out.AtlasOffset = atlasOffset;
	EmitVertex();
	// bottomRight
	gl_Position = projection * view * model * (bottomLeft + br);
	gs_out.TexCoords = vec2(gs_in[0].FSize.x * TILE_SIZE, gs_in[0].FSize.y * TILE_SIZE);
	gs_out.FragPos = vec3(model * (bottomLeft + br));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	gs_out.AtlasOffset = atlasOffset;
	EmitVertex();
	EndPrimitive();
}

void main() {
	vec3 tl[6] = vec3[6](
		vec3(0, gs_in[0].FSize.y, 0),
		vec3(0, gs_in[0].FSize.y, 0),
		vec3(0, gs_in[0].FSize.y, 0),
		vec3(0, gs_in[0].FSize.y, 0),
		vec3(0, 0, -gs_in[0].FSize.y),
		vec3(0, 0, gs_in[0].FSize.y)
	);

	vec3 br[6] = vec3[6](
		vec3(gs_in[0].FSize.x, 0, 0),
		vec3(0, 0, -gs_in[0].FSize.x),
		vec3(-gs_in[0].FSize.x, 0, 0),
		vec3(0, 0, gs_in[0].FSize.x),
		vec3(gs_in[0].FSize.x, 0, 0),
		vec3(gs_in[0].FSize.x, 0, 0)
	);

	buildFace(gl_in[0].gl_Position, vec4(tl[gs_in[0].FaceId], 0), vec4(br[gs_in[0].FaceId], 0));
}
