#version 410 core

#define MAX_TEXTURES 12
#define TEXTURE_ATLAS_WIDTH 16
#define TILE_SIZE 0.0625 // 1.0 / TEXTURE_ATLAS_WIDTH;
#define GAMMA 2.2

out vec4	FragColor;

in VS_OUT {
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec2 AtlasOffset;
	flat int TextureId;
} fs_in;

struct	Material {
	vec3		specular;
	float		shininess;
};

struct DirLight {
	vec3		direction;

	vec3		ambient;
	vec3		diffuse;
	vec3		specular;
};

struct Fog {
	bool	enabled;
	int	maxDist;
	int	minDist;
	vec4	color;
};

uniform sampler2D	textureAtlas;

uniform vec3		viewPos;
uniform Material	material;
uniform DirLight	dirLight;
uniform Fog			fog = Fog(
	true,
	256,
	180,
	vec4(0.509, 0.8, 0.905, 1.0)
);

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec2 texCoords) {
	vec3	lightDir = normalize(-light.direction);
	// diffuse
	float	diff = max(dot(norm, lightDir), 0.0);
	// specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess * 4);

	// use texture or color for the diffuse
	vec3	ambient = light.ambient;
	vec3	diffuse = light.diffuse;

	ambient *= vec3(texture(textureAtlas, texCoords));
	diffuse *= diff * vec3(texture(textureAtlas, texCoords));

	// use texture or color for the specular
	vec3 specular = light.specular;
	specular *= spec * pow(material.specular, vec3(GAMMA));

	return (ambient + diffuse + specular);
}

void main() {
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	// calc texture coordinate
	vec2 texCoords = vec2(
		fs_in.AtlasOffset.x + mod(fs_in.TexCoords.x, TILE_SIZE),
		fs_in.AtlasOffset.y + mod(fs_in.TexCoords.y, TILE_SIZE));

	// Directional lighting
	FragColor = vec4(calcDirLight(dirLight, norm, viewDir, texCoords), 1.0);

	// apply gamma correction
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / GAMMA));

	// Calculate fog
	if (fog.enabled) {
		float dist = distance(viewPos, fs_in.FragPos);
		float fog_factor = (fog.maxDist - dist) / (fog.maxDist - fog.minDist);
		fog_factor = clamp(fog_factor, 0.0, 1.0);

		FragColor = mix(fog.color, FragColor, fog_factor);
	}
}
