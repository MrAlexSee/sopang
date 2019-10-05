#ifndef ZSTD_HELPER_HPP
#define ZSTD_HELPER_HPP

#include <string>

namespace sopang
{

std::string decompressZstd(const std::string &compressed, size_t bufferSize);

} // namespace sopang

#endif // ZSTD_HELPER_HPP
