#pragma once

#include <stdint.h>
#include <limits.h>
#include <limits>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#ifdef _WIN64
typedef u64 uptr;
#else
typedef u32 uptr;
#endif

constexpr u32 U32_INVALID = UINT_MAX;
constexpr u64 U64_INVALID = ULLONG_MAX;
constexpr f32 F32_MIN = std::numeric_limits<f32>::lowest();
constexpr f32 F32_MAX = std::numeric_limits<f32>::max();
constexpr f64 F64_MIN = std::numeric_limits<f64>::lowest();
constexpr f64 F64_MAX = std::numeric_limits<f64>::max();
