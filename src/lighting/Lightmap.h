#ifndef LIGHTING_LIGHTMAP_H
#define LIGHTING_LIGHTMAP_H

#include <cstdint>

#include "voxels/Chunk.h"

class Lightmap {
public:
	unsigned short* map;
	Lightmap();
	~Lightmap();

	void set(const Lightmap* lightmap) const;

	[[nodiscard]] inline unsigned short get(std::int32_t x, std::int32_t y, std::int32_t z) const{
		return (map[y*CHUNK_D*CHUNK_W+z*CHUNK_W+x]);
	}

	[[nodiscard]] inline unsigned char get(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t channel) const{
		return (map[y*CHUNK_D*CHUNK_W+z*CHUNK_W+x] >> (channel << 2)) & 0xF;
	}

	[[maybe_unused]] [[nodiscard]] inline unsigned char getR(std::int32_t x, std::int32_t y, std::int32_t z) const{
		return map[y*CHUNK_D*CHUNK_W+z*CHUNK_W+x] & 0xF;
	}

	[[maybe_unused]] [[nodiscard]] inline std::uint8_t getG(std::int32_t x, std::int32_t y, std::int32_t z) const{
		return (map[y*CHUNK_D*CHUNK_W+z*CHUNK_W+x] >> 4) & 0xF;
	}

	[[maybe_unused]] [[nodiscard]] inline std::uint8_t getB(std::int32_t x, std::int32_t y, std::int32_t z) const{
		return (map[y*CHUNK_D*CHUNK_W+z*CHUNK_W+x] >> 8) & 0xF;
	}

	[[nodiscard]] inline std::uint8_t getS(std::int32_t x, std::int32_t y, std::int32_t z) const {
		return (map[y*CHUNK_D*CHUNK_W+z*CHUNK_W+x] >> 12) & 0xF;
	}

	[[maybe_unused]] inline void setR(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t value) const{
		const std::int32_t index = y*CHUNK_D*CHUNK_W+z*CHUNK_W+x;
		map[index] = (map[index] & 0xFFF0) | value;
	}

	[[maybe_unused]] inline void setG(int x, int y, int z, int value) const{
		const int index = y*CHUNK_D*CHUNK_W+z*CHUNK_W+x;
		map[index] = (map[index] & 0xFF0F) | (value << 4);
	}

	[[maybe_unused]] inline void setB(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t value) const{
		const std::int32_t index = y*CHUNK_D*CHUNK_W+z*CHUNK_W+x;
		map[index] = (map[index] & 0xF0FF) | (value << 8);
	}

	[[maybe_unused]] inline void setS(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t value) const{
		const std::int32_t index = y*CHUNK_D*CHUNK_W+z*CHUNK_W+x;
		map[index] = (map[index] & 0x0FFF) | (value << 12);
	}

	inline void set(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t channel, std::int32_t value) const{
		const std::int32_t index = y*CHUNK_D*CHUNK_W+z*CHUNK_W+x;
		map[index] = (map[index] & (0xFFFF & (~(0xF << (channel*4))))) | (value << (channel << 2));
	}
};

#endif /* LIGHTING_LIGHTMAP_H */
