#include "WorldGenerator.h"
#include "Voxel.h"
#include "Chunk.h"

#include <cmath>
#define FNL_IMPL
#include "maths/FastNoiseLite.h"
#include <ctime>

class PseudoRandom {
	std::uint32_t seed;
public:
	PseudoRandom(){
		seed = (unsigned)time(nullptr);
	}

	auto rand(){
		seed = (8253729 * seed + 2396403);
		return static_cast<std::int32_t>(seed % 32768);
	}

	void setSeed(int number){
		seed = (unsigned)number+8253729;
		rand();
	}
};

static float calcHeight(fnl_state *noise, float real_x, float real_z){
	const float s = 0.2f;
	float height = fnlGetNoise3D(noise, real_x*0.0125f*s*32,real_z*0.0125f*s*32, 0.0f);
	height += fnlGetNoise3D(noise, real_x*0.025f*s*32,real_z*0.025f*s*32, 0.0f)*0.5f;
	height += fnlGetNoise3D(noise, real_x*0.05f*s*32,real_z*0.05f*s*32, 0.0f)*0.25f;
	height += fnlGetNoise3D(noise, real_x*0.1f*s*32,real_z*0.1f*s*32, 0.0f)*0.225f;
	height += fnlGetNoise3D(noise, real_x*0.2f*s*32,real_z*0.2f*s*32, 0.0f)*0.125f;
	height += fnlGetNoise3D(noise, real_x*0.4f*s*32,real_z*0.4f*s*32, 0.0f)*0.125f*0.5F;
	height = height * 0.5f + 0.5f;
	height *= height;
	height *= (140.0f)*0.12f/s;
	height += (42)*0.12f/s;
	return height;
}

static float calcHeightFaster(fnl_state *noise, float real_x, float real_z){
	const float s = 0.2f;
	float height = fnlGetNoise3D(noise, real_x*0.0125f*s*32,real_z*0.0125f*s*32, 0.0f);
	height += fnlGetNoise3D(noise, real_x*0.025f*s*32,real_z*0.025f*s*32, 0.0f)*0.5f;
	height += fnlGetNoise3D(noise, real_x*0.05f*s*32,real_z*0.05f*s*32, 0.0f)*0.25f;
	height += fnlGetNoise3D(noise, real_x*0.1f*s*32,real_z*0.1f*s*32, 0.0f)*0.225f;
	height += fnlGetNoise3D(noise, real_x*0.2f*s*32,real_z*0.2f*s*32, 0.0f)*0.125f;
	//height += fnlGetNoise3D(noise, real_x*0.4f*s*32,real_z*0.4f*s*32, 0.0f)*0.125f*0.5F;
	height = height * 0.5f + 0.5f;
	height *= height;
	height *= (140.0f)*0.12f/s;
	height += (42)*0.12f/s;
	return height;
}

int generateTree(fnl_state *noise, PseudoRandom* random, const float* heights, int real_x, int real_y, int real_z, int tileSize){
	const int tileX = floor((double)real_x/(double)tileSize);
	const int tileY = floor((double)real_z/(double)tileSize);
	random->setSeed(tileX*4325261+tileY*12160951+tileSize*9431111);

	const bool gentree = fnlGetNoise3D(noise,static_cast<float>(tileX)*3.0f+633, 0.0,
									   static_cast<float>(tileY)*3.0f) > -0.1f && (random->rand() % 10) < 7;
	if (!gentree)
		return 0;

	(void)heights;

	const auto randomX = static_cast<std::int32_t>((random->rand() % (tileSize/2)) - tileSize/4);
	const auto randomZ = static_cast<std::int32_t>((random->rand() % (tileSize/2)) - tileSize/4);

	int centerX = tileX * tileSize + tileSize/2 + randomX;
	int centerY = tileY * tileSize + tileSize/2 + randomZ;
	int height = (int) calcHeightFaster(noise, static_cast<float>(centerX), static_cast<float>(centerY));
	if (height < 55)
		return 0;
	int lx = real_x - centerX;
	const std::uint32_t radius = random->rand() % 4 + 3;
	const std::uint32_t ly = real_y - height - 3 * radius;
	const std::uint32_t lz = real_z - centerY;
	if (lx == 0 && lz == 0 && real_y - height < 4*radius)
		return 6;
	if (lx*lx+ly*ly/2+lz*lz < radius*radius)
		return 7;
	return 0;
}

void WorldGenerator::generate(voxel* voxels, int cx, int cy, int cz){
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;

	PseudoRandom random;

	float heights[CHUNK_VOL];

	for (int z = 0; z < CHUNK_D; z++){
		for (int x = 0; x < CHUNK_W; x++){
			int real_x = x + cx * CHUNK_W;
			int real_z = z + cz * CHUNK_D;
			float height = calcHeight(&noise, static_cast<float>(real_x), static_cast<float>(real_z));
			heights[z*CHUNK_W+x] = height;
		}
	}

	for (int z = 0; z < CHUNK_D; z++){
		for (int x = 0; x < CHUNK_W; x++){
			int real_x = x + cx * CHUNK_W;
			int real_z = z + cz * CHUNK_D;
			float height = heights[z*CHUNK_W+x];

			for (int y = 0; y < CHUNK_H; y++){
				int real_y = y + cy * CHUNK_H;
				int id = real_y < 55 ? 9 : 0;
				if (real_y == (int)height)
					id = 2;
				else if (static_cast<float>(real_y) < height){
					if (static_cast<float>(real_y) < height-6)
						id = 8;
					else
						id = 1;
				} else {
					int tree = generateTree(&noise, &random, heights, real_x, real_y, real_z, 16);
					if (tree)
						id = tree;
					else if ((tree = generateTree(&noise, &random, heights, real_x, real_y, real_z, 19))){
						id = tree;
					}else if ((tree = generateTree(&noise, &random, heights, real_x, real_y, real_z, 23))){
						id = tree;
					}
				}

				if (real_y <= 2)
					id = 2;
				voxels[(y * CHUNK_D + z) * CHUNK_W + x].id = id;
			}
		}
	}
}
