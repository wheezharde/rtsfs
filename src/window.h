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

#ifndef ___RTSFS_WINDOW_H___
#define ___RTSFS_WINDOW_H___

#include "vec.h"
#include "rect.h"

#include <stddef.h>
#include <stdint.h>

typedef struct rgba_s rgba_s;
typedef struct window_s window_s;

typedef enum windowMsg_e : uint8_t {
    // usage:  called before the window becomes visible
    // msg:    kWindow_OnCreate
    // a:      ( void * )param passed in Window_Create
    // b:      ( void * )user data, if allocated
    // return: nonzero on success, zero on failure
    kWindow_OnCreate,

    // usage:  called before the window is destroyed
    // msg:    kWindow_OnDestroy
    // a:      0
    // b:      ( void * )userData
    // return: ignored
    kWindow_OnDestroy,

    // usage:  called to draw the window to the surface. changes to dimensions are ignored.
    // msg:    kWindow_OnRender
    // a:      windowRenderData_s *
    // b:      ( void * )userData
    // return: ignored
    kWindow_OnRender,

    // usage:  called before a window's size changes. changes to position are ignored. changes to size are respected.
    // msg:    kWindow_OnSize
    // a:      vec2f32_s * position
    // b:      vec2f32_s * size
    // return: nonzero if the change to size should be kept, zero otherwise
    kWindow_OnSize,

    // usage:  called before a window's position changes. changes to position are respected. changes to size are ignored.
    // msg:    kWindow_OnPosition
    // a:      vec2f32_s * position
    // b:      vec2f32_s * size
    // return: nonzero if the change to position should be kept, zero otherwise
    kWindow_OnPosition,

    // usage:  called when a parent window acquires a new child window
    // msg:    kWindow_OnAddChild
    // a:      0
    // b:      window_t * window
    // return: ignored
    kWindow_OnAddChild,
} windowMsg_e;

typedef struct windowRenderData_s {
    rgba_s * surface = nullptr;
    size_t stride = 0;
    rect_s< size_t > clip;
    vec2_s< size_t > position;
    vec2_s< size_t > size;
} windowRenderData_s;

typedef uintptr_t ( * msgHandler_cb )( window_s * const window,
                                       const windowMsg_e msg,
                                       const uintptr_t a,
                                       const uintptr_t b );

window_s * Window_Create( window_s * const parent,
                          msgHandler_cb cb,
                          vec2_s< size_t > position,
                          vec2_s< size_t > size,
                          const size_t userDataSize,
                          void * const param );

void Window_Destroy( window_s * const window );

uintptr_t Window_SendMessage( window_s * const window,
                              const windowMsg_e msg,
                              const uintptr_t a,
                              const uintptr_t b );

void * Window_GetUserData( window_s * const window );

void Window_Render( rgba_s * const surface, size_t stride, rect_s< size_t > clip );

void Window_SetParent( window_s * const parent, window_s * const child );

#endif // ___RTSFS_WINDOW_H___

// TODO:
// postion as world space or local space
// flip HWND render such that y=0 is top