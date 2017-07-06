#include <complex>
#include <iostream>
#include <thread>
#include <fstream>
#include <set>

#include <SDL.h>
#include <stdlib.h> //rand()
#define MAX_ITER 8000

static float r = 0.0f;
static SDL_Window *gWindow = NULL;
static SDL_Surface * gScreenSurface = NULL;

int xRes = 1920, yRes = 1080 ;

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    // printf( "%d\n", surface->pitch ) ;
    Uint32 *target_pixel = (Uint32*)surface->pixels;
    target_pixel[ x * surface->h + y ] = pixel;
}

void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color) {
    //printf( "%d %d\r", x, y ) ;
    if ( x < 0 || y < 0 )
        return ;
    if ( x >= xRes || y >= yRes )
        return ;

    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = color;
}

void render() {
    SDL_FillRect( gScreenSurface, NULL,
                  SDL_MapRGB( gScreenSurface->format,
                              0,
                              0,
                              0
                            )
                ) ;
    std::fstream log ;
    log.open( "log", std::ios::out ) ;
    std::set<double> cRec ;
    //SDL_LockSurface(gScreenSurface);
    {
        /*mandelbrot here*/
        double zrSqr = 0, ziSqr = 0, zrMulzi = 0 ;
        for ( double Re = -2 ; Re <= 2 ; Re += 0.00208 ) {
            for ( double Im = -2 ; Im <= 2 ; Im += 0.00208 ) {
                std::complex<double> z( 0, 0 ) ;
                std::complex<double> c( Re, Im ) ;
                bool in = false, cache = false ;
                int iterCount = 1 ;
                // log << c.real() * c.real() + 2 * c.real() * c.imag() - c.imag() * c.imag() << std::endl ;
                for ( iterCount = 1 ; iterCount <= MAX_ITER ; ++ iterCount ) {
                    static std::complex<double> temp;
                    zrSqr = z.real() * z.real() ;
                    ziSqr = z.imag() * z.imag() ;
                    zrMulzi = z.real() * z.imag() ;
                    temp.real( zrSqr - ziSqr ) ;
                    temp.imag( zrMulzi + zrMulzi ) ;
                    // z = c + std::pow( z, 2 ) ;
                    z = c + temp ;
                    if ( iterCount == 1 && cRec.find( c.real() * c.real() + 2 * c.real() * c.imag() - c.imag() * c.imag() ) != cRec.end() ) {
                        std::cout << "HIT!" << std::endl ;
                        in = true ;
                        cache = true ;
                        break ;
                    } // if
                    else if ( zrSqr + ziSqr > 4 ) {
                        PutPixel32_nolock( gScreenSurface, (int)( Re / 0.00208 + 960 ), (int)( Im / 0.00208 + 540 ), ( iterCount * 20 ) + 0x0055141C ) ;
                        in = false ;
                        // cRec[ c.real() * c.real() + 2 * c.real() * c.imag() - c.imag() * c.imag() ] = iterCount ;
                        //log << z << " OUT[" << iterCount << "," << z.real() * z.real() + z.imag() * z.imag() << "]" << std::endl ;
                        break ;
                    } // if
                    else {
                        //log << z << " ITER[" << iterCount << "," << z.real() * z.real() + z.imag() * z.imag() << "]" << std::endl ;
                        in = true ;
                    } // else
                } // for

                if ( in ) {
                    //std::cout << 0.008 * ( xBorder / 2 )<< " " << 0.008 * ( yBorder / 2 ) << std::endl ;
                    //log << z << " IN" << std::endl ;
                    // system( "pause" ) ;
                    if ( !cache )
                      cRec.insert( c.real() * c.real() + 2 * c.real() * c.imag() - c.imag() * c.imag() ) ;
                    else
                      cache = false ;
                    PutPixel32_nolock( gScreenSurface, (int)( Re / 0.00208 + 960 ), (int)( Im / 0.00208 + 540 ), 0x0 ) ;
                    //SDL_UpdateWindowSurface( gWindow );
                    // std::cout << "(" << (int)( (Re+1)/0.008 ) << "," << (int)( (Im+1)/0.008 ) << ")" << std::endl ;
                } // if
            } // for
        } // for

    }
    //SDL_UnlockSurface(gScreenSurface);
    log.close() ;
    SDL_UpdateWindowSurface( gWindow );
    return ;
} //render

void render2() {
    SDL_LockSurface(gScreenSurface);
    {
        /*mandelbrot here*/
        for ( int x = 0 ; x < xRes ; x ++ ) {
            for ( int y = 0 ; y < yRes ; y ++ ) {
                PutPixel32_nolock( gScreenSurface, x, y, 0x00FFFFFF ) ;
                SDL_UpdateWindowSurface( gWindow );
            } // for
        } // for

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
    //double x =1 , y = 1 ;

    auto start = std::chrono::system_clock::now();
    // do some work
    render() ;
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
