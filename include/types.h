#ifndef TYPES_H
#define TYPES_H

#define EMPTY { 0 }

#ifndef ERRVAL
#define ERRVAL -1
#endif // ERRVAL

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;

typedef float     f32;
typedef double    f64;

typedef size_t    usize;
typedef ptrdiff_t isize;

typedef struct Vector2   v2_t;
typedef struct Vector3   v3_t;
typedef struct Vector4   v4_t;
typedef struct Rectangle rec_t;

#endif // TYPES_H
