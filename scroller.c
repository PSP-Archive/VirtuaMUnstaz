#include <stdarg.h>
#include <string.h>
#include <pspkernel.h>
#include <pspdebug.h>

#include "SDL.h"
#include "font.h"

extern void putpixel(int x, int y, int color);
extern SDL_Surface *screen;

int put_char(int x, int y, char c, int fg, int bg, int stretch, int skip, int mirror);
void put_str(int x, int y, char *c, int length, int fg, int bg, int stretch);

#define X1 170
#define Y1 237
#define X2 480
#define Y2 272

#define SKIP 0
//#define REAL_SINE_SCROLLER 1

SDL_Rect scroll_rect = {
	X1, Y1, X2 - X1, Y2 - Y1		
};

// the sine, thanks to WAB
int sine[120]=
{ 259 , 260 , 260 , 261 , 262 , 262 , 263 , 264 ,
 264 , 264 , 265 , 265 , 265 , 265 , 265 , 265 ,
 265 , 265 , 265 , 264 , 264 , 264 , 263 , 262 ,
 262 , 261 , 260 , 260 , 259 , 258 , 257 , 256 ,
 256 , 255 , 254 , 254 , 253 , 252 , 252 , 252 ,
 251 , 251 , 251 , 251 , 251 , 251 , 251 , 251 ,
 251 , 252 , 252 , 252 , 253 , 254 , 254 , 255 ,
 256 , 256 , 257 , 258 , 259 , 260 , 260 , 261 ,
 262 , 262 , 263 , 264 , 264 , 264 , 265 , 265 ,
 265 , 265 , 265 , 265 , 265 , 265 , 265 , 264 ,
 264 , 264 , 263 , 262 , 262 , 261 , 260 , 260 ,
 259 , 258 , 257 , 256 , 256 , 255 , 254 , 254 ,
 253 , 252 , 252 , 252 , 251 , 251 , 251 , 251 ,
 251 , 251 , 251 , 251 , 251 , 252 , 252 , 252 ,
 253 , 254 , 254 , 255 , 256 , 256 , 257 , 258 };


char *text = 
" \1 \1 \1 "
"VirtuaMUnstaz are back with this little Dreamcast VMU demo"
" \1 "
"Please Press X and O to start the emulated demo on the VMU."
" \1 "
"Greetz to the PSPSDK team! Great work! \5 "
"This little demo was inspired by WABs >>f*cktro<< - Take it if you like! \5 "
" \1 "
"Many many thanks to Marcus Comstedt who is a pioneer on the VMU and wrote "
"the emulator you see here!"
" \1 "
"Before we forget, this is the missing sinus scroller on the VMU demo. "
"Thanks for the sine values, WAB!!! :-)"
" \1 "
"No promises, but expect the VMU Emulator to load real games from Memory Stick in future."
" \1 "
"More info and updates here: http://virtuamunstaz.de/ \1 \1 \1";

char dbg[100] = "###########################";

int text_length, pos = 0, sinidx = 0, start_px = X2;
int scroller_length;

void debug(int x, int y, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	va_arg(ap, char *);
	va_arg(ap, char *);
	sprintf(dbg, fmt, ap);
	va_end(ap);
	put_str(x * 8, y * 8, dbg, 100, 0x000000, 0xffffff, 1);
}

void setup_scroller() {
	text_length = strlen(text);
	scroller_length = (X2 - X1) / 7 + 1;
}

void scroller() {
	SDL_FillRect(screen, &scroll_rect, 0xffffff);

	int c, fx, fy, px, py, sidx;
	int skipbits;
	
	px = start_px;
	skipbits = X1 - px;
	
	if(skipbits >= 8) {
		skipbits = 0;
		px = start_px = X1;
		pos++;
		if(pos > text_length) {
			pos = 0;
			px = start_px = X2;
		}
	}
	
	if(sinidx > 119) {
		sinidx = 0;
	}
	sidx = sinidx;

	for(c = 0; c < scroller_length; c++) {
		if(pos + c >= text_length) break;
		py = Y1;

#ifdef REAL_SINE_SCROLLER
		int ch = text[pos + c] * 8;
		for(fx = 0; fx < 8; fx++) {
			if(skipbits-- >= 0) continue;
			if(px + fx >= X2) break;
			
			for(fy = 0; fy < 8; fy++) {
				int bits = font[ch + fy];
				if((bits << fx) & 0x80) {
					if(sidx > 119) sidx = 0;
					putpixel(px + fx, fy + sine[sidx], 0x000000);
//					putpixel(px + fx, py + fy, 0x000000);
				}
			}
			sidx++;
		}
#else
		if(sidx > 119) sidx = 0;
		if(px >= X2) break;
		fx = put_char(px, sine[sidx] - 10, text[pos + c], 0x000000, 0xffffff, 8, skipbits, 0);
		if(skipbits > 0) skipbits = 0;
		sidx++;
#endif
		px += fx - 1;
	}
	sinidx++;
	start_px-=2;	
//	SDL_UpdateRect(screen, X1, Y1, X2 - X1, Y2 - Y1);
	SDL_Flip(screen);
}


int draw_pixel_row(int x, int y, int pixels, int fg, int bg, int skip, int mirror) {
	int bit;
	for(bit = 0; bit < 8; bit++) {
		if(skip-- > 0) continue;
		if(x + bit > 479) break;
		if(mirror) {
			if((pixels >> bit) & 0x01) {
				putpixel(x + bit, y, fg);
			} else {
				putpixel(x + bit, y, bg);
			}
		} else {
			if((pixels << bit) & 0x80) {
				putpixel(x + bit, y, fg);
			} else {
				putpixel(x + bit, y, bg);
			}
		}
	}
	return bit + 1;
}

int put_char(int x, int y, char c, int fg, int bg, int stretch, int skip, int mirror) {
	int char_start, row, rs, yoffset;
	char_start = c * 8;
	if(stretch < 1) stretch = 1;
	if(stretch > 8) stretch = 8;
	
	rs = 8 / stretch;
	y  -= stretch / 2;
	yoffset = 0;
	
	int char_width = 0;
	for(row = 0; row < 8; row++) {
		int pixels = font[char_start + row];
		char_width = draw_pixel_row(x, y + row + yoffset, pixels, fg, bg, skip, mirror);
		if(stretch > 1 &&!(row % rs)) {
			yoffset++;
			draw_pixel_row(x, y + row + yoffset, pixels, fg, bg, skip, mirror);
		}
	}
	return char_width; 
}

void put_str(int x, int y, char *s, int length, int fg, int bg, int stretch) {
	int i, width;
	for(i = 0; i < length && s[i] != 0; i++) {
		width = put_char(x, y, s[i], fg, bg, stretch, 0, 0);
		x+=width;
	}
}
