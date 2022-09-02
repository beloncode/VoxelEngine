
#include "Lightmap.h"

Lightmap::Lightmap(){
	map = new std::uint16_t[CHUNK_VOL];
	for (std::uint32_t index = 0; index < CHUNK_VOL; index++){
		map[index] = 0x0000;
	}
}

Lightmap::~Lightmap(){
	delete[] map;
}

void Lightmap::set(const Lightmap* lightmap) const{
	for (std::uint32_t i = 0; i < CHUNK_VOL; i++){
		map[i] = lightmap->map[i];
	}
}
