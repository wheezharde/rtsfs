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

template < typename _type_ >
struct vec2_s {
    _type_ x = 0;
    _type_ y = 0;
};

template < typename _type_ >
vec2_s< _type_ > vec2_zero( void ) {
    return { _type_( 0 ), _type_( 0 ) };
}

template < typename _type_ >
vec2_s< _type_ > operator+( vec2_s< _type_ > a, vec2_s< _type_ > b ) {
    return { a.x + b.x, a.y + b.y };
}

template < typename _type_ >
vec2_s< _type_ > operator-( vec2_s< _type_ > a, vec2_s< _type_ > b ) {
    return { a.x - b.x, a.y - b.y };
}

#endif // ___RTSFS_VEC_H___
