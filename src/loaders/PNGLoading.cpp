#include "PNGLoading.h"

#include <GL/glew.h>

#include "graphics/Texture.h"

#ifndef _WIN32
#define LIBPNG
#endif

#ifdef LIBPNG
#include <png.h>

#include <iostream>
#include <exception>
#include <cassert>

static GLuint pngLoad(const char* file, int* width, int* height){
	FILE *f;
	int is_png, bit_depth, color_type, row_bytes;
	png_infop info_ptr, end_info;
	png_uint_32 t_width, t_height;
	png_byte header[8], *image_data;
	png_bytepp row_pointers;
	png_structp png_ptr;
	GLuint texture;
	int alpha;

	if (!(f = fopen(file, "r"))){
		return 0;
	}
    const auto fret = fread(header, 1, 8,f);
    if (fret == 1 * 8) {
      std::runtime_error("Can't read a file specified by header variable inside pngLoad routine");
    }
	is_png = png_sig_cmp(header, 0, 8);
	if (is_png){
		fclose(f);
		return 0;
	}
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
		nullptr, nullptr );
	if (!png_ptr){
		fclose(f);
		return 0;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr){
		png_destroy_read_struct(&png_ptr, (png_infopp) nullptr,
			(png_infopp) nullptr );
		fclose(f);
		return 0;
	}
	end_info = png_create_info_struct(png_ptr);
	if ( !end_info ){
		png_destroy_read_struct(&png_ptr, (png_infopp) nullptr,
			(png_infopp) nullptr);
		fclose(f);
		return 0;
	}
	if (setjmp(png_jmpbuf(png_ptr))){
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(f);
		return 0;
	}
	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &t_width, &t_height, &bit_depth,
		&color_type, nullptr, nullptr, nullptr );
	*width = static_cast<std::int32_t>(t_width);
	*height = static_cast<std::int32_t>(t_height);
	png_read_update_info(png_ptr, info_ptr);
	row_bytes = static_cast<std::int32_t>(png_get_rowbytes(png_ptr, info_ptr));
	image_data = (png_bytep)malloc(row_bytes * t_height * sizeof(png_byte));
	assert(image_data != nullptr);
	if (!image_data) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose( f );
		return 0;
	}
	row_pointers = (png_bytepp) malloc(t_height * sizeof(png_bytep));
	if (!row_pointers) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(f);
		return 0;
	}
	for (unsigned int i = 0; i < t_height; ++i ) {
		row_pointers[t_height - 1 - i] = image_data + i * row_bytes;
	}
	png_read_image(png_ptr, row_pointers);
	switch (png_get_color_type(png_ptr, info_ptr)){
		case PNG_COLOR_TYPE_RGBA:
			alpha = GL_RGBA;
			break;
		case PNG_COLOR_TYPE_RGB:
			alpha = GL_RGB;
			break;
		default:
			printf("Color type %d not supported!\n",
				png_get_color_type(png_ptr, info_ptr));
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			return 0;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(t_width), static_cast<GLsizei>(t_height), 0,
		alpha, GL_UNSIGNED_BYTE, static_cast<GLvoid*>(image_data));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(image_data);
	free(row_pointers);
	fclose(f);
	return texture;
}
#else
#include <spng.h>
#include <stdio.h>
#include <inttypes.h>

static GLuint png_load(const char* file, int* pwidth, int* pheight){
	int r = 0;
	FILE *png;
	char *pngbuf = nullptr;
	spng_ctx *ctx = nullptr;
	unsigned char *out = nullptr;

	png = fopen(file, "rb");
	if (png == nullptr){
		std::cerr << "could not to open file " << file << std::endl;
		return 0;
	}

	fseek(png, 0, SEEK_END);
	long siz_pngbuf = ftell(png);
	rewind(png);
	if(siz_pngbuf < 1) {
		std::cerr << "could not to read file " << file << std::endl;
		return 0;
	}
	pngbuf = new char[siz_pngbuf];
	if(fread(pngbuf, siz_pngbuf, 1, png) != 1){
		std::cerr << "fread() failed" << std::endl;
		return 0;
	}
	ctx = spng_ctx_new(0);
	if (ctx == nullptr){
		std::cerr << "spng_ctx_new() failed" << std::endl;
		return 0;
	}
	r = spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
	if (r){
		std::cerr << "spng_set_crc_action() error: " << spng_strerror(r) << std::endl;
		return 0;
	}
	r = spng_set_png_buffer(ctx, pngbuf, siz_pngbuf);
	if (r){
		std::cerr << "spng_set_png_buffer() error: " << spng_strerror(r) << std::endl;
		return 0;
	}

	spng_ihdr ihdr;
	r = spng_get_ihdr(ctx, &ihdr);
	if (r){
		std::cerr << "spng_get_ihdr() error: " << spng_strerror(r) << std::endl;
		return 0;
	}

	char *clr_type_str;
	if(ihdr.color_type == SPNG_COLOR_TYPE_GRAYSCALE)
		clr_type_str = "grayscale";
	else if(ihdr.color_type == SPNG_COLOR_TYPE_TRUECOLOR)
		clr_type_str = "truecolor";
	else if(ihdr.color_type == SPNG_COLOR_TYPE_INDEXED)
		clr_type_str = "indexed color";
	else if(ihdr.color_type == SPNG_COLOR_TYPE_GRAYSCALE_ALPHA)
		clr_type_str = "grayscale with alpha";
	else
		clr_type_str = "truecolor with alpha";

	size_t out_size;
	r = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &out_size);
	if (r){
		std::cerr << "spng_decoded_image_size() error: " << spng_strerror(r) << std::endl;
		return 0;
	}
	out = new unsigned char[out_size];
	r = spng_decode_image(ctx, out, out_size, SPNG_FMT_RGBA8, 0);
	if (r){
		std::cerr << "spng_decode_image() error: " << spng_strerror(r) << std::endl;
		return 0;
	}

	unsigned char* flipped = new unsigned char[out_size];

	for (size_t i = 0; i < ihdr.height; i+=1){
		size_t rowsize = ihdr.width*4;
		for (size_t j = 0; j < rowsize; j++){
			flipped[(ihdr.height-i-1)*rowsize+j] = out[i*rowsize+j];
		}
	}
	delete[] out;

	unsigned int texture;
	int alpha = GL_RGBA;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ihdr.width, ihdr.height, 0,
		alpha, GL_UNSIGNED_BYTE, (GLvoid *) flipped);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	pwidth[0] = ihdr.width;
	pheight[0] = ihdr.height;

	spng_ctx_free(ctx);
	delete[] pngbuf;

	return texture;
}

#endif

Texture* loadTexture(const std::string& filename){
	int width, height;
	GLuint texture = pngLoad(filename.c_str(), &width, &height);
	if (texture == 0){
		std::fprintf(stderr, "Couldn't load texture %s\n", filename.c_str());
		return nullptr;
	}
	return new Texture(texture, width, height);
}
