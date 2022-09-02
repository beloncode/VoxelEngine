#ifndef VOXELS_CHUNKS_H
#define VOXELS_CHUNKS_H

#include <cstdlib>
#include <glm/glm.hpp>

using namespace glm;

class Mesh;
class VoxelRenderer;

class Chunk;
class voxel;
class WorldFiles;

class Chunks {
public:
	Chunk** chunks;
	Chunk** chunksSecond;
	Mesh** meshes;
	Mesh** meshesSecond;
	size_t volume;
	unsigned int w, h, d;
	int ox, oy, oz;

	Chunks(int w, int h, int d, int ox, int oy, int oz);
	~Chunks();

	bool putChunk(Chunk* chunk) const;

	[[nodiscard]] Chunk* getChunk(int x, int y, int z) const;
	[[nodiscard]] Chunk* getChunkByVoxel(int x, int y, int z) const;
	[[nodiscard]] voxel* get(int x, int y, int z) const;
	[[nodiscard]] unsigned short getLight(int x, int y, int z) const;
	[[nodiscard]] unsigned char getLight(int x, int y, int z, int channel) const;
	void set(int x, int y, int z, int id) const;
	voxel* rayCast(vec3 start, vec3 dir, float maxLength, vec3& end, vec3& norm, vec3& iend) const;

	[[nodiscard]] bool isObstacle(int x, int y, int z) const;

	// does not move m_chunks inside
	void setOffset(int x, int y, int z);

	void setCenter(WorldFiles* worldFiles, int x, int y, int z);
	void translate(WorldFiles* worldFiles, int x, int y, int z);

	void clear(bool freeMemory) const;
};

#endif /* VOXELS_CHUNKS_H */
