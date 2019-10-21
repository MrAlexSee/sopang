#ifndef ZSTD_HELPER_HPP
#define ZSTD_HELPER_HPP

#include <string>

std::string decompressZstd(const std::string &compressed, size_t bufferSize);

#endif // ZSTD_HELPER_HPP
