#include "Batch2D.h"
#include "Mesh.h"

[[maybe_unused]] Batch2D::Batch2D(size_t capacity) : /*m_capacity(m_capacity),
													 m_offset(0), */
													 m_color(1.0f, 1.0f, 1.0f, 1.0f){
	static const int attrs[] = {
			2, 2, 4, 0 //null terminator
	};

	m_buffer = new float[capacity];
	m_mesh = new Mesh(nullptr, 0, attrs);
}

Batch2D::~Batch2D(){
	delete m_buffer;
	delete m_mesh;
}
