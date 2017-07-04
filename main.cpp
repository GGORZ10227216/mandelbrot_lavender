#include <complex>
#include <iostream>
#include <thread>

#include <SDL.h>
#include <stdlib.h> //rand()
#define MAX_ITER 600

static float r = 0.0f;
static SDL_Window *gWindow = NULL;
static SDL_Surface * gScreenSurface = NULL;

int xRes = 800, yRes = 600 ;

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
  // printf( "%d\n", surface->pitch ) ;
  Uint32 *target_pixel = (Uint32*)surface->pixels;
  target_pixel[ x * surface->h + y ] = pixel;
}

void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint32));
    *((Uint32*)pixel) = color;
}

void render() {
  SDL_LockSurface(gScreenSurface); {
    /*mandelbrot here*/
    for ( double Re = -2 ; Re <= 2 ; Re += 0.004 ) {
      for ( double Im = -2 ; Im <= 2 ; Im += 0.004 ) {
        std::complex<double> z( 0, 0 ) ;
        std::complex<double> c( Re, Im ) ;
        bool in = false ;
        for ( int iterCount = 1 ; iterCount <= MAX_ITER ; ++ iterCount ) {
          z = c + std::pow( z, 2 ) ;
          if ( std::sqrt( z.real() * z.real() + z.imag() * z.imag() ) > 2 ) {
            in = false ;
            break ;
          } // if
          else
            in = true ;
        } // for

        if ( in ) {
          PutPixel32_nolock( gScreenSurface, (int)( Re/0.004 + 400 ), (int)( Im/0.004 + 300 ), 0x00FFFFFF ) ;
          //SDL_UpdateWindowSurface( gWindow );
          // std::cout << "(" << (int)( (Re+1)/0.008 ) << "," << (int)( (Im+1)/0.008 ) << ")" << std::endl ;
        } // if
      } // for
    } // for

  } SDL_UnlockSurface(gScreenSurface);

  SDL_UpdateWindowSurface( gWindow );
  return ;
} //render

void render2() {
  SDL_LockSurface(gScreenSurface); {
    /*mandelbrot here*/
    for ( int x = 0 ; x < xRes ; x ++ ) {
        for ( int y = 0 ; y < yRes ; y ++ ) {
           PutPixel32_nolock( gScreenSurface, x, y, 0x00FFFFFF ) ;
           SDL_UpdateWindowSurface( gWindow );
        } // for
    } // for

  } SDL_UnlockSurface(gScreenSurface);

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
    system( "pause" ) ;
    std::thread renderThread( render ) ;
    bool quitting = false ;
    while(!quitting) {

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if(event.type == SDL_QUIT) {
                quitting = true;
            }
        }

        SDL_Delay(2);
    }

    renderThread.join() ;
    SDL_DestroyWindow(gWindow);
    SDL_Quit();

    return 0 ;
} //main
