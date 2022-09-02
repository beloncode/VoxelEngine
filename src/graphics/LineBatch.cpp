/*
 * LineBatch.cpp
 *
 *  Created on: Jun 25, 2020
 *      Author: MihailRis
 */

#include "LineBatch.h"
#include "Mesh.h"

#include <GL/glew.h>

#define LB_VERTEX_SIZE (3+4)

LineBatch::LineBatch(size_t capacity) : m_capacity(capacity) {
	int attrs[] = {3,4, 0};
    m_buffer = new float[capacity * LB_VERTEX_SIZE * 2];
    m_mesh = new Mesh(m_buffer, 0, attrs);
    m_index = 0;
}

LineBatch::~LineBatch(){
	delete[] m_buffer;
	delete m_mesh;
}

void LineBatch::line(float x1, float y1, float z1, float x2, float y2, float z2,
		float r, float g, float b, float a) {
    m_buffer[m_index] = x1;
    m_buffer[m_index + 1] = y1;
    m_buffer[m_index + 2] = z1;
    m_buffer[m_index + 3] = r;
    m_buffer[m_index + 4] = g;
    m_buffer[m_index + 5] = b;
    m_buffer[m_index + 6] = a;
    m_index += LB_VERTEX_SIZE;

    m_buffer[m_index] = x2;
    m_buffer[m_index + 1] = y2;
    m_buffer[m_index + 2] = z2;
    m_buffer[m_index + 3] = r;
    m_buffer[m_index + 4] = g;
    m_buffer[m_index + 5] = b;
    m_buffer[m_index + 6] = a;
    m_index += LB_VERTEX_SIZE;
}

void LineBatch::box(float x, float y, float z, float w, float h, float d,
		float r, float g, float b, float a) {
	w *= 0.5f;
	h *= 0.5f;
	d *= 0.5f;

	line(x-w, y-h, z-d, x+w, y-h, z-d, r,g,b,a);
	line(x-w, y+h, z-d, x+w, y+h, z-d, r,g,b,a);
	line(x-w, y-h, z+d, x+w, y-h, z+d, r,g,b,a);
	line(x-w, y+h, z+d, x+w, y+h, z+d, r,g,b,a);

	line(x-w, y-h, z-d, x-w, y+h, z-d, r,g,b,a);
	line(x+w, y-h, z-d, x+w, y+h, z-d, r,g,b,a);
	line(x-w, y-h, z+d, x-w, y+h, z+d, r,g,b,a);
	line(x+w, y-h, z+d, x+w, y+h, z+d, r,g,b,a);

	line(x-w, y-h, z-d, x-w, y-h, z+d, r,g,b,a);
	line(x+w, y-h, z-d, x+w, y-h, z+d, r,g,b,a);
	line(x-w, y+h, z-d, x-w, y+h, z+d, r,g,b,a);
	line(x+w, y+h, z-d, x+w, y+h, z+d, r,g,b,a);
}

void LineBatch::render(){
	if (m_index == 0)
		return;
	m_mesh->reload(m_buffer, m_index / LB_VERTEX_SIZE);
	m_mesh->draw(GL_LINES);
    m_index = 0;
}
