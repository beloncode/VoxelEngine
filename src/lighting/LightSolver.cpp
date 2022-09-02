#include <iostream>
#include <cassert>

#include "LightSolver.h"
#include "Lightmap.h"
#include "voxels/Chunks.h"
#include "voxels/Voxel.h"
#include "voxels/Block.h"

LightSolver::LightSolver(Chunks* chunks, int channel) : m_chunks(chunks), m_channel(channel) {
}

void LightSolver::add(int x, int y, int z, int emission) {
	if (emission <= 1)
		return;
	lightentry entry{};
	entry.x = x;
	entry.y = y;
	entry.z = z;
	entry.light = emission;
	m_addQueue.push(entry);

	Chunk* chunk = m_chunks->getChunkByVoxel(entry.x, entry.y, entry.z);
	chunk->modified = true;
	chunk->lightmap->set(entry.x-chunk->x*CHUNK_W, entry.y-chunk->y*CHUNK_H, entry.z-chunk->z*CHUNK_D, m_channel, entry.light);
}

void LightSolver::add(int x, int y, int z) {
	// assert(m_chunks != nullptr);
	add(x, y, z, m_chunks->getLight(x, y, z, m_channel));
}

void LightSolver::remove(int x, int y, int z) {
	Chunk* chunk = m_chunks->getChunkByVoxel(x, y, z);
	if (chunk == nullptr)
		return;

	int light = chunk->lightmap->get(x-chunk->x*CHUNK_W, y-chunk->y*CHUNK_H, z-chunk->z*CHUNK_D, m_channel);
	if (light == 0){
		return;
	}
	lightentry entry{};
	entry.x = x;
	entry.y = y;
	entry.z = z;
	entry.light = light;
	m_remQueue.push(entry);

	chunk->lightmap->set(entry.x-chunk->x*CHUNK_W, entry.y-chunk->y*CHUNK_H, entry.z-chunk->z*CHUNK_D, m_channel, 0);
}

void LightSolver::solve(){
	constexpr int coords[] = {
            0, 0, 1,
			0, 0,-1,
			0, 1, 0,
			0,-1, 0,
            1, 0, 0,
            -1, 0, 0
	};

	while (!m_remQueue.empty()){
		const lightentry entry = m_remQueue.front();
		m_remQueue.pop();

		for (size_t i = 0; i < 6; i++) {
            int x = entry.x+coords[i*3+0];
			int y = entry.y+coords[i*3+1];
			int z = entry.z+coords[i*3+2];
			Chunk* chunk = m_chunks->getChunkByVoxel(x, y, z);
			if (chunk) {
                int light = m_chunks->getLight(x, y, z, m_channel);
                if (light != 0 && light == entry.light-1){
					lightentry n_entry{};
                    n_entry.x = x;
                    n_entry.y = y;
                    n_entry.z = z;
                    n_entry.light = light;
					m_remQueue.push(n_entry);
					chunk->lightmap->set(x-chunk->x*CHUNK_W, y-chunk->y*CHUNK_H, z-chunk->z*CHUNK_D, m_channel, 0);
					chunk->modified = true;
				}else if (light >= entry.light){
					lightentry n_entry{};
                    n_entry.x = x;
                    n_entry.y = y;
                    n_entry.z = z;
                    n_entry.light = light;
					m_addQueue.push(n_entry);
				}
			}
		}
	}

	while (!m_addQueue.empty()){
		lightentry entry = m_addQueue.front();
		m_addQueue.pop();

		if (entry.light <= 1)
			continue;

		for (size_t i = 0; i < 6; i++) {
			int x = entry.x+coords[i*3+0];
			int y = entry.y+coords[i*3+1];
			int z = entry.z+coords[i*3+2];
			Chunk* chunk = m_chunks->getChunkByVoxel(x, y, z);
			if (chunk) {
				int light = m_chunks->getLight(x, y, z, m_channel);
				voxel* v = m_chunks->get(x, y, z);
				Block* block = Block::blocks[v->id];
				if (block->lightPassing && light+2 <= entry.light){
					chunk->lightmap->set(x-chunk->x*CHUNK_W, y-chunk->y*CHUNK_H, z-chunk->z*CHUNK_D, m_channel, entry.light - 1);
					chunk->modified = true;
					lightentry n_entry{};
                    n_entry.x = x;
                    n_entry.y = y;
                    n_entry.z = z;
                    n_entry.light = entry.light-1;
					m_addQueue.push(n_entry);
				}
			}
		}
	}
}
