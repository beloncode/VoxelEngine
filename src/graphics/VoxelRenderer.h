#ifndef GRAPHICS_VOXELRENDERER_H
#define GRAPHICS_VOXELRENDERER_H

#include <cstdlib>

class Mesh;
class Chunk;

class VoxelRenderer {
public:
	explicit VoxelRenderer(size_t capacity);
	~VoxelRenderer();

	Mesh* render(Chunk* chunk, const Chunk** chunks);
private:
    float* m_buffer;
    size_t m_capacity;
};

#endif /* GRAPHICS_VOXELRENDERER_H */
