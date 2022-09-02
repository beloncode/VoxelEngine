#ifndef SRC_ASSETS_H
#define SRC_ASSETS_H

#include <string>
#include <unordered_map>

class Texture;
class Shader;

class Assets {
public:
	~Assets();
	Texture* getTexture(const std::string& name);
	void store(Texture* texture, const std::string& name);

	Shader* getShader(const std::string& name);
	void store(Shader* shader, const std::string& name);
private:
	std::unordered_map<std::string, Texture*> m_textures;
	std::unordered_map<std::string, Shader*> m_shaders;
};

#endif /* SRC_ASSETS_H */
