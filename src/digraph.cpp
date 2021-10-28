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

#include "digraph.h"

#include <assert.h>
#include <memory.h>
#include <new>

typedef struct digraphNode_s {
    bool inUse = false;
    void * data = nullptr;
    size_t * child = nullptr;
    size_t childCount = 0;
    size_t childSize = 0;
} digraphNode_s;

typedef struct digraph_s {
    digraphNode_s * node = nullptr;
    size_t nodeCount = 0;
    size_t nodeSize = 0;
    size_t * head = nullptr;
    size_t headCount = 0;
    size_t headSize = 0;
} digraph_s;

static bool InsertHeadNode( digraph_s * const me, const size_t index ) {
    for ( size_t i = 0; i < me->headCount; i++ ) {
        if ( me->head[ i ] == index ) {
            return true;
        }
    }

    if ( me->headCount == me->headSize ) {
        constexpr size_t step = 16;
        size_t * const old = me->head;
        me->head = new size_t[ me->headSize + step ];
        if ( me->head == nullptr ) {
            me->head = old;
            return false;
        }
        me->headSize += step;
        if ( old != nullptr ) {
            memcpy( me->head, old, sizeof( size_t ) * me->headCount );
            delete [] old;
       }
    }

    me->head[ me->headCount++ ] = index;
    return true;
}

static void RemoveHeadNode( digraph_s * const me, const size_t index ) {
    for ( size_t i = 0; i < me->headCount; i++ ) {
        if ( me->head[ i ] == index ) {
            memmove( me->head + i, me->head + i + 1, sizeof( size_t ) * ( me->headCount - ( i + 1 ) ) );
            me->headCount--;
            return;
        }
    }
}

static bool CanReach( digraph_s * const me, const size_t nodeIndex, const size_t searchFor ) {
    assert( nodeIndex < me->nodeCount );
    digraphNode_s * const node = me->node + nodeIndex;
    for ( size_t i = 0; i < node->childCount; i++ ) {
        const size_t childIndex = node->child[ i ];
        if ( childIndex == searchFor ) {
            return true;
        }
        if ( CanReach( me, childIndex, searchFor ) ) {
            return true;
        }
    }
    return false;
}

static int Walk( digraph_s * const me, const size_t nodeIndex, int ( * onNode )( void * const param, void * const parent, void * const child ), void * const param ) {
    assert( nodeIndex < me->nodeCount );
    digraphNode_s * const node = me->node + nodeIndex;
    if ( node->childCount != 0 ) {
        for ( size_t i = 0; i < node->childCount; i++ ) {
            const size_t childIndex = node->child[ i ];
            digraphNode_s * child = me->node + childIndex;
            if ( onNode( param, node->data, child->data ) == 0 ) {
                return 0;
            }
            if ( Walk( me, childIndex, onNode, param ) == 0 ) {
                return 0;
            }
        }
    } else {
        if ( onNode( param, node->data, nullptr ) == 0 ) {
            return 0;
        }
    }
    return 1;
}

digraph_s * Digraph_Create( const size_t hint_initialNodeCount ) {
    digraph_s * const me = new digraph_s;
    if ( me == nullptr ) {
        return nullptr;
    }

    if ( hint_initialNodeCount != 0 ) {
        me->node = new digraphNode_s[ hint_initialNodeCount ];
        if ( me->node == nullptr ) {
            delete me;
            return nullptr;
        }
        me->nodeSize = hint_initialNodeCount;
    }

    return me;
}

void Digraph_Destroy( digraph_s * const me ) {
    if ( me == nullptr ) {
        return;
    }

    Digraph_Clear( me );

    delete me;
}

digraphEntry_s Digraph_AddNode( digraph_s * const me, void * const data ) {
    if ( me == nullptr ) {
        return { SIZE_MAX };
    }

    size_t nodeIndex = SIZE_MAX;

    for ( size_t i = 0; i < me->nodeCount; i++ ) {
        digraphNode_s * const node = me->node + i;
        if ( !node->inUse ) {
            node->inUse = true;
            node->data = data;
            nodeIndex = i;
            break;
        }
    }

    if ( nodeIndex == SIZE_MAX ) {
        if ( me->nodeCount == me->nodeSize ) {
            constexpr size_t step = 16;
            digraphNode_s * const old = me->node;
            me->node = new digraphNode_s[ me->nodeSize + step ];
            if ( me->node == nullptr ) {
                me->node = old;
                return { SIZE_MAX };
            }
            me->nodeSize += step;
            if ( old != nullptr ) {
                memcpy( me->node, old, sizeof( digraphNode_s ) * me->nodeCount );
                delete [] old;
            }
        }

        nodeIndex = me->nodeCount++;
        digraphNode_s * const node = me->node + nodeIndex;
        node->inUse = true;
        node->data = data;
    }

    if ( !InsertHeadNode( me, nodeIndex ) ) {
        me->node[ nodeIndex ].inUse = false;
        return { SIZE_MAX };
    }

    return { nodeIndex };
}

void * Digraph_RemoveNode( digraph_s * const me, const digraphEntry_s entry ) {
    if ( me == nullptr ) {
        return nullptr;
    }

    if ( entry.value > me->nodeCount ) {
        return nullptr;
    }

    for ( size_t i = 0; i < me->nodeCount; i++ ) {
        if ( i != entry.value ) {
            Digraph_RemoveDependency( me, { i }, entry );
        }
    }

    digraphNode_s * const node = me->node + entry.value;

    for ( size_t i = 0; i < node->childCount; i++ ) {
        InsertHeadNode( me, node->child[ i ] );
    }

    void * const data = node->data;
    delete [] node->child;
    new ( node ) digraphNode_s;

    RemoveHeadNode( me, entry.value );

    for ( size_t i = me->nodeCount; i > 0; i-- ) {
        size_t index = i - 1;
        if ( me->node[ index ].inUse == false ) {
            me->nodeCount--;
        }
    }

    return data;
}

digraphError_e Digraph_AddDependency( digraph_s * const me, const digraphOpt_e opt, const digraphEntry_s parent, const digraphEntry_s child ) {
    if ( me == nullptr ) {
        return kDigraphError_InvalidParam;
    }
    if ( parent.value > me->nodeCount ) {
        return kDigraphError_InvalidParam;
    }
    if ( child.value > me->nodeCount ) {
        return kDigraphError_InvalidParam;
    }

    const bool cyclic = ( parent.value == child.value ) || CanReach( me, child.value, parent.value );
    if ( opt == kDigraphOpt_NonCyclic && cyclic ) {
        return kDigraphError_WouldBeCyclic;
    }

    digraphNode_s * const node = me->node + parent.value;

    if ( node->childCount == node->childSize ) {
        constexpr size_t step = 4;
        size_t * const old = node->child;
        node->child = new size_t[ node->childSize + step ];
        if ( node->child == nullptr ) {
            node->child = old;
            return kDigraphError_OutOfMemory;
        }
        node->childSize += step;
        if ( old != nullptr ) {
            memcpy( node->child, old, sizeof( size_t ) * node->childCount );
            delete [] old;
        }
    }

    node->child[ node->childCount++ ] = child.value;

    if ( !cyclic ) {
        RemoveHeadNode( me, child.value );
    }

    return kDigraphError_None;
}

digraphError_e Digraph_RemoveDependency( digraph_s * const me, const digraphEntry_s parent, const digraphEntry_s child ) {
    if ( me == nullptr ) {
        return kDigraphError_InvalidParam;
    }
    if ( parent.value > me->nodeCount ) {
        return kDigraphError_InvalidParam;
    }
    if ( child.value > me->nodeCount ) {
        return kDigraphError_InvalidParam;
    }

    digraphNode_s * const node = me->node + parent.value;
    size_t * dst = node->child;
    bool addHeadNode = false;
    const size_t count = node->childCount;
    for ( size_t * src = node->child; src != node->child + count; src++ ) {
        if ( *src != child.value ) {
            *dst++ = *src;
        } else {
            addHeadNode = true;
            node->childCount--;
        }
    }

    if ( addHeadNode ) {
        InsertHeadNode( me, child.value );
    }

    return kDigraphError_None;
}

void * Digraph_GetNode( digraph_s * const me, const size_t id ) {
    if ( me == nullptr ) {
        return nullptr;
    }
    if ( id > me->nodeCount ) {
        return nullptr;
    }
    return me->node[ id ].data;
}

size_t Digraph_GetChildCount( digraph_s * const me, const digraphEntry_s entry ) {
    if ( me == nullptr ) {
        return SIZE_MAX;
    }
    if ( entry.value > me->nodeCount ) {
        return SIZE_MAX;
    }
    return me->node[ entry.value ].childCount;
}

size_t Digraph_GetChild( digraph_s * const me, const digraphEntry_s entry, const size_t childIndex ) {
    if ( me == nullptr ) {
        return SIZE_MAX;
    }
    if ( entry.value > me->nodeCount ) {
        return SIZE_MAX;
    }
    if ( childIndex > me->node[ entry.value ].childCount ) {
        return SIZE_MAX;
    }
    return me->node[ entry.value ].child[ childIndex ];
}

void Digraph_Clear( digraph_s * const me ) {
    for ( size_t i = 0; i < me->nodeCount; i++ ) {
        delete [] me->node[ i ].child;
    }
    delete [] me->node;
    delete [] me->head;

    new ( me ) digraph_s;
}

void Digraph_Walk( digraph_s * const me, int ( * onNode )( void * const param, void * const parent, void * const child ), void * const param ) {
    for ( size_t i = 0; i < me->headCount; i++ ) {
        if ( Walk( me, me->head[ i ], onNode, param ) == 0 ) {
            break;
        }
    }
}
