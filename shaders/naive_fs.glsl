#version 410 core

// #define GAMMA 2.2

out vec4	FragColor;

in VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
} fs_in;

struct	ColorData {
	bool		isTexture;
	vec3		color;
	sampler2D	texture;
};

struct	Material {
	ColorData	diffuse;
	ColorData	specular;
	float		shininess;
};

struct DirLight {
	vec3		direction;

	vec3		ambient;
	vec3		diffuse;
	vec3		specular;
};

uniform vec3		viewPos;
uniform Material	material;
uniform DirLight	dirLight;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir) {
	vec3	lightDir = normalize(-light.direction);
	// diffuse
	float	diff = max(dot(norm, lightDir), 0.0);
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess * 4);

	// use texture or color for the diffuse
	vec3	ambient = light.ambient;
	vec3	diffuse = light.diffuse;
	if (material.diffuse.isTexture) {
		ambient *= vec3(texture(material.diffuse.texture, fs_in.TexCoords));
		diffuse *= diff * vec3(texture(material.diffuse.texture, fs_in.TexCoords));
	}
	else {
		ambient *= pow(material.diffuse.color, vec3(GAMMA));
		diffuse *= diff * pow(material.diffuse.color, vec3(GAMMA));
	}

	// use texture or color for the specular
	vec3 specular = light.specular;
	if (material.specular.isTexture) {
		specular *= spec * vec3(texture(material.specular.texture, fs_in.TexCoords));
	}
	else {
		specular *= spec * pow(material.specular.color, vec3(GAMMA));
	}

	return (ambient + diffuse + specular);
}

void main() {
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	// Directional lighting
	vec3 result = calcDirLight(dirLight, norm, viewDir);

	FragColor = vec4(result, 1.0);

	// apply gamma correction
    FragColor.rgb = pow(fragColor.rgb, vec3(1.0 / GAMMA));
}
