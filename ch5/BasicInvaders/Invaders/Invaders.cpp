/*
 * code based on standard OpenGL examples 
 * including OpenGL ES 2.0 Programming Guide
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <vector>

#ifdef RASPBERRY
#include "bcm_host.h"
#endif

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "MyFiles.h"
#include "surface.h"
#include "Objects.h"
#include "OpenGLESCode.h"
#include "Game.h"


typedef unsigned long Pixel;
	


Input* TheInput;


// from esUtil.h
#define TRUE 1
#define FALSE 0


int imageWidth;
int imageHeight;


GLuint framebufferTexID[2];

Pixel* Locations[2];
void* framedata = 0;
int bufferIndex = 0; // this is the working buffer


uint32_t scr_width, scr_height;
Surface* m_Screen;
std::vector<Objects*> MyObjects; // all our game objects are held here for rendering.

MyFiles* FileHandler;

// we're not really creating buffers
bool createFBtexture()
{
	
	int Red = 0xff0000ff;
	int Green = 0x00ff00ff;
	
	Locations[0] = (Pixel*)malloc(SCRWIDTH * SCRHEIGHT * 4);
	Locations[1] = (Pixel*)malloc(SCRWIDTH * SCRHEIGHT * 4);
	//set it all to black
	memset(Locations[0], 255, SCRWIDTH * SCRHEIGHT * 4);
	memset(Locations[1], 255, SCRWIDTH * SCRHEIGHT * 4);

	m_Screen = new Surface(SCRWIDTH, SCRHEIGHT, Locations[0]);
	m_Screen->SetPitch(SCRWIDTH);

	
}

bool swap()
{


	bufferIndex++;
	bufferIndex = bufferIndex % 2;
	glDeleteTextures(1, &framebufferTexID[bufferIndex]); //delete the old texture (displayed 1 frame ago)
	framebufferTexID[bufferIndex] = CreateSimpleTexture2D(SCRWIDTH, SCRHEIGHT, (char*)m_Screen->GetBuffer());
	m_Screen->SetBuffer(Locations[bufferIndex]);

}

CUBE_STATE_T state, *p_state = &state;


void  esMainLoop(CUBE_STATE_T *esContext)
{
	struct timeval t1, t2;
	struct timezone tz;
	float deltatime;
	float totaltime = 0.0f;
	unsigned int frames = 0;

	gettimeofday(&t1, &tz);
	static int ycoord = 0;

	Game TheGame; // make an instance of game
	TheGame.Init(FileHandler); // since init needs to load we pass the file handler

// make a temp objects so we can see it works
	Surface* TempObjectToSeeSomething = new Surface((char*)"../Assets/invaders/InvaderA-1.png", FileHandler);

	while (1)
	{

		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 0.0000001f);
		t1 = t2;

		m_Screen->ClearBuffer(0xff000000);

		TheGame.Update(deltatime, FileHandler, TheInput, m_Screen); // update the game objects
		TempObjectToSeeSomething->CopyAlphaPlot(m_Screen, 150, 150); // remove this once you have proper objects set up
		
		if (esContext->draw_func != NULL)
			esContext->draw_func(esContext);

		eglSwapBuffers(esContext->display, esContext->surface);

		

		if (TheInput->TestKey(KEY_A) != FALSE) // example of a key input
			printf("What is happening?\n");

		totaltime += deltatime;
		frames++;
		if (totaltime >  1.0f)
		{
			printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames / totaltime);
			totaltime -= 1.0f;
			frames = 0;
		}


	}
}



int main(int argc, char *argv[])
{
	UserData user_data;

	bcm_host_init(); 
	
	graphics_get_display_size(0,
			&scr_width,
			&scr_height); // gets the screen size

	TheInput = new Input();
	TheInput->Init();
	printf("Key input should be set up\n");

	FileHandler = new MyFiles();
	printf("filehandler set up\n");

	esInitContext(p_state);
	init_ogl(p_state, scr_width, scr_height); // sets up openGL with this screen size, but you can enter your own values

	createFBtexture();

	p_state->user_data = &user_data;

	if (!Init(p_state))	return 0;

	esRegisterDrawFunc(p_state, Draw);
	
	esMainLoop(p_state);

}
 