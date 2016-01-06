#pragma once

#pragma once

#include <stdexcept>

// standard throw
#define ThrowIfFailed(hr, msg) \
if ( FAILED(hr) ) \
{ \
  throw std::runtime_error(msg); \
}

#define ThrowNotImplmented() \
throw std::logic_error("Logic Not Implemented");

#include <cstdint>
#include <vector>
#include <string>

typedef unsigned char byte;

extern std::vector<byte> ReadBinaryFile(const std::wstring &fileName);