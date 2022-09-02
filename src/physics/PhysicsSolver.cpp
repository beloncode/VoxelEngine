#include "PhysicsSolver.h"
#include "Hitbox.h"
#include "voxels/Chunks.h"

#define E 0.03
#define DEFAULT_FRICTION 10.0

PhysicsSolver::PhysicsSolver(vec3 gravity) : m_gravity(gravity) {
}

void PhysicsSolver::step(Chunks* chunks, Hitbox* hitbox, float delta, unsigned substeps, bool shifting, float gravityScale) const {
	hitbox->grounded = false;
	for (unsigned i = 0; i < substeps; i++){
		const float dt = delta / static_cast<float>(substeps);
		const float linear_damping = hitbox->linear_damping;
		vec3& pos = hitbox->position;

		const vec3& half = hitbox->halfsize;

		vec3& vel = hitbox->velocity;
		vel.x += m_gravity.x * dt * gravityScale;
		vel.y += m_gravity.y * dt * gravityScale;
		vel.z += m_gravity.z * dt * gravityScale;

		float px = pos.x;
		float pz = pos.z;

		if (vel.x < 0.0){
			for (std::int32_t y = floor(pos.y-half.y+E); y <= floor(pos.y+half.y-E); y++){
				for (std::int32_t z = floor(pos.z-half.z+E); z <= floor(pos.z+half.z-E); z++){
					const std::int32_t x = floor(pos.x-half.x-E);
					if (chunks->isObstacle(x, y, z)){
						vel.x *= .0f;
						pos.x = static_cast<float>(x) + 1 + half.x + E;
						break;
					}
				}
			}
		}
		if (vel.x > 0.0){
			for (std::int32_t y = floor(pos.y-half.y+E); y <= floor(pos.y+half.y-E); y++){
				for (std::int32_t z = floor(pos.z-half.z+E); z <= floor(pos.z+half.z-E); z++){
					const std::int32_t x = floor(pos.x+half.x+E);
					if (chunks->isObstacle(x,y,z)){
						vel.x *= .0f;
						pos.x = static_cast<float>(x) - half.x - E;
						break;
					}
				}
			}
		}

		if (vel.z < 0.0){
			for (std::int32_t y = floor(pos.y-half.y+E); y <= floor(pos.y+half.y-E); y++){
				for (std::int32_t x = floor(pos.x-half.x+E); x <= floor(pos.x+half.x-E); x++){
					const std::int32_t z = floor(pos.z-half.z-E);
					if (chunks->isObstacle(x,y,z)){
						vel.z *= .0f;
						pos.z = static_cast<float>(z) + 1 + half.z + E;
						break;
					}
				}
			}
		}

		if (vel.z > 0.0){
			for (std::int32_t y = floor(pos.y-half.y+E); y <= floor(pos.y+half.y-E); y++){
				for (std::int32_t x = floor(pos.x-half.x+E); x <= floor(pos.x+half.x-E); x++){
					std::int32_t z = floor(pos.z+half.z+E);
					if (chunks->isObstacle(x, y, z)){
						vel.z *= .0f;
						pos.z = static_cast<float>(z) - half.z - E;
						break;
					}
				}
			}
		}

		if (vel.y < 0.0){
			for (std::int32_t x = floor(pos.x-half.x+E); x <= floor(pos.x+half.x-E); x++){
				bool broken = false;
				for (std::int32_t z = floor(pos.z-half.z+E); z <= floor(pos.z+half.z-E); z++){
					const std::int32_t y = floor(pos.y-half.y-E);
					if (chunks->isObstacle(x, y, z)){
						vel.y *= .0f;
						pos.y = static_cast<float>(y) + 1 + half.y;
						const float f = DEFAULT_FRICTION;
						vel.x *= static_cast<float>(max(0.0, 1.0 - dt * f));
						vel.z *= static_cast<float>(max(0.0, 1.0 - dt * f));
						hitbox->grounded = true;
						broken = true;
						break;
					}
				}
				if (broken)
					break;
			}
		}
		if (vel.y > 0.0){
			for (std::int32_t x = floor(pos.x-half.x+E); x <= floor(pos.x+half.x-E); x++){
				for (std::int32_t z = floor(pos.z-half.z+E); z <= floor(pos.z+half.z-E); z++){
					std::int32_t y = floor(pos.y+half.y+E);
					if (chunks->isObstacle(x, y, z)){
						vel.y *= .0f;
						pos.y = static_cast<float>(y) - half.y - E;
						break;
					}
				}
			}
		}

		vel.x *= static_cast<float>(max(0.0, 1.0 - dt * linear_damping));
		vel.z *= static_cast<float>(max(0.0, 1.0 - dt * linear_damping));

		pos.x += vel.x * dt;
		pos.y += vel.y * dt;
		pos.z += vel.z * dt;

		if (shifting && hitbox->grounded){
			const std::int32_t y = floor(pos.y-half.y-E);

			hitbox->grounded = false;
			for (std::int32_t x = floor(px-half.x+E); x <= floor(px+half.x-E); x++){
				for (std::int32_t z = floor(pos.z-half.z+E); z <= floor(pos.z+half.z-E); z++){
					if (chunks->isObstacle(x, y, z)){
						hitbox->grounded = true;
						break;
					}
				}
			}
			if (!hitbox->grounded) {
				pos.z = pz;
			}
			hitbox->grounded = false;

			for (std::int32_t x = floor(pos.x-half.x+E); x <= floor(pos.x+half.x-E); x++){
				for (std::int32_t z = floor(pz-half.z+E); z <= floor(pz+half.z-E); z++){
					if (chunks->isObstacle(x,y,z)){
						hitbox->grounded = true;
						break;
					}
				}
			}
			if (!hitbox->grounded)
				pos.x = px;

			hitbox->grounded = true;
		}
	}
}

bool PhysicsSolver::isBlockInside(std::int32_t x, std::int32_t y, std::int32_t z, Hitbox* hitbox) {
	const vec3& pos = hitbox->position;
	const vec3& half = hitbox->halfsize;
	return (static_cast<float>(x) >= floor(pos.x-half.x) && static_cast<float>(x) <= floor(pos.x+half.x) &&
		static_cast<float>(z) >= floor(pos.z-half.z) && static_cast<float>(z) <= floor(pos.z+half.z) &&
		static_cast<float>(y) >= floor(pos.y-half.y) && static_cast<float>(y) <= floor(pos.y+half.y));
}

