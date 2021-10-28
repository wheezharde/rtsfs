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

#ifndef ___RTSFS_DIGRAPH_H___
#define ___RTSFS_DIGRAPH_H___

#include <stdint.h>

typedef struct digraph_s digraph_s;

typedef enum digraphError_e : uint8_t {
    kDigraphError_None = 0,
    kDigraphError_WouldBeCyclic,
    kDigraphError_InvalidParam,
    kDigraphError_OutOfMemory
} digraphError_e;

typedef enum digraphOpt_e : uint8_t {
    kDigraphOpt_None = 0,
    kDigraphOpt_NonCyclic,
} digraphOpt_e;

typedef struct digraphEntry_s {
    size_t value;
} digraphEntry_s;

digraph_s * Digraph_Create( const size_t hint_initialNodeCount );

void Digraph_Destroy( digraph_s * const me );

// returns the index of the node or SIZE_MAX on error
digraphEntry_s Digraph_AddNode( digraph_s * const me, void * const data );

// returns the parameter assigned to the node or nullptr on error
void * Digraph_RemoveNode( digraph_s * const me, const digraphEntry_s entry );

digraphError_e Digraph_AddDependency( digraph_s * const me, const digraphOpt_e opt, const digraphEntry_s parent, const digraphEntry_s child );
digraphError_e Digraph_RemoveDependency( digraph_s * const me, const digraphEntry_s parent, const digraphEntry_s child );

// returns the parameter assigned to the node, or nullptr on error.
void * Digraph_GetNode( digraph_s * const me, const digraphEntry_s entry );

size_t Digraph_GetChildCount( digraph_s * const me, const digraphEntry_s entry );

// returns the index of the child or SIZE_MAX on error
size_t Digraph_GetChild( digraph_s * const me, const digraphEntry_s parent, const size_t childIndex );

void Digraph_Clear( digraph_s * const me );

void Digraph_Walk( digraph_s * const me, int ( * onNode )( void * const param, void * const parent, void * const child ), void * const param );

#endif // ___RTSFS_DIGRAPH_H___
