#include <stdlib.h>
#include <string.h>

#include <pspkernel.h>
#include <pspdebug.h>

#include "SDL.h"
#include "SDL_thread.h"

extern int psp_vms(char *argv[]);

SDL_Surface *screen;
int SDL_main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
	
	screen = SDL_SetVideoMode(480, 272, 16, SDL_SWSURFACE|SDL_ANYFORMAT);
	SDL_ShowCursor(SDL_DISABLE);
		
	setup_scroller();		
	psp_vms(argv);

	return 0;	
}

void putpixel(int x, int y, int pixel)
{
	if(x > 479 || y > 271) return;
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

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


