/*
 * LineBatch.h
 *
 *  Created on: Jun 25, 2020
 *      Author: MihailRis
 */

#ifndef GRAPHICS_LINEBATCH_H
#define GRAPHICS_LINEBATCH_H

#include <cstdlib>

class Mesh;

class LineBatch {
public:
	explicit LineBatch(size_t capacity);
	~LineBatch();

	void line(float x1, float y1, float z1, float x2, float y2, float z2,
			float r, float g, float b, float a);
	void box(float x, float y, float z, float w, float h, float d,
			float r, float g, float b, float a);
	void render();
private:
    Mesh* m_mesh = nullptr;
    float* m_buffer = nullptr;
    size_t m_index{};
    size_t m_capacity{};
};

#endif /* GRAPHICS_LINEBATCH_H */
