   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "types.h"
#include "brlan.h"
#include "brlyt.h"

#define PLAYER_INIT_FLAGS	(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO)
#define PLAYER_VIDEO_FLAGS	(SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGLBLIT )
#define PLAYER_WIDTH		(640)
#define PLAYER_HEIGHT		(480)
#define PLAYER_BPP		(32)

int PLAYER_Done = 0;

SDL_Surface *screen = NULL;

void PLAYER_Finish()
{
	SDL_Quit();
}

void PLAYER_Draw()
{
	int ret = SDL_Flip(screen);
	if(ret < 0) {
		printf("Couldn't flip screen. Quitting.\n");
		exit(1);
	}
}

void PLAYER_HandleAnimation(BRLYT_object* objs, int objcount, BRLAN_animation* anims, int animcount)
{

}

void PLAYER_BlitObjects(BRLYT_object* objs, int objcount, SDL_Surface* scrn)
{

}

void PLAYER_Run()
{
	PLAYER_Done = 0;
	int fps = 60;
	int delay = 1000/fps;
	int thenTicks = -1;
	int nowTicks;

	BRLYT_object* objs;
	int objcount = BRLYT_ReadObjects(&objs);
	BRLAN_animation* anims;
	int animcount = BRLAN_ReadAnimations(&anims);
	
	while(!PLAYER_Done) {
		PLAYER_HandleAnimation(objs, objcount, anims, animcount);
		PLAYER_BlitObjects(objs, objcount, screen);
		PLAYER_Draw();

		// Draw at 60 hz
		if(thenTicks > 0) {
			nowTicks = SDL_GetTicks();
			delay += (1000 / fps) - (nowTicks - thenTicks);
			thenTicks = nowTicks;
			if (delay < 0)
				delay = 1000 / fps;
		} else
			thenTicks = SDL_GetTicks();

		SDL_Delay(delay);
	}
}

void PLAYER_Initialize()
{
	int ret = SDL_Init(PLAYER_INIT_FLAGS);
	if(ret < 0) {
		printf("Couldn't initialize SDL. Quitting.\n");
		exit(1);
	}
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	screen = SDL_SetVideoMode(PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_BPP, PLAYER_VIDEO_FLAGS);
	if(screen == NULL) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	s32 tmp;
	if(SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &tmp) < 0) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(tmp != 8) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &tmp) < 0) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(tmp != 8) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &tmp) < 0) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(tmp != 8) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &tmp) < 0) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(tmp != 8) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &tmp) < 0) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(tmp != 1) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &tmp) < 0) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	if(tmp != 32) {
		printf("Couldn't initialize SDL Video. Quitting.\n");
		exit(1);
	}
	
}

int main(int argc, char *argv[])
{
	if(argc != 2) {
		printf("Invalid arguments. Usage:\n\t%s <banner_bin_out>\n", argv[0]);
		return 1;
	}
	PLAYER_Initialize();
	BRLYT_Initialize(argv[1]);
	BRLAN_Initialize(argv[1]);

	PLAYER_Run();

	BRLAN_Finish();
	BRLYT_Finish();
	PLAYER_Finish();

	return 0;
}
































