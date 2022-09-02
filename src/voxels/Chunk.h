#ifndef VOXELS_CHUNK_H
#define VOXELS_CHUNK_H

#include <cstdint>

constexpr std::uint8_t CHUNK_W = 16;
constexpr std::uint16_t CHUNK_H = 256;
constexpr std::uint8_t CHUNK_D = 16;
constexpr std::uint32_t CHUNK_VOL = CHUNK_W * CHUNK_H * CHUNK_D;

class voxel;
class Lightmap;

class Chunk {
public:
    Chunk(int x, int y, int z);
    ~Chunk();

	int x{}, y{}, z{};
	voxel* voxels = nullptr;
	Lightmap* lightmap = nullptr;
	bool modified = true;
	bool ready = false;
	bool loaded = false;
	int surrounding = 0;
	int references = 1;

	[[nodiscard]] bool isEmpty() const;

    [[maybe_unused]] [[nodiscard]] Chunk* clone() const;
	void incref();
	void decref();
};

#endif /* VOXELS_CHUNK_H */
