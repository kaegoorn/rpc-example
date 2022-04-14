#pragma once

#include <string>
#include <string_view>

using ByteArray = std::basic_string<unsigned char>;
using ByteArrayView = std::basic_string_view<unsigned char>;
using CommandId = uint32_t;
using SerialNumber = uint32_t;

// simplified, there should be a normal class here
using Error = uint32_t;
static constexpr Error Success = 1;
static constexpr Error Fail = 0;
static constexpr Error Busy = 2;
