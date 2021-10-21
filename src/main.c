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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdint.h>
#include <stdlib.h>

#include "config.h"
#include "rgba.h"
#include "window.h"

typedef struct appData_s {
    BITMAPINFO ** buffers;
    uint32_t bufferForeground;
    uint32_t bufferCount;
    HDC dc;
    HBITMAP bmp;
    int width;
    int height;
    int framerate;
    int frametime;
    uint64_t nextframe;
} appData_s;

static LRESULT CALLBACK myWindowProc( HWND wnd, UINT msg, WPARAM wp, LPARAM lp ) {
    switch ( msg ) {
        case WM_CREATE: {
            CREATESTRUCT * const cs = ( CREATESTRUCT * )lp;
            appData_s * const me = ( appData_s * )cs->lpCreateParams;

            SetWindowLongPtr( wnd, GWLP_USERDATA, ( LONG_PTR )me );
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC const dc = BeginPaint( wnd, &ps );;

            RECT r;
            GetClientRect( wnd, &r );

            appData_s * const me = ( appData_s * )GetWindowLongPtr( wnd, GWLP_USERDATA );

            if ( me->width != r.right - r.left || me->height != r.bottom - r.top ) {
                me->width = r.right - r.left;
                me->height = r.bottom - r.top;

                ReleaseDC( NULL, me->dc );
                DeleteObject( me->bmp );

                if ( me->buffers != NULL ) {
                    for ( size_t i = 0; i < me->bufferCount; i++ ) {
                        if ( me->buffers[ i ] != NULL ) {
                            free( me->buffers[ i ] );
                        }
                    }
                    free( me->buffers );
                }

                me->dc = CreateCompatibleDC( GetDC( NULL ) );
                me->bmp = CreateCompatibleBitmap( GetDC( NULL ), me->width, me->height );

                SelectObject( me->dc, me->bmp );

                me->buffers = ( BITMAPINFO ** )malloc( sizeof( BITMAPINFO * ) * me->bufferCount );
                if ( me->buffers != NULL ) {
                    for ( size_t i = 0; i < me->bufferCount; i++ ) {
                        const size_t size = sizeof( BITMAPINFO ) + sizeof( COLORREF ) * me->width * me->height;
                        BITMAPINFO * const bmi = ( BITMAPINFO * )malloc( size );
                        me->buffers[ i ] = bmi;
                        if ( bmi == NULL ) {
                            continue;
                        }
                        memset( bmi, 0, size );
                        bmi->bmiHeader.biSize = sizeof( BITMAPINFO );
                        bmi->bmiHeader.biWidth = ( LONG )me->width;
                        bmi->bmiHeader.biHeight = ( LONG )me->height;
                        bmi->bmiHeader.biPlanes = 1;
                        bmi->bmiHeader.biBitCount = 32;
                        bmi->bmiHeader.biCompression = BI_RGB;
                        bmi->bmiHeader.biSizeImage = 0;
                        bmi->bmiHeader.biXPelsPerMeter = 0;
                        bmi->bmiHeader.biYPelsPerMeter = 0;
                        bmi->bmiHeader.biClrUsed = 0;
                        bmi->bmiHeader.biClrImportant = 0;
                    }
                }
            }

            me->bufferForeground++;
            if ( me->bufferForeground >= me->bufferCount ) {
                me->bufferForeground = 0;
            }

            BITMAPINFO * const bmi = me->buffers[ me->bufferForeground ];
            if ( bmi != NULL ) {
                SetDIBits( me->dc, me->bmp, 0, me->height, bmi + 1, bmi, DIB_RGB_COLORS );
                BitBlt( dc,
                        ps.rcPaint.left,
                        ps.rcPaint.top,
                        ps.rcPaint.right - ps.rcPaint.left,
                        ps.rcPaint.bottom - ps.rcPaint.top,
                        me->dc,
                        ps.rcPaint.left,
                        ps.rcPaint.top,
                        SRCCOPY );
            }

            EndPaint( wnd, &ps );
        } break;
        case WM_CLOSE:
            PostQuitMessage( 0 );
            break;
    }

    return DefWindowProc( wnd, msg, wp, lp );
}

static ATOM registerWindowClass( const char * const classname ) {
    WNDCLASSEXA wndClass;
    memset( &wndClass, 0, sizeof( wndClass ) );
    wndClass.cbSize = sizeof( wndClass );
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = myWindowProc;
    wndClass.hInstance = GetModuleHandle( NULL );
    wndClass.lpszClassName = classname;
    return RegisterClassExA( &wndClass );
}

static void unregisterWindowClass( const char * const classname ) {
    UnregisterClassA( classname, GetModuleHandle( NULL ) );
}

static int update( void ) {
    return 0;
}

static void render( void ) {
}

static uintptr_t windowCallback( window_s * const window, const windowMsg_e msg, const uintptr_t a, const uintptr_t b ) {
    ( void )window;
    ( void )msg;
    ( void )a;
    ( void )b;

    switch ( msg ) {
        case kWindow_OnCreate:
            return 1;

        case kWindow_OnDestroy:
            return 0;

        case kWindow_OnRender: {
            windowRenderData_s * const renderData = ( windowRenderData_s * )a;
            // todo: clip
            rgba_s * pix = renderData->surface + ( uintptr_t )( renderData->position.y * renderData->stride + renderData->position.x );
            for ( size_t y = 0; y < renderData->size.y; y++ ) {
                memset( pix , 0xcc, ( size_t )(renderData->size.x * sizeof( rgba_s ) ) );
                pix += renderData->stride;
            }
        } break;

        case kWindow_OnSize:
            return 0;

        case kWindow_OnPosition:
            return 0;

        case kWindow_OnAddChild:
            return 0;

        default:
            break;
    }

    return 0;
}

int __stdcall WinMain( HINSTANCE inst, HINSTANCE prev, char * cmdline, int show ) {
    ( void )inst;
    ( void )prev;
    ( void )cmdline;
    ( void )show;

    uint8_t runEditor = 0;

    configRule_s configRule[] = {
        // name     parseFunction        parseParam  arg
        { "editor", Config_ParseBoolean, &runEditor, kConfigArg_Required },
    };
    const size_t configRuleCount = sizeof( configRule ) / sizeof( configRule[ 0 ] );

    const configResult_s result = Config_Parse( __argc, __argv, configRuleCount, configRule );
    ( void )result;

    const char * windowClassName = "rtsfs";

    ATOM wndAtom = registerWindowClass( windowClassName );
    if ( wndAtom == 0 ) {
        return -1;
    }

    appData_s appData;
    memset( &appData, 0, sizeof( appData ) );

    appData.bufferCount = 3;
    appData.framerate = 60;
    appData.frametime = 1000 / appData.framerate;

    HWND wnd = CreateWindowExA( 0,
                                windowClassName,
                                "RTS From Scratch",
                                WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                0,
                                0,
                                1024,
                                768,
                                NULL,
                                NULL,
                                inst,
                                &appData );

    appData.nextframe = GetTickCount64() + appData.frametime;

    window_s * const w = Window_Create( 0, windowCallback, vec2f32_zero(), ( vec2f32_s ){ 100, 100 }, 64, 0 );

    if ( wnd != NULL ) {
        for ( ;; ) {
            for ( ;; ) {
                const ULONGLONG now = GetTickCount64();
                if ( now >= appData.nextframe ) {
                    appData.nextframe += appData.frametime;
                    break;
                }
                Sleep( 1 );
            }

            MSG msg;
            while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
                if ( msg.message == WM_QUIT ) {
                    break;
                }
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }

            if ( msg.message == WM_QUIT ) {
                break;
            }

            rgba_s * const rgba = ( rgba_s * )( appData.buffers[ appData.bufferForeground ] + 1 );

            Window_Render( rgba,
                           appData.width,
                           ( rectf32_s ){
                               vec2f32_zero(),
                               ( vec2f32_s ){ ( float )appData.width, ( float )appData.height }
                           }  );

            InvalidateRect( wnd, 0, FALSE );

            render();

            if ( update() != 0 ) {
                break;
            }
        }
    }

    Window_Destroy( w );

    ReleaseDC( wnd, appData.dc );
    DeleteObject( appData.bmp );

    if ( appData.buffers != NULL ) {
        for ( size_t i = 0; i < appData.bufferCount; i++ ) {
            if ( appData.buffers[ i ] != NULL ) {
                free( appData.buffers[ i ] );
            }
        }
        free( appData.buffers );
    }

    unregisterWindowClass( windowClassName );

    return 0;
}
