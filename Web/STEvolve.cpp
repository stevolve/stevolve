
// STEvolve.cpp : Defines the class behaviors for the application.
//

#include <iostream>

#include <emscripten.h>
#include <SDL/SDL.h>

#include "portable/Settings.h"
#include "portable/CellBase.h"
#include "portable/CellProgram.h"
#include "portable/WorldBase.h"
#include "portable/WorldToroidal.h"

World *pWorld = NULL;
bool gbRefreshStop = true;
bool gbThreadStop = false;

SDL_Surface *screen;

int main(int argc, const char * argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(giWorldWidth, giWorldHeight, 32, SDL_SWSURFACE);

//	SDL_Quit();

	pWorld = new WorldToroidal();
	int ret = pWorld->Start();
}

#define S(o,n)r[t[int(h[0])/60*3+o]+o-2]=(n+h[2]-c/2)*255;
void C(float*h,int*r){float g=2*h[2]-1,c=(g<0?1+g:1-g)*h[1],a=int(h[0])%120/60.f-1;int t[]={2,2,2,3,1,2,3,3,0,4,2,0,4,1,1,2,3,1};S(0,c)S(1,c*(a<0?1+a:1-a))S(2,0)}

void SetPixelRGB(int x, int y, unsigned long rgb)
{
/*	if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	*((Uint32 *)screen->pixels + x * 256 + y) = SDL_MapRGBA(screen->format, x, y, 255-x, 255);
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);*/
}

void UpdateDisplay()
{
	if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	for (int i = 0; i < giWorldHeight; i++)
		for (int j = 0; j < giWorldWidth; j++)
		{
			//*((Uint32 *)screen->pixels + i * 256 + j) = SDL_MapRGBA(screen->format, rand() % 255, rand() % 255, rand() % 255, 255);
			int rgb[3];
			float hls[3];
			hls[0] = pWorld->water[j][i]->wMyColor;
			hls[1] = 240;
			hls[2] = __min(pWorld->water[j][i]->energy, 0xFFFF) / 273;
//			C(hls, rgb);
			DrawOpcode(pWorld->water[j][i], rgb);
			*((Uint32 *)screen->pixels + i * giWorldWidth + j) = SDL_MapRGBA(screen->format, rgb[0], rgb[1], rgb[2], 255);
		}
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	emscripten_sleep(10);
	//requestAnimationFrame();
	//emscripten_sleep_with_yield(1000);
}