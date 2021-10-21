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

#ifndef ___RTSFS_RECT_H___
#define ___RTSFS_RECT_H___

#include "vec.h"

template < typename _type_ >
struct rect_s {
    vec2_s< _type_ > mn{ 0, 0 };
    vec2_s< _type_ > mx{ 0, 0 };
};

template < typename _type_ >
rect_s< _type_ > rectFrom( vec2_s< _type_ > position, vec2_s< _type_ > size ) {
    return rect_s< _type_ > {
        position,
        position + size - vec2_s< _type_ >{ 1, 1 }
    };
}

#endif // ___RTSFS_RECT_H___