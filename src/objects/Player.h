#ifndef SRC_OBJECTS_PLAYER_H
#define SRC_OBJECTS_PLAYER_H

#include <glm/glm.hpp>

class Camera;
class Hitbox;

class Player {
public:
	float speed;
	Camera* camera = nullptr;
	Hitbox* hitbox = nullptr;
	bool flight = false;
	int chBlock{};
	float camX{}, camY{};
	float cameraShaking = 0.0f;
	float cameraShakingTimer = 0.0f;
	glm::vec3 interpVel {0.0f, 0.0f, 0.0f};
	Player(glm::vec3 position, float speed, Camera* camera);
	~Player();
};

#endif /* SRC_OBJECTS_PLAYER_H */
