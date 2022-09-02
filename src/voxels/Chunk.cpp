#include "Chunk.h"
#include "Voxel.h"
#include "../lighting/Lightmap.h"


Chunk::Chunk(int x_pos, int y_pos, int z_pos) : x(x_pos), y(y_pos), z(z_pos){
	voxels = new voxel[CHUNK_VOL];
	for (unsigned int i = 0; i < CHUNK_VOL; i++)
		voxels[i].id = 1;
	lightmap = new Lightmap();
}

Chunk::~Chunk(){
	delete lightmap;
	delete[] voxels;
}

bool Chunk::isEmpty() const{
	int id = -1;
	for (int i = 0; i < CHUNK_VOL; i++){
		if (voxels[i].id != id){
			if (id != -1)
				return false;
			else
				id = voxels[i].id;
		}
	}
	return true;
}

[[maybe_unused]] Chunk* Chunk::clone() const {
	auto* other = new Chunk(x,y,z);
	for (int i = 0; i < CHUNK_VOL; i++)
		other->voxels[i] = voxels[i];
	other->lightmap->set(lightmap);
	return other;
}

void Chunk::incref(){
	references++;
}

void Chunk::decref(){
	if (--references <= 0)
		delete this;
}
