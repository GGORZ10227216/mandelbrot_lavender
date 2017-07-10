#include <complex>
#include <iostream>
#include <thread>
#include <fstream>
#include <map>
#include <algorithm>

#include <SDL.h>
#include <stdlib.h> //rand()
#define MAX_ITER 3000

static float r = 0.0f;
static SDL_Window *gWindow = NULL;
static SDL_Surface * gScreenSurface = NULL;

int xRes = 1920, yRes = 1080 ;

struct myComplex {
    double real ;
    double imag ;

    myComplex( const double & r, const double & i )
        : real( r ), imag( i ) {

    } // myComplex
};

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    if ( x < 0 || y < 0 )
        return ;
    if ( x >= xRes || y >= yRes )
        return ;

    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}
using namespace std ;
void render( double xStart, double xEnd, double yStart, double xPan, double yPan ) {
    // 16:9 --> 4:2.25
    SDL_FillRect( gScreenSurface, NULL,
                  SDL_MapRGB( gScreenSurface->format,
                              0,
                              0,
                              0
                            )
                ) ;

    /*mandelbrot here*/
    double xScale = 4. / xRes ;
    double transXstart = -2 + ( xStart * xScale ) ;
    double transYstart = -1.125 + ( yStart * xScale ) ;
    double transXend = -2 + ( xEnd * xScale ) ;
    double transYend = transYstart + ( transXend - transXstart ) * 0.5625 ;
    double resultScale = ( transXend - transXstart ) / 1920 ;
    double yEnd = yStart + ( xEnd - xStart ) * 0.5625 ;
    cout << "transform scale: " << xScale << endl
         << "transXstart: " << transXstart << endl
         << "transYstart: " << transYstart << endl
         << "transXend: " << transXend << endl
         << "transYend: " << transYend << endl
         << "result scale: " << resultScale << endl
         << "real yEnd: " << yEnd << endl ;
    // return ;

    double zrSqr = 0, ziSqr = 0, zrMulzi = 0 ;
    for ( double Re = transXstart ; Re <= transXend ; Re += resultScale ) {
        for ( double Im = transYstart ; Im <= transYend ; Im += resultScale ) {
            myComplex z( 0, 0 ) ;
            myComplex c( Re, Im ) ;
            bool in = false, cache = false ;
            int iterCount = 1 ;
            // log << c.real() * c.real() + 2 * c.real() * c.imag() - c.imag() * c.imag() << std::endl ;
            for ( iterCount = 1 ; iterCount <= MAX_ITER ; ++ iterCount ) {
                if( Re*(1+Re*(8*Re*Re+(16*Im*Im-3)))+Im*Im*(8*Im*Im-3) < 3./32 || ((Re+1)*(Re+1)+Im*Im)<1./16 )
                    iterCount = MAX_ITER ;
                else {
                    myComplex temp( 0, 0 ) ;
                    zrSqr = z.real * z.real ;
                    ziSqr = z.imag * z.imag ;
                    zrMulzi = z.real * z.imag ;
                    temp.real = zrSqr - ziSqr  ;
                    temp.imag = zrMulzi + zrMulzi ;

                    if ( zrSqr + ziSqr > 4 ) {
                        putpixel( gScreenSurface, (int)( Re / resultScale + xPan ),
                                  (int)( Im / resultScale + yPan ), ( iterCount * 20 ) + 0x0055141C ) ;
                        // renderMem[] ;
                        iterCount = MAX_ITER ;
                    } // if
                    else {
                        z.real = c.real + temp.real ;
                        z.imag = c.imag + temp.imag ;
                    } // else
                } // else
            } // for
        } // for
    } // for

    SDL_UpdateWindowSurface( gWindow );
    return ;
} //render

void render2() {
    SDL_LockSurface(gScreenSurface);
    {
        Uint8 * pixel = (Uint8*)gScreenSurface->pixels ;
        memset( pixel, 0xFFFFFF00, 1920*1080*4 ) ;
    }
    SDL_UnlockSurface(gScreenSurface);

    SDL_UpdateWindowSurface( gWindow );
    return ;
} //render

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    gWindow = SDL_CreateWindow( "title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xRes, yRes, SDL_WINDOW_SHOWN );

    gScreenSurface = SDL_GetWindowSurface( gWindow );
    SDL_FillRect( gScreenSurface, NULL,
                  SDL_MapRGB( gScreenSurface->format,
                              0,
                              0,
                              0
                            )
                ) ;
    // SDL_AddEventWatch(watch, NULL);
    // system( "pause" ) ;
    // std::thread renderThread( render ) ;
    bool quitting = false ;
    double x = xRes/2, y = yRes/2 ;
    int sx = 0, ex = xRes, sy = 0 ;
    auto start = std::chrono::system_clock::now();
    // do some work
    for ( int i = -)
    render( sx, ex, sy, xRes/2, yRes/2 ) ;
    // record end time
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end-start;
    std::cout << "Render time is " << diff.count() << " s\n";


    while(!quitting) {

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if(event.type == SDL_QUIT) {
                quitting = true;
            } // if
            else if ( event.type == SDL_KEYDOWN ) {
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    x-=10;
                    break;
                case SDLK_RIGHT:
                    x+=10;
                    break;
                case SDLK_UP:
                    y-=10;
                    break;
                case SDLK_DOWN:
                    y+=10;
                    break;
                case SDLK_q:
                    sx+=1;
                    break;
                case SDLK_a:
                    sx-=1;
                    break;
                case SDLK_w:
                    ex+=1;
                    break;
                case SDLK_s:
                    ex-=1;
                    break;
                case SDLK_e:
                    sy+=1;
                    break;
                case SDLK_d:
                    sy-=1;
                    break;
                }


                cout << x << " " << y << endl ;
                //render( sx, ex, sy, x, y ) ;
            } // else
            else {
                if ( event.type == SDL_MOUSEBUTTONDOWN ) {
                    switch (event.button.button) {
                    case SDL_BUTTON_LEFT : {
                        int mx = 0, my = 0 ;
                        SDL_GetMouseState( &mx, &my ) ;
                        printf( "%d %d\n", mx, my ) ;
                    }
                    break;
                    case SDL_BUTTON_RIGHT:
                        printf( "right click" ) ;
                        break;
                    default:
                        printf( "other click" ) ;
                        break;
                    }
                    break;
                } // if
            } // else
        } // while

        // render( x, y ) ;
        // x -= 0.1 ;
        // y -= 0.1 ;
        // SDL_Delay(10);
    }

    // renderThread.join() ;
    SDL_DestroyWindow(gWindow);
    SDL_Quit();

    return 0 ;
} //main
