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

#ifndef ___RTSFS_CONFIG_H___
#define ___RTSFS_CONFIG_H___

#include <stddef.h>
#include <stdint.h>

typedef enum configError_e {
    kConfigError_None = 0,
    kConfigError_MalformedArgument,
    kConfigError_UnknownArgument,
    kConfigError_MissingValue,
    kConfigError_ArgumentRequired,
    kConfigError_ArgumentNotAllowed,
} configError_e;

typedef enum configArg_e {
    kConfigArg_None,     // 'argument'; 'argument=abc' generates an error
    kConfigArg_Required, // 'argument=abc'; 'argument' generates an error
    kConfigArg_Optional, // 'argument=abc' or 'argument'
} configArg_e;

typedef struct configResult_s {
    configError_e error; // what is the type of error
    size_t argumentIndex; // which argument has the error
} configResult_s;

typedef struct configRule_s {
    const char * name;  // required: set by the caller to indicate the argument name

    // optional; called if the argument contains an "=value", passes just "value"
    void ( * parseFunction )( void * const param, const char * const value );
    void * parseParam;

    configArg_e arg;    // required: set by the caller to indicate the argument requirement

    int32_t present;    // ignored; set by Config_Parse to indicate if the argument was present
    const char * value; // ignored: set by Config_Parse to point to the string value
} configRule_s;

configResult_s Config_Parse( const size_t count, const char * const * const argv, const size_t ruleCount, configRule_s * const rules );

// helper config parser functions

// number must be a pointer to an int32
void Config_ParseInt32( void * const result, const char * const value );

// number must be a pointer to an uint8_t
// evaluates "1" "yes" or "true" (case insensitive) to true (1)
// evaluates all others to false (0)
void Config_ParseBoolean( void * const result, const char * const value );

#endif /* ___RTSFS_CONFIG_H___ */
