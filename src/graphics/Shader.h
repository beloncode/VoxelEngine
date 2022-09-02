#ifndef GRAPHICS_SHADER_H
#define GRAPHICS_SHADER_H

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
	unsigned int id;

	explicit Shader(unsigned int id);
	~Shader();

	void use() const;
	void uniformMatrix(const std::string& name, glm::mat4 matrix) const;
	[[maybe_unused]] void uniform1i(const std::string& name, int x) const;
	void uniform1f(const std::string& name, float x) const;
	[[maybe_unused]] void uniform2f(const std::string& name, float x, float y) const;
	void uniform3f(const std::string& name, float x, float y, float z) const;
};

extern Shader* load_shader(const std::string& vertexFile, const std::string& fragmentFile);

#endif /* GRAPHICS_SHADER_H */
