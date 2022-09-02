/*
 * Camera.h
 *
 *  Created on: Feb 11, 2020
 *      Author: MihailRis
 */

#ifndef WINDOW_CAMERA_H
#define WINDOW_CAMERA_H

#include <glm/glm.hpp>

using namespace glm;

class Camera {
public:
	vec3 front{};
	vec3 up{};
	vec3 right{};
	vec3 dir{};

	vec3 position;
	float fov;
	float zoom;
	mat4 rotation;
	Camera(vec3 position, float fov);

	void rotate(float x, float y, float z);

	[[nodiscard]] mat4 getProjection() const;
	[[nodiscard]] mat4 getView() const;
private:
    void updateVectors();

};

#endif /* WINDOW_CAMERA_H */
