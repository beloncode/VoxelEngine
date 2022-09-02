#ifndef VOXELS_WORLDGENERATOR_H
#define VOXELS_WORLDGENERATOR_H

class voxel;

class WorldGenerator {
public:
	static void generate(voxel* voxels, int x, int y, int z);
};

#endif /* VOXELS_WORLDGENERATOR_H */
