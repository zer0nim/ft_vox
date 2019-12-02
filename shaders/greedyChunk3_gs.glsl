#version 410 core

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
	flat int TextureId;
} gs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void buildFace(vec4 bottomLeft, vec4 tl, vec4 br) {
	// topLeft
	gl_Position = projection * view * model * (bottomLeft + tl);
	gs_out.TexCoords = vec2(0, 0);
	gs_out.FragPos = vec3(model * (bottomLeft + tl));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	// bottomLeft
	gl_Position = projection * view * model * bottomLeft;
	gs_out.TexCoords = vec2(0, gs_in[0].FSize.y);
	gs_out.FragPos = vec3(model * bottomLeft);
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	// topRight
	gl_Position = projection * view * model * (bottomLeft + tl + br);
	gs_out.TexCoords = vec2(gs_in[0].FSize.x, 0);
	gs_out.FragPos = vec3(model * (bottomLeft + tl + br));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
	EmitVertex();
	// bottomRight
	gl_Position = projection * view * model * (bottomLeft + br);
	gs_out.TexCoords = vec2(gs_in[0].FSize.x, gs_in[0].FSize.y);
	gs_out.FragPos = vec3(model * (bottomLeft + br));
	gs_out.Normal = gs_in[0].Normal;
	gs_out.TextureId = gs_in[0].TextureId;
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
