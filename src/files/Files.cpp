#include "Files.h"

#include <fstream>

[[maybe_unused]] bool WriteBinaryFilePart(const std::string& filename, const char* data, size_t offset, size_t size){
	std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::in);
	if (!output.is_open())
		return false;
	output.seekp(static_cast<std::streamoff>(offset));
	output.write(data, static_cast<std::streamsize>(size));
	return true;
}

bool WriteBinaryFile(const std::string& filename, const char* data, size_t size) {
	std::ofstream output(filename, std::ios::binary);
	if (!output.is_open())
		return false;
	output.write(data, static_cast<std::streamsize>(size));
	output.close();
	return true;
}

[[maybe_unused]] [[maybe_unused]] unsigned int appendBinaryFile(const std::string& filename, const char* data, size_t size) {
	std::ofstream output(filename, std::ios::binary | std::ios::app);
	if (!output.is_open())
		return 0;
	unsigned int position = output.tellp();
	output.write(data, static_cast<std::streamsize>(size));
	output.close();
	return position;
}

[[maybe_unused]] [[maybe_unused]] bool readBinaryFile(const std::string& filename, char* data, size_t size) {
	std::ifstream output(filename, std::ios::binary);
	if (!output.is_open())
		return false;
	output.read(data, static_cast<std::streamsize>(size));
	output.close();
	return true;
}

char* readBinaryFile(const std::string& filename, size_t& length) {
	std::ifstream input(filename, std::ios::binary);
	if (!input.is_open())
		return nullptr;
	input.seekg(0, std::ios_base::end);
	length = input.tellg();
	input.seekg(0, std::ios_base::beg);

	char* data = new char[length];
	input.read(data, static_cast<std::streamsize>(length));
	input.close();
	return data;
}

// returns decompressed length
unsigned int decompressRLE(const char* src, unsigned int length, char* dst, unsigned int targetLength){
	(void)targetLength;
	unsigned int offset = 0;
	for (unsigned int i = 0; i < length;){
		unsigned char counter = src[i++];
		char c = src[i++];
		for (unsigned int j = 0; j <= counter; j++){
			dst[offset++] = c;
		}
	}
	return offset;
}

[[maybe_unused]] unsigned int calcRLE(const char* src, unsigned int length) {
	unsigned int offset = 0;
	unsigned int counter = 1;
	char c = src[0];
	for (unsigned int i = 0; i < length; i++){
		const char C_next = src[i];
		if (C_next != c || counter == 256){
			offset += 2;
			c = C_next;
			counter = 0;
		}
		counter++;
	}
	return offset + 2;
}

// max result size = length * 2; returns compressed length
unsigned int compressRLE(const char* src, unsigned int length, char* dst) {
	std::uint32_t offset = 0;
	std::uint32_t counter = 1;
	char c = src[0];
	for (unsigned int i = 1; i < length; i++){
		const char C_next = src[i];
		if (C_next != c || counter == 256){
			dst[offset++] = static_cast<char>(counter-1);
			dst[offset++] = c;
			c = C_next;
			counter = 0;
		}
		counter++;
	}
	dst[offset++] = static_cast<char>(counter-1);
	dst[offset++] = c;
	return offset;
}
