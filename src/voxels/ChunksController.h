#ifndef VOXELS_CHUNKSCONTROLLER_H
#define VOXELS_CHUNKSCONTROLLER_H

class Chunks;
class Lighting;
class WorldFiles;
class VoxelRenderer;
class ChunksLoader;

#include <cstdint>

class ChunksController {
public:
	ChunksController(Chunks* chunks, Lighting* lighting);
	~ChunksController();

	int countFreeLoaders();
	bool loadVisible(WorldFiles* worldFiles);
	bool buildMeshes(VoxelRenderer* renderer, int tick);
private:
	Chunks* m_chunks;
	ChunksLoader** m_loaders;
	std::uint32_t m_loadersCount;
};

#endif /* VOXELS_CHUNKSCONTROLLER_H */
