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

 #include "window.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include <new>

typedef struct window_s {
    msgHandler_cb cb = nullptr;
    size_t userDataSize = 0;
    vec2_s< size_t > position;
    vec2_s< size_t > size;
    window_s ** child = nullptr;
    size_t childCount = 0;
    size_t childSize = 0;
} window_s;

static window_s * root = 0;

static void Window_RenderRecurse( window_s * const window, rgba_s * const surface, size_t stride, rect_s< size_t > clip ) {
    windowRenderData_s renderData = {
        surface,
        stride,
        clip,
        window->position,
        window->size
    };

    void * const userData = window->userDataSize ? window + 1 : 0;

    if ( window->cb != nullptr ) {
        window->cb( window, kWindow_OnRender, ( uintptr_t )&renderData, ( uintptr_t )userData );
    }

    rect_s< size_t > windowClip = rectFrom( window->position, window->size );

    for ( size_t i = 0; i < window->childCount; i++ ) {
        Window_RenderRecurse( window->child[ i ], surface, stride, windowClip );
    }
}

window_s * Window_Create( window_s * const parent, msgHandler_cb cb, vec2_s< size_t > position, vec2_s< size_t > size, size_t userDataSize, void * const param ) {
    const size_t mallocSize = sizeof( window_s ) + userDataSize;

    window_s * const window = ( window_s * )malloc( mallocSize );
    if ( window == 0 ) {
        return 0;
    }

    new ( window ) window_s;

    window->cb = cb;
    window->userDataSize = userDataSize;
    window->position = position;
    window->size = size;

    void * const userData = userDataSize ? window + 1 : 0;

    if ( window->cb != nullptr ) {
        window->cb( window, kWindow_OnCreate, ( uintptr_t )param, ( uintptr_t )userData );
    }

    Window_SetParent( parent, window );

    return window;
}

void Window_Destroy( window_s * const window ) {
    if ( window == 0 ) {
        return;
    }

    if ( window->cb != nullptr ) {
        window->cb( window, kWindow_OnDestroy, 0, ( uintptr_t )( window + 1 ) );
    }

    free( window );
}

uintptr_t Window_SendMessage( window_s * const window,
                              const windowMsg_e msg,
                              const uintptr_t a,
                              const uintptr_t b ) {
    if ( window->cb != nullptr ) {
        return window->cb( window, msg, a, b );
    }
    return 0;
}

void * Window_GetUserData( window_s * const window ) {
    if ( window == 0 ) {
        return 0;
    }

    return window->userDataSize ? window + 1 : 0;
}

void Window_Render( rgba_s * const surface, const size_t stride, const rect_s< size_t > clip ) {
    if ( root == 0 ) {
        return;
    }

    for ( size_t i = 0; i < root->childCount; i++ ) {
        window_s * const window = root->child[ i ];
        
        if ( window == 0 ) {
            continue;
        }
        
        if ( window->cb == 0 ) {
            continue;
        }

        Window_RenderRecurse( window, surface, stride, clip );
    }
}

void Window_SetParent( window_s * const parent, window_s * const child ) {
    window_s * top = parent;

    if ( parent == 0 ) {
        if ( root == 0 ) {
            root = ( window_s * )malloc( sizeof( window_s ) );
            if ( root == 0 ) {
                assert( root != nullptr );
                return;
            }
            new ( root ) window_s;
        }
        top = root;
    }

    if ( top->childSize == top->childCount ) {
        const size_t growthStep = 8;
        window_s ** const old = top->child;
        top->childSize += growthStep;
        top->child = ( window_s ** )malloc( sizeof( window_s * ) * top->childSize );
        if ( top->child == 0 ) {
            assert( top->child != nullptr );
            top->child = old;
            top->childSize -= growthStep;
            return;
        }
        if ( old != nullptr ) {
            memcpy( top->child, old, sizeof( window_s * ) * top->childCount );
            free( old );
        }
    }

    top->child[ top->childCount++ ] = child;

    if ( top->cb != nullptr ) {
        ( void )top->cb( top, kWindow_OnAddChild, 0, ( uintptr_t )child );
    }
}
