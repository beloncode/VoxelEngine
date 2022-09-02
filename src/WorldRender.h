#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <vector>
#include <algorithm>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window/Window.h"
#include "window/Camera.h"
#include "graphics/Mesh.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/LineBatch.h"
#include "voxels/Chunks.h"
#include "voxels/Chunk.h"

extern float g_camera_cx;
extern float g_camera_cz;
extern Chunks* g_chunks;

Mesh *crosshair;

float vertices[] = {
		// x    y
	   -0.01f,-0.01f,
	    0.01f, 0.01f,

	   -0.01f, 0.01f,
	    0.01f,-0.01f,
};

static const std::int32_t attrs[] = {
		2,  0 //null terminator
};

LineBatch *lineBatch;

void allocateRenderer(){
	crosshair = new Mesh(vertices, 4, attrs);
	lineBatch = new LineBatch(4096);
}

void finalizeRenderer(){
	delete crosshair;
	delete lineBatch;
}

void drawChunk(size_t index, Camera* camera, Shader* shader, bool occlusion){
    Chunk* chunk = g_chunks->chunks[index];
	Mesh* mesh = g_chunks->meshes[index];
    assert(mesh != nullptr);
    if (mesh == nullptr)
		return;

	// Simple frustum culling (culling m_chunks behind the camera in 2D - XZ)
	if (occlusion){
		const float cameraX = camera->position.x;
		const float cameraZ = camera->position.z;
		const float camDirX = camera->dir.x;
		const float camDirZ = camera->dir.z;

		bool unoccluded = false;
		do {
			if ((static_cast<float>(chunk->x)*CHUNK_W-cameraX)*camDirX + (static_cast<float>(chunk->z)*CHUNK_D-cameraZ)*camDirZ >= 0.0){
				unoccluded = true; break;
			}
			if ((static_cast<float>(chunk->x+1)*CHUNK_W-cameraX)*camDirX + (static_cast<float>(chunk->z)*CHUNK_D-cameraZ)*camDirZ >= 0.0){
				unoccluded = true; break;
			}
			if (((static_cast<float>(chunk->x+1))*CHUNK_W-cameraX)*camDirX + (static_cast<float>(chunk->z+1)*CHUNK_D-cameraZ)*camDirZ >= 0.0){
				unoccluded = true; break;
			}
			if ((static_cast<float>(chunk->x)*CHUNK_W-cameraX)*camDirX + (static_cast<float>(chunk->z+1)*CHUNK_D-cameraZ)*camDirZ >= 0.0){
				unoccluded = true; break;
			}
		} while (false);
		if (!unoccluded)
			return;
	}

	const mat4 model = glm::translate(mat4(1.0f), vec3(static_cast<float>(chunk->x)*CHUNK_W+0.5f,
                                                 static_cast<float>(chunk->y)*CHUNK_H+0.5f,
                                                 static_cast<float>(chunk->z)*CHUNK_D+0.5f));
	shader->uniformMatrix("u_model", model);
	mesh->draw(GL_TRIANGLES);
}

bool chunksComparator(size_t i, size_t j) {
	Chunk* a = g_chunks->chunks[i];
	Chunk* b = g_chunks->chunks[j];
	return ((static_cast<float>(a->x) + 0.5f - g_camera_cx) * (static_cast<float>(a->x) + 0.5f - g_camera_cx) +
            (static_cast<float>(a->z) + 0.5f - g_camera_cz) * (static_cast<float>(a->z) + 0.5f - g_camera_cz) >
            (static_cast<float>(b->x) + 0.5f - g_camera_cx) * (static_cast<float>(b->x) + 0.5f - g_camera_cx) +
        (static_cast<float>(b->z) + 0.5f - g_camera_cz) * (static_cast<float>(b->z) + 0.5f - g_camera_cz));
}

void drawWorld(Camera* camera, Assets* assets,
               Chunks* chunks, bool occlusion){
	glClearColor(0.7f,0.71f,0.73f,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_chunks = chunks;

	// Draw VAO
	Texture* texture = assets->getTexture("block");
	Shader* shader = assets->getShader("main");
	Shader* crosshairShader = assets->getShader("crosshair");
	Shader* linesShader = assets->getShader("lines");
	shader->use();
	shader->uniformMatrix("u_proj", camera->getProjection());
	shader->uniformMatrix("u_view", camera->getView());
	shader->uniform1f("u_gamma", 1.6f);
	shader->uniform3f("u_skyLightColor", 1.8f,1.8f,1.8f);
	shader->uniform3f("u_fogColor", 0.7f,0.71f,0.73f);
	shader->uniform3f("u_cameraPos", camera->position.x,camera->position.y,camera->position.z);
	texture->bind();

	std::vector<size_t> indices;

	for (size_t i = 0; i < chunks->volume; i++){
		Chunk* chunk = chunks->chunks[i];
		if (chunk == nullptr)
			continue;
		if (chunks->meshes[i] != nullptr)
			indices.push_back(i);
	}

	const float px = camera->position.x / (float)CHUNK_W;
	const float pz = camera->position.z / (float)CHUNK_D;

    g_camera_cx = px;
    g_camera_cz = pz;

    std::sort(indices.begin(), indices.end(), chunksComparator);

	for (unsigned long indice : indices){
        drawChunk(indice, camera, shader, occlusion);
	}

	crosshairShader->use();
	crosshairShader->uniform1f("u_ar", (float)Window::height / (float)Window::width);
	crosshairShader->uniform1f("u_scale", 1.0f / ((float)Window::height / 1000.0f));
	crosshair->draw(GL_LINES);

	linesShader->use();
	linesShader->uniformMatrix("u_projview", camera->getProjection()*camera->getView());
	glLineWidth(2.0f);
	lineBatch->line(camera->position.x, camera->position.y-0.5f, camera->position.z, camera->position.x+0.1f, camera->position.y-0.5f, camera->position.z, 1, 0, 0, 1);
	lineBatch->line(camera->position.x, camera->position.y-0.5f, camera->position.z, camera->position.x, camera->position.y-0.5f, camera->position.z+0.1f, 0, 0, 1, 1);
	lineBatch->render();
}

#endif // WORLD_RENDERER_H
