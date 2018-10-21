#pragma once
#include "ResourceManager.h"
#include "Shader.h"

class ShaderManager : public ResourceManager<Shader>
{
public:
	static void initialize();
	static void reloadAll();
	static bool isLightingShader(Shader*);
	static Shader* visualizeTexture();
	static Shader* visualizeMesh();
	static Shader* visualizeCubemap();
	static Shader* unlit();
	static Shader* pbr();
	static Shader* blinnPhong();
	static Shader* phong();
	static Shader* gouraud();
	static Shader* flat();
	static Shader* reflection();
	static Shader* refraction();
	static Shader* equirectangularToCubemap();
	static Shader* debugNormals();
	static Shader* debugNormalsShowLines();
	static Shader* debugTexCoords();
	static Shader* debugDepthBuffer();
	static Shader* shadowMappingUnidirectional();
	static Shader* shadowMappingOmnidirectional();
	static Shader* skybox();
	static Shader* gammaHDR();
	static Shader* passthrough();
	static Shader* grayscale();
	static Shader* chromaticAberration();
	static Shader* invert();
	static Shader* convolution();
};
