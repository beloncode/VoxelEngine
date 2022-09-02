#ifndef LIGHTING_LIGHTSOLVER_H
#define LIGHTING_LIGHTSOLVER_H

#include <queue>
#include <cstdint>

class Chunks;

struct lightentry {
	int x;
	int y;
	int z;
	std::uint8_t light;
};

class LightSolver {
public:
	LightSolver(Chunks* chunks, int channel);

	void add(int x, int y, int z);
	void add(int x, int y, int z, int emission);
	void remove(int x, int y, int z);
	void solve();
private:
    std::queue<lightentry> m_addQueue;
    std::queue<lightentry> m_remQueue;
    Chunks* m_chunks;
    std::int32_t m_channel;
};

#endif /* LIGHTING_LIGHTSOLVER_H */
