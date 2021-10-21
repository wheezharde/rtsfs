/*
 * Copyright (c) 2021 wheezharde
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ___RTSFS_VEC_H___
#define ___RTSFS_VEC_H___

#include <stdint.h>

typedef struct vec2f32_s {
    float x;
    float y;
} vec2f32_s;

inline vec2f32_s vec2f32_zero( void ) {
    return ( vec2f32_s ) { 0, 0 };
}

inline vec2f32_s vec2f32_add( vec2f32_s a, vec2f32_s b ) {
    return ( vec2f32_s ) { a.x + b.x, a.y + b.y };
}

typedef struct vec2u32_s {
    uint32_t x;
    uint32_t y;
} vec2u32_s;

inline vec2u32_s vec2u32_zero( void ) {
    return ( vec2u32_s ) { 0, 0 };
}

inline vec2u32_s vec2u32_add( vec2u32_s a, vec2u32_s b ) {
    return ( vec2u32_s ) { a.x + b.x, a.y + b.y };
}

#endif // ___RTSFS_VEC_H___
