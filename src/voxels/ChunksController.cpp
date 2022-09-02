#include "ChunksController.h"
#include "Chunk.h"
#include "Chunks.h"
#include "graphics/Mesh.h"
#include "graphics/VoxelRenderer.h"
#include "lighting/Lighting.h"
#include "files/WorldFiles.h"
#include "ChunksLoader.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#include <mingw.thread.h>
#else
#include <thread>
#endif

#define MIN_SURROUNDING 9

ChunksController::ChunksController(Chunks* chunks, Lighting* lighting) : m_chunks(chunks){
	(void)lighting;
	m_loadersCount = std::thread::hardware_concurrency() - 1;
	if (m_loadersCount <= 0)
		m_loadersCount = 1;
	m_loaders = new ChunksLoader*[m_loadersCount];
	for (int i = 0; i < m_loadersCount; i++){
		m_loaders[i] = new ChunksLoader();
	}
	std::printf("Created {%d} loaders\n", m_loadersCount);
}

ChunksController::~ChunksController(){
	for (int i = 0; i < m_loadersCount; i++)
		delete m_loaders[i];
	delete[] m_loaders;
}

int ChunksController::countFreeLoaders(){
	int count = 0;
	for (int i = 0; i < m_loadersCount; i++){
		if (!m_loaders[i]->isBusy())
			count++;
	}
	return count;
}

bool ChunksController::loadVisible(WorldFiles* worldFiles){
	const auto w = m_chunks->w;
	const auto h = m_chunks->h;
	const auto d = m_chunks->d;
	const int ox = m_chunks->ox;
	const int oy = m_chunks->oy;
	const int oz = m_chunks->oz;
	int nearX = 0;
	int nearY = 0;
	int nearZ = 0;
	auto minDistance = (w/2)*(w/2);
	for (int y = 0; y < h; y++){
		for (int z = 2; z < d-2; z++){
			for (int x = 2; x < w-2; x++){
				auto index = (y * d + z) * w + x;
				Chunk* chunk = m_chunks->chunks[index];
				if (chunk != nullptr){
					int surrounding = 0;
					for (int i = -1; i <= 1; i++){
						for (int k = -1; k <= 1; k++){
							Chunk* other = m_chunks->getChunk(chunk->x + k, chunk->y, chunk->z + i);
							if (other != nullptr && other->ready) surrounding++;
						}
					}
					chunk->surrounding = surrounding;
					continue;
				}
				auto lx = x - w / 2;
				auto ly = y - h / 2;
				auto lz = z - d / 2;
				auto distance = (lx * lx + ly * ly + lz * lz);
				if (distance < minDistance){
					minDistance = distance;
					nearX = x;
					nearY = y;
					nearZ = z;
				}
			}
		}
	}

	auto index = (nearY * d + nearZ) * w + nearX;
	Chunk* chunk = m_chunks->chunks[index];
	if (chunk != nullptr)
		return false;

	ChunksLoader* freeLoader = nullptr;
	for (int i = 0; i < m_loadersCount; i++){
		ChunksLoader* loader = m_loaders[i];
		if (loader->isBusy()){
			continue;
		}
		freeLoader = loader;
		break;
	}
	if (freeLoader == nullptr)
		return false;
	chunk = new Chunk(nearX+ox,nearY+oy,nearZ+oz);
	if (worldFiles->getChunk(chunk->x, chunk->z, (char*)chunk->voxels))
		chunk->loaded = true;

	m_chunks->chunks[index] = chunk;

	Chunk* closes[27];
	for (auto & close : closes)
		close = nullptr;
	for (size_t j = 0; j < m_chunks->volume; j++){
		Chunk* other = m_chunks->chunks[j];
		if (other == nullptr)
			continue;
		if (!other->ready)
			continue;

		int ox_l = other->x - chunk->x;
		int oy_l = other->y - chunk->y;
		int oz_l = other->z - chunk->z;

		if (abs(ox_l) > 1 || abs(oy_l) > 1 || abs(oz_l) > 1)
			continue;

		ox_l += 1;
		oy_l += 1;
		oz_l += 1;
		closes[(oy_l * 3 + oz_l) * 3 + ox_l] = other;
	}
	freeLoader->perform(chunk, (Chunk**)closes);
	return true;
}

bool ChunksController::buildMeshes(VoxelRenderer* renderer, int tick) {
	(void)tick;
	const auto w = m_chunks->w;
	const auto h = m_chunks->h;
	const auto d = m_chunks->d;

	int nearX = 0;
	int nearY = 0;
	int nearZ = 0;
	int minDistance = 1000000000;
	for (int y = 0; y < h; y++){
		for (int z = 1; z < d-1; z++){
			for (int x = 1; x < w-1; x++){
				auto index = (y * d + z) * w + x;
				Chunk* chunk = m_chunks->chunks[index];
				if (chunk == nullptr)
					continue;
				Mesh* mesh = m_chunks->meshes[index];
				if (mesh != nullptr && !chunk->modified)
					continue;
				if (!chunk->ready || chunk->surrounding < MIN_SURROUNDING){
					continue;
				}
				auto lx = x - w / 2;
				auto ly = y - h / 2;
				auto lz = z - d / 2;
				auto distance = (lx * lx + ly * ly + lz * lz);
				if (distance < minDistance){
					minDistance = static_cast<std::int32_t>(distance);
					nearX = x;
					nearY = y;
					nearZ = z;
				}
			}
		}
	}

	const auto index = (nearY * d + nearZ) * w + nearX;


	Chunk* chunk = m_chunks->chunks[index];
	if (chunk == nullptr){
		return false;
	}
	Mesh* mesh = m_chunks->meshes[index];
	if (mesh == nullptr || chunk->modified){
		Chunk* closes[27];
		/*
		Deleting a null pointer causes nothing (no effect)
		if (mesh != nullptr)
			delete mesh;
		 */
		delete mesh;
		if (chunk->isEmpty()){
			m_chunks->meshes[index] = nullptr;
			return false;
		}
		chunk->modified = false;
		for (auto & close : closes)
			close = nullptr;
		for (size_t j = 0; j < m_chunks->volume; j++){
			Chunk* other = m_chunks->chunks[j];
			if (other == nullptr)
				continue;
			if (!other->ready)
				continue;

			int ox = other->x - chunk->x;
			int oy = other->y - chunk->y;
			int oz = other->z - chunk->z;

			if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1)
				continue;

			ox += 1;
			oy += 1;
			oz += 1;
			closes[(oy * 3 + oz) * 3 + ox] = other;
		}
		mesh = renderer->render(chunk, (const Chunk**)closes);
		m_chunks->meshes[index] = mesh;
		return true;
	}
	return false;
}
