#ifndef FILES_FILES_H
#define FILES_FILES_H

#include <string>

extern bool WriteBinaryFile(const std::string& filename, const char* data, size_t size);

[[maybe_unused]] extern unsigned int appendBinaryFile(const std::string& filename, const char* data, size_t size);
[[maybe_unused]] extern bool readBinaryFile(const std::string& filename, char* data, size_t size);
extern char* readBinaryFile(const std::string& filename, size_t& length);
[[maybe_unused]] extern unsigned int calcRLE(const char* src, unsigned int length);
extern unsigned int compressRLE(const char* src, unsigned int length, char* dst);
extern unsigned int decompressRLE(const char* src, unsigned int length, char* dst, unsigned int targetLength);

#endif /* FILES_FILES_H */
