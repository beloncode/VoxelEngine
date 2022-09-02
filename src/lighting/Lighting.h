#ifndef LIGHTING_LIGHTING_H
#define LIGHTING_LIGHTING_H

#include <cstdint>
#include <memory>

class Chunks;
class LightSolver;

class Lighting {
	Chunks* chunks = nullptr;
	std::shared_ptr<LightSolver> solverR;
	std::shared_ptr<LightSolver> solverG;
	std::shared_ptr<LightSolver> solverB;
	std::shared_ptr<LightSolver> solverS;
	/*
	LightSolver* solverR = nullptr;
	LightSolver* solverG = nullptr;
	LightSolver* solverB = nullptr;
	LightSolver* solverS = nullptr;
	*/
public:
	explicit Lighting(Chunks* chunks);
	~Lighting() = default;

	[[maybe_unused]] void clear();
	void onChunkLoaded(std::int32_t cx, std::int32_t cy, std::int32_t cz, bool sky);
	void onBlockSet(std::int32_t x, std::int32_t y, int z, std::int32_t id);
};

#endif /* LIGHTING_LIGHTING_H */
