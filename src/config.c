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

#include "config.h"

#include <stdlib.h>
#include <string.h>

configResult_s Config_Parse( const size_t count, const char * const * const argv, const size_t ruleCount, configRule_s * const rules ) {
    for ( configRule_s * rule = rules; rule < rules + ruleCount; rule++ ) {
        rule->present = 0;
        rule->value = 0;
    }

    for ( const char * const * namePtr = argv + 1; namePtr < argv + count; namePtr++ ) {
        const char * const name = *namePtr;

        if ( name == 0 ) {
            continue;
        }

        const char * const eq = strstr( name, "=" );

        const ptrdiff_t nameLength = eq ? ( eq - name ) : strlen( name );

        if ( eq && eq[ 1 ] == 0 ) {
            return ( configResult_s ){ kConfigError_MissingValue, namePtr - argv };
        }

        int handled = 0;

        for ( configRule_s * rule = rules; rule < rules + ruleCount; rule++ ) {
            if ( rule->name == 0 ) {
                continue;
            }

            if ( rule->name[ 0 ] != name[ 0 ] ) {
                continue;
            }

            const ptrdiff_t ruleLength = strlen( rule->name );

            if ( ruleLength != nameLength ) {
                continue;
            }

            if ( 0 != strncmp( rule->name, name, nameLength ) ) {
                continue;
            }

            if ( rule->arg == kConfigArg_None && eq != 0 ) {
                return ( configResult_s ){ kConfigError_ArgumentNotAllowed, namePtr - argv };
            }

            else if ( rule->arg == kConfigArg_Required && eq == 0 ) {
                return ( configResult_s ){ kConfigError_ArgumentRequired, namePtr - argv };
            }

            rule->value = eq != 0 ? eq + 1 : 0;

            if ( eq != 0 && rule->parseFunction != 0 ) {
                rule->parseFunction( rule->parseParam, eq + 1 );
            }

            rule->present = 1;
            handled = 1;

            break;
        }

        if ( !handled ) {
            return ( configResult_s ){ kConfigError_UnknownArgument, namePtr - argv };
        }
    }

    return ( configResult_s ){ kConfigError_None, 0 };
}

void Config_ParseInt32( void * const number, const char * const value ) {
    *( int32_t * )( number ) = atoi( value );
}

void Config_ParseBoolean( void * const result, const char * const value ) {
    const char * const possibilities[] = {
        "true",
        "yes",
        "1",
    };
    for ( size_t i = 0; i < sizeof( possibilities ) / sizeof( possibilities[ 0 ] ); i++ ) {
        if ( _stricmp( possibilities[ i ], value ) == 0 ) {
            *( uint8_t * )( result ) = 1;
            return;
        }
    }
    *( uint8_t * )( result ) = 0;
}
