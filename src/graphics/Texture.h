#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include <string>

class Texture {
public:
	unsigned int id{};
	int width;
	int height;
	Texture(unsigned int id, int width, int height);
	Texture(unsigned char* data, int width, int height);
	~Texture();

	void bind() const;
	[[maybe_unused]] void reload(unsigned char* data) const;
};

extern Texture* loadTexture(const std::string& filename);

#endif /* GRAPHICS_TEXTURE_H */
