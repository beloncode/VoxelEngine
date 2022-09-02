#ifndef GRAPHICS_MESH_H
#define GRAPHICS_MESH_H

#include <cstdlib>

class Mesh {
public:
	Mesh(const float* buffer, size_t vertices, const int* attrs);
	~Mesh();

	void reload(const float* buffer, size_t vertices);
	void draw(unsigned int primitive) const;
private:
	unsigned int m_vao{};
	unsigned int m_vbo{};
	size_t m_vertices{};
	size_t m_vertexSize{};

};

#endif /* GRAPHICS_MESH_H */
