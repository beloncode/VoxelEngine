#ifndef PHYSICS_PHYSICSSOLVER_H
#define PHYSICS_PHYSICSSOLVER_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Hitbox;
class Chunks;

class PhysicsSolver {
public:
	explicit PhysicsSolver(vec3 gravity);
	void step(Chunks* chunks, Hitbox* hitbox, float delta, unsigned substeps, bool shifting, float gravityScale) const;
	static bool isBlockInside(int x, int y, int z, Hitbox* hitbox);
private:
    vec3 m_gravity;
};

#endif /* PHYSICS_PHYSICSSOLVER_H */
