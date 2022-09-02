#ifndef SRC_GRAPHICS_BATCH2D_H
#define SRC_GRAPHICS_BATCH2D_H

#include <cstdlib>
#include <memory>

#include <glm/glm.hpp>

class Mesh;

class [[maybe_unused]] Batch2D {
public:
    [[maybe_unused]] explicit Batch2D(size_t capacity);
    ~Batch2D();
    /*
    void rect(float x, float y, float w, float h);
    void render();
    */
private:
	float* m_buffer = nullptr;
	glm::vec4 m_color{};
	Mesh* m_mesh = nullptr;

    /*
    void vertex(float x, float y, float u, float v, float r, float g, float b, float a);
    */

};

#endif /* SRC_GRAPHICS_BATCH2D_H */
