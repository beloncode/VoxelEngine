#include "Lighting.h"
#include "LightSolver.h"
#include "Lightmap.h"
#include "voxels/Chunks.h"
#include "voxels/Voxel.h"
#include "voxels/Block.h"

Lighting::Lighting(Chunks* chunks){
	this->chunks = chunks;
    /*
	solverR = new LightSolver(m_chunks, 0);
	solverG = new LightSolver(m_chunks, 1);
	solverB = new LightSolver(m_chunks, 2);
	solverS = new LightSolver(m_chunks, 3);
    */
    solverR = std::make_shared<LightSolver>(chunks, 0);
    solverG = std::make_shared<LightSolver>(chunks, 1);
    solverB = std::make_shared<LightSolver>(chunks, 2);
    solverS = std::make_shared<LightSolver>(chunks, 3);
}
/*
Lighting::~Lighting(){

	delete solverR;
	delete solverG;
	delete solverB;
	delete solverS;
}
 */

[[maybe_unused]] void Lighting::clear(){
	for (std::uint32_t index = 0; index < chunks->volume; index++){
		Chunk* chunk = chunks->chunks[index];
		if (chunk == nullptr)
			continue;
		Lightmap* lightmap = chunk->lightmap;
		for (std::int32_t i = 0; i < CHUNK_VOL; i++){
			lightmap->map[i] = 0;
		}
	}
}

void Lighting::onChunkLoaded(std::int32_t cx, std::int32_t cy, std::int32_t cz, bool sky){
	Chunk* chunk = chunks->getChunk(cx, cy, cz);
	Chunk* chunkUpper = chunks->getChunk(cx, cy+1, cz);
	Chunk* chunkLower = chunks->getChunk(cx, cy-1, cz);
	if (chunkLower && sky){
		for (std::int32_t z = 0; z < CHUNK_D; z++){
			for (std::int32_t x = 0; x < CHUNK_W; x++){
				const std::int32_t gx = x + cx * CHUNK_W;
				const std::int32_t gz = z + cz * CHUNK_D;

				const std::int32_t light = chunk->lightmap->getS(x,0,z);
                std::int32_t ncy = cy-1;
				if (light < 15){
					Chunk* current = chunkLower;
					if (chunkLower->lightmap->getS(x,15,z) == 0)
						continue;
					for (int y = 15;;y--){
						if (y < 0){
							ncy--;
							y += CHUNK_H;
						}
						if (ncy != current->y)
							current = chunks->getChunk(cx,ncy,cz);
						if (!current)
							break;
						voxel* vox = &(current->voxels[(y * CHUNK_D + z) * CHUNK_W + x]);//m_chunks->get(gx,gy+y,gz);
						Block* block = Block::blocks[vox->id];
						if (!block->skyLightPassing)
							break;
						//current->lightmap->setS(x,y,z, 0);
						current->modified = true;
						solverS->remove(gx,y+ncy*CHUNK_H,gz);
						current->lightmap->setS(x,y,z, 0);
					}
				}
			}
		}
	}
	if (chunkUpper && sky){
		for (std::int32_t z = 0; z < CHUNK_D; z++){
			for (std::int32_t x = 0; x < CHUNK_W; x++){
				const std::int32_t gx = x + cx * CHUNK_W;
				const std::int32_t gy = cy * CHUNK_H;
				const std::int32_t gz = z + cz * CHUNK_D;
                std::int32_t ncy = cy;

				const std::int32_t light = chunkUpper->lightmap->getS(x,0,z);

				Chunk* current = chunk;
				if (light == 15){
					for (int y = CHUNK_H-1;;y--){
						if (y < 0){
							ncy--;
							y += CHUNK_H;
						}
						if (ncy != current->y)
							current = chunks->getChunk(cx,ncy,cz);
						if (!current)
							break;
						const voxel* vox = &(current->voxels[(y * CHUNK_D + z) * CHUNK_W + x]);//m_chunks->get(gx,gy+y,gz);
						Block* block = Block::blocks[vox->id];
						if (!block->skyLightPassing)
							break;
						current->lightmap->setS(x,y,z, 15);
						current->modified = true;
						solverS->add(gx,y+ncy*CHUNK_H,gz);
					}
				} else if (light){
					solverS->add(gx,gy+CHUNK_H,gz);
				}
			}
		}
	} else if (sky){
		for (std::int32_t z = 0; z < CHUNK_D; z++){
			for (std::int32_t x = 0; x < CHUNK_W; x++){
				const std::int32_t gx = x + cx * CHUNK_W;
				const std::int32_t gz = z + cz * CHUNK_D;
				std::int32_t ncy = cy;

				Chunk* current = chunk;
				for (std::int32_t y = CHUNK_H-1;;y--){
					if (y < 0){
						ncy--;
						y += CHUNK_H;
					}
					if (ncy != current->y)
						current = chunks->getChunk(cx,ncy,cz);
					if (!current)
						break;
					voxel* vox = &(current->voxels[(y * CHUNK_D + z) * CHUNK_W + x]);//m_chunks->get(gx,gy+y,gz);
					Block* block = Block::blocks[vox->id];
					if (!block->skyLightPassing)
						break;
					current->lightmap->setS(x,y,z, 15);
					current->modified = true;
					solverS->add(gx,y+ncy*CHUNK_H,gz);
				}
			}
		}
	}
	//std::cout << "DONE" << std::endl;
	for (std::int32_t y = 0; y < CHUNK_H; y++){
		for (std::int32_t z = 0; z < CHUNK_D; z++){
			for (std::int32_t x = 0; x < CHUNK_W; x++){
				voxel vox = chunk->voxels[(y * CHUNK_D + z) * CHUNK_W + x];
				Block* block = Block::blocks[vox.id];
				if (block->emission[0] || block->emission[1] || block->emission[2]){
					const std::int32_t gx = x + cx * CHUNK_W;
					const std::int32_t gy = y + cy * CHUNK_H;
					const std::int32_t gz = z + cz * CHUNK_D;
					solverR->add(gx,gy,gz,block->emission[0]);
					solverG->add(gx,gy,gz,block->emission[1]);
					solverB->add(gx,gy,gz,block->emission[2]);
				}
			}
		}
	}
	for (std::int32_t y = -1; y <= CHUNK_H; y++){
		for (std::int32_t z = -1; z <= CHUNK_D; z++){
			for (std::int32_t x = -1; x <= CHUNK_W; x++){
				if (!(x == -1 || x == CHUNK_W || y == -1 || y == CHUNK_H || z == -1 || z == CHUNK_D))
					continue;
				const std::int32_t gx = x + cx * CHUNK_W;
				const std::int32_t gy = y + cy * CHUNK_H;
				const std::int32_t gz = z + cz * CHUNK_D;
				if (chunks->getLight(x,y,z)){
					solverR->add(gx,gy,gz);
					solverG->add(gx,gy,gz);
					solverB->add(gx,gy,gz);
					if (sky)
						solverS->add(gx,gy,gz);
				}
			}
		}
	}

	solverR->solve();
	solverG->solve();
	solverB->solve();
	solverS->solve();

	/* Chunk* other; */

    static const std::int32_t updateIndexes[][3] ={
            {-1,+0,+0},
            {+1,+0,+0},
            {+0,-1,+0},
            {+0,+1,+0},
            {+0,+0,-1},
            {+0,+0,+1}
    };

    for (auto const updateIndex : updateIndexes){
        const auto other = chunks->getChunk(cx + updateIndex[0], cy + updateIndex[1], updateIndex[2]);
        if (other != nullptr){
            other->modified = true;
        }

    }
    /*
	other = m_chunks->getChunk(cx-1,cy,cz); if (other) other->modified = true;
	other = m_chunks->getChunk(cx+1,cy,cz); if (other) other->modified = true;
	other = m_chunks->getChunk(cx,cy-1,cz); if (other) other->modified = true;
	other = m_chunks->getChunk(cx,cy+1,cz); if (other) other->modified = true;
	other = m_chunks->getChunk(cx,cy,cz-1); if (other) other->modified = true;
	other = m_chunks->getChunk(cx,cy,cz+1); if (other) other->modified = true;
     */
}

void Lighting::onBlockSet(int x, int y, int z, int id){
	Block* block = Block::blocks[id];
	if (id == 0){
		solverR->remove(x,y,z);
		solverG->remove(x,y,z);
		solverB->remove(x,y,z);

		solverR->solve();
		solverG->solve();
		solverB->solve();

		if (chunks->getLight(x,y+1,z, 3) == 0xF){
			for (int i = y; i >= 0; i--){
				voxel* vox = chunks->get(x,i,z);
				if ((vox == nullptr || vox->id != 0) && Block::blocks[id]->skyLightPassing)
					break;
				solverS->add(x,i,z, 0xF);
			}
		}

		solverR->add(x,y+1,z); solverG->add(x,y+1,z); solverB->add(x,y+1,z); solverS->add(x,y+1,z);
		solverR->add(x,y-1,z); solverG->add(x,y-1,z); solverB->add(x,y-1,z); solverS->add(x,y-1,z);
		solverR->add(x+1,y,z); solverG->add(x+1,y,z); solverB->add(x+1,y,z); solverS->add(x+1,y,z);
		solverR->add(x-1,y,z); solverG->add(x-1,y,z); solverB->add(x-1,y,z); solverS->add(x-1,y,z);
		solverR->add(x,y,z+1); solverG->add(x,y,z+1); solverB->add(x,y,z+1); solverS->add(x,y,z+1);
		solverR->add(x,y,z-1); solverG->add(x,y,z-1); solverB->add(x,y,z-1); solverS->add(x,y,z-1);

		solverR->solve();
		solverG->solve();
		solverB->solve();
		solverS->solve();
	} else {
		solverR->remove(x,y,z);
		solverG->remove(x,y,z);
		solverB->remove(x,y,z);
		if (!block->skyLightPassing){
			solverS->remove(x,y,z);
			for (int i = y-1; i >= 0; i--){
				solverS->remove(x,i,z);
				if (i == 0 || chunks->get(x,i-1,z)->id != 0){
					break;
				}
			}
			solverS->solve();
		}
		solverR->solve();
		solverG->solve();
		solverB->solve();

		if (block->emission[0] || block->emission[1] || block->emission[2]){
			solverR->add(x,y,z,block->emission[0]);
			solverG->add(x,y,z,block->emission[1]);
			solverB->add(x,y,z,block->emission[2]);
			solverR->solve();
			solverG->solve();
			solverB->solve();
		}
	}
}
