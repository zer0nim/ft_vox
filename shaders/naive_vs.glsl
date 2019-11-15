#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec4 pos = vec4(aPos, 1.0);

	vs_out.TexCoords = aTexCoords;
	// vs_out.FragPos = vec3(aPos);
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * model * pos;
}
