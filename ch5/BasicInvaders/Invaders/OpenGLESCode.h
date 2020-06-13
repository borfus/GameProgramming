#pragma once


#include <iostream>
#include <stdio.h>
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




typedef struct
{
    // Handle to a program object
	GLuint programObject;
		// Attribute locations
	GLint  positionLoc;
	GLint  texCoordLoc;

	// Sampler location
	GLint samplerLoc;

	// Texture handle
	GLuint textureId;
} UserData;


#define SIZETEX 512
typedef struct CUBE_STATE_T
{
	uint32_t width;
	uint32_t height; 

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;

	EGL_DISPMANX_WINDOW_T nativewindow;
	Surface* m_Screen;	


	
	void *user_data;
	void(*draw_func)(struct CUBE_STATE_T*);
	
} CUBE_STATE_T;


// define the routines we plan to use

	GLuint CreateSimpleTexture2D(int width, int height, char* TheData);
	void  esMainLoop(CUBE_STATE_T *esContext);

	void esRegisterDrawFunc(CUBE_STATE_T *p_state, void(*draw_func)(CUBE_STATE_T*));
	void esInitContext(CUBE_STATE_T *p_state);
	void init_ogl(CUBE_STATE_T *state, int width, int height);
	void Draw(CUBE_STATE_T *p_state);
	int Init(CUBE_STATE_T *p_state);

	GLuint LoadProgram(char *vertShaderSrc, char *fragShaderSrc);

	GLuint LoadShader(GLenum type, const char *shaderSrc);