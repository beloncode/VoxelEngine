#include "Assets.h"

#include "graphics/Texture.h"
#include "graphics/Shader.h"

Assets::~Assets() {
	for (auto& iter : m_shaders){
		delete iter.second;
	}

	for (auto& iter : m_textures){
		delete iter.second;
	}
}

Texture* Assets::getTexture(const std::string& name){
	return m_textures[name];
}

void Assets::store(Texture* texture, const std::string& name){
	m_textures[name] = texture;
}


Shader* Assets::getShader(const std::string& name){
	return m_shaders[name];
}

void Assets::store(Shader* shader, const std::string& name){
	m_shaders[name] = shader;
}
