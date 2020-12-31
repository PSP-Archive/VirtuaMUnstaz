#include <stdarg.h>
#include <string.h>

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>

#include "SDL.h"
#include "prototypes.h"

#define VMU_IMG "VMU.BMP"
//#define VMU_GAME "shs.lcd"
#define VMU_GAME "SCROLL.VMS"

#define WIDTH 48*2
#define HEIGHT 40*2

#define XOFFSET 36
#define YOFFSET 85

extern void scroller();
extern void putpixel();

extern SDL_Surface *screen;

SDL_Joystick *stick;
unsigned short fg, bg;

void error_msg(char *fmt, ...) {
  va_list va;
  va_start(va, fmt);
  pspDebugScreenPrintf(fmt, va);
  pspDebugScreenPrintf("\n");
  va_end(va);
}

int last_index_of(char *str, char c) {
	int i = strlen(str) - 1;
	while(i > 0 && str[i] != c) i--;
	return i;
}

int psp_vms(void *data) {
	SDL_Surface *vmu;
	char **argv;
	
	argv = (char **)data;
	char image[strlen(argv[0]) + strlen(VMU_IMG)];
	char game[strlen(argv[0]) + strlen(VMU_GAME)];
	// cut off the EBOOT.PBP
	argv[0][last_index_of(argv[0], '/')] = 0;
	
	sprintf(image, "%s/%s", argv[0], VMU_IMG); 
	sprintf(game, "%s/%s", argv[0], VMU_GAME);
		
	vmu = SDL_LoadBMP(image);
	SDL_BlitSurface(vmu, NULL, screen, NULL);
	SDL_Flip(screen);

	SDL_Rect display_rect = { XOFFSET, YOFFSET, WIDTH, HEIGHT };
	SDL_FillRect(screen, &display_rect, bg);

	SDL_Flip(screen);
	
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	stick = SDL_JoystickOpen(0);
	
	fg = SDL_MapRGB(screen->format, 0x47, 0x13, 0x78);
	bg = SDL_MapRGB(screen->format, 0x57, 0x89, 0x75);
	
//	do_lcdimage(game);
	do_vmsgame(game, NULL);
	
	return 0;
}

void vmputpixel(int x, int y, int p)
{
	int screen_x, screen_y;
	screen_x = (x<<1) + XOFFSET;
	screen_y = (y<<1) + YOFFSET;
	
	unsigned short color = p & 0x1 ? fg : bg;
	
	putpixel(screen_x, screen_y, color);	
	putpixel(screen_x+1, screen_y, color);	
	putpixel(screen_x, screen_y+1, color);	
	putpixel(screen_x+1, screen_y+1, color);	
}

void redrawlcd()
{
	SDL_Flip(screen);
}

/*
 0 - triangle
 1 - circle
 2 - cross
 3 - square
 4 - left trigger
 5 - right trigger
 6 - down
 7 - left
 8 - up
 9 - right
10 - select
11 - start
12 - home
13 - hold
*/

#define TRIANGLE	0
#define CIRCLE	1
#define CROSS	2
#define SQUARE	3
#define T_LEFT	4
#define T_RIGHT	5
#define DOWN		6
#define LEFT		7
#define UP		8
#define RIGHT	9
#define SELECT	10
#define START	11
#define HOME		12
#define HOLD		13


char buttons = 0;

void checkevents()
{
	SDL_JoystickUpdate();
	
//	pspDebugScreenSetXY(0,0);
//	pspDebugScreenPrintf("%s: %d: up\n",SDL_JoystickName(0), SDL_JoystickGetButton(stick, UP));
//	pspDebugScreenPrintf("Axis: [%d]", SDL_JoystickGetAxis(stick, 0));

	if(SDL_JoystickGetButton(stick, UP) && !(buttons & (1 << UP)))  { keypress(0); buttons |= (1 << UP); }
	if(SDL_JoystickGetButton(stick, DOWN) && !(buttons & (1 << DOWN))) { keypress(1); buttons |= (1 << DOWN); }
	if(SDL_JoystickGetButton(stick, LEFT) && !(buttons & (1 << LEFT))) { keypress(2); buttons |= (1 << LEFT); }
	if(SDL_JoystickGetButton(stick, RIGHT) && !(buttons & (1 << RIGHT))) { keypress(3); buttons |= (1 << RIGHT); }
	if(SDL_JoystickGetButton(stick, CROSS) && !(buttons & (1 << CROSS))) { keypress(4); buttons |= (1 << CROSS); }
	if(SDL_JoystickGetButton(stick, CIRCLE) && !(buttons & (1 << CIRCLE))) { keypress(5); buttons |= (1 << CIRCLE); }
	if(SDL_JoystickGetButton(stick, SELECT) && !(buttons & (1 << SELECT))) { keypress(6); buttons |= (1 << SELECT); }
	if(SDL_JoystickGetButton(stick, START) && !(buttons & (1 << START))) { keypress(7); buttons |= (1 << START); }

	if(!SDL_JoystickGetButton(stick, UP) && (buttons & (1 << UP)))  { keyrelease(0); buttons ^= (1 << UP); }
	if(!SDL_JoystickGetButton(stick, DOWN) && (buttons & (1 << DOWN))) { keyrelease(1); buttons ^= (1 << DOWN); }
	if(!SDL_JoystickGetButton(stick, LEFT) && (buttons & (1 << LEFT))) { keyrelease(2); buttons ^= (1 << LEFT); }
	if(!SDL_JoystickGetButton(stick, RIGHT) && (buttons & (1 << RIGHT))) { keyrelease(3); buttons ^= (1 << RIGHT); }
	if(!SDL_JoystickGetButton(stick, CROSS) && (buttons & (1 << CROSS))) { keyrelease(4); buttons ^= (1 << CROSS); }
	if(!SDL_JoystickGetButton(stick, CIRCLE) && (buttons & (1 << CIRCLE))) { keyrelease(5); buttons ^= (1 << CIRCLE); }
	if(!SDL_JoystickGetButton(stick, SELECT) && (buttons & (1 << SELECT))) { keyrelease(6); buttons ^= (1 << SELECT); }
	if(!SDL_JoystickGetButton(stick, START) && (buttons & (1 << START))) { keyrelease(7); buttons ^= (1 << START); }

	scroller();
}

void waitforevents(struct timeval *t)
{
	SDL_WaitEvent(NULL);
}

void sound(int freq)
{
}

