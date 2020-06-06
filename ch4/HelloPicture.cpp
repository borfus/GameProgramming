/* Hello Triangle
code adapted from OpenGL® ES 2.0 Programming Guide
and code snippets from RPI Forum to set up Dispmanx
*/

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include "bcm_host.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include "MyFiles.h"

#define TRUE 1
#define FALSE 0

typedef struct {
	//save a Handle to a program object
	GLuint programObject;
	// attribute locations
	GLint positionLoc;
	GLint texCoordLoc;

	// sampler location
	GLint samplerLoc;

	// texture handle
	GLuint textureId;
} UserData;

typedef struct Target_State {
	uint32_t width;
	uint32_t height;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	
	EGL_DISPMANX_WINDOW_T nativewindow;
	UserData *user_data;
	void(*draw_func) (struct Target_State*);
} Target_State;

Target_State state;
Target_State* p_state = &state;

static const EGLint attribute_list[] = {
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_NONE
};

static const EGLint context_attributes[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};

/* Create a texture with width and height*/
GLuint CreateTexture2D(int width, int height, char *data) {
	// texture handle
	GLuint textureId;
	// set alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// generate texture object
	glGenTextures(1, &textureId);
	// bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);
	// set it up
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data);

	if (glGetError() != GL_NO_ERROR) {
		printf("Oh bugger");
	}

	// set filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (glGetError() == GL_NO_ERROR) {
		return textureId;
	}

	printf("Oh bugger");
	return textureId;
}

/*
  Now we have be able to create a shader object, pass the shader source
  and then compile the shader.
*/
GLuint LoadShader(GLenum type, const char *shaderSrc) {
	// 1st create the shader object
	GLuint TheShader = glCreateShader(type);

	if (TheShader == 0) return 0; // can't allocate so stop.
	// pass the shader source
	glShaderSource(TheShader, 1, &shaderSrc, NULL);
	// Compile the shader
	glCompileShader(TheShader);

	GLint IsItCompiled;

	// After the compile we need to check the status and report any errors
	glGetShaderiv(TheShader, GL_COMPILE_STATUS, &IsItCompiled);
	if (!IsItCompiled) {
		GLint RetinfoLen = 0;
		glGetShaderiv(TheShader, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1) {
			// standard output for errors
			char* infoLog = (char*) malloc(sizeof(char) * RetinfoLen);
			glGetShaderInfoLog(TheShader, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling this shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(TheShader);
		return 0;
	}
	return TheShader;
}

// Initialize the shader and program object
int Init(Target_State *p_state) {
	p_state->user_data = (UserData*)malloc(sizeof(UserData));

	GLbyte vShaderStr[] =
		"attribute vec4 a_position;\n"
		"attribute vec2 a_texCoord;\n"
		"varying vec2 v_texCoord;\n"
		"void main()\n"
		"{gl_Position=a_position;\n"
		" v_texCoord=a_texCoord;}\n";

	GLbyte fShaderStr[] =
		"precision mediump float;\n"
		"varying vec2 v_texCoord;\n"
		"uniform sampler2D s_texture;\n"
		"void main() {\n"
		"gl_FragColor = texture2D(s_texture, v_texCoord);\n"
		"}\n";

	GLuint programObject, vertexShader, fragmentShader; // we need some variables

	// Load and compile the vertext/fragment shaders
	vertexShader = LoadShader(GL_VERTEX_SHADER, (char*)vShaderStr);
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, (char*)fShaderStr);

	// Create the program object
	programObject = glCreateProgram();
	if (programObject == 0) return 0;

	// now we have the V and F shaders  attach them to the program object
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);

	// Link the program
	glLinkProgram(programObject);
	// Check the link status
	GLint AreTheylinked;
	glGetProgramiv(programObject, GL_LINK_STATUS, &AreTheylinked);
	if (!AreTheylinked) {
		GLint RetintoLen = 0;
		// check and report any errors
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &RetintoLen);
		if (RetintoLen > 1) {
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * RetintoLen);
			glGetProgramInfoLog(programObject, RetintoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return FALSE;
	}

	// Store the program object
	p_state->user_data->programObject = programObject;

	// get the attribute locations
	p_state->user_data->positionLoc = glGetAttribLocation(p_state->user_data->programObject, "a_position");
	p_state->user_data->texCoordLoc = glGetAttribLocation(p_state->user_data->programObject, "a_texCoord");

	// get the sampler location
	p_state->user_data->samplerLoc = glGetUniformLocation(p_state->user_data->programObject, "s_texture");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return TRUE;
}

void init_ogl(Target_State* state, int width, int height) {
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	//RPI setup is a little different to normal EGL
	DISPMANX_ELEMENT_HANDLE_T DispmanElementH;
	DISPMANX_DISPLAY_HANDLE_T DispmanDisplayH;
	DISPMANX_UPDATE_HANDLE_T DispmanUpdateH;
	VC_RECT_T dest_rect;
	VC_RECT_T src_rect;
	EGLConfig config;

	// get an EGL display connection
    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	assert(state->display != EGL_NO_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(state->display, NULL, NULL);

	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
	assert(EGL_FALSE != result);

	// get an appropriate EGL frame buffer configuration
	result = eglBindAPI(EGL_OPENGL_ES_API);
	assert(EGL_FALSE != result);

	// create an EGL rendering context
	state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
	assert(state->context != EGL_NO_CONTEXT);

	//create an EGL window surface
	state->width = width;
	state->height = height;

	dest_rect.x = 0;
	dest_rect.y = 0;
	dest_rect.width = state->width; // it needs to know our window size
	dest_rect.height = state->height;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = state->width << 16;
	src_rect.height= state->height << 16;

	DispmanDisplayH = vc_dispmanx_display_open(0);
	DispmanUpdateH = vc_dispmanx_update_start(0);

	DispmanElementH = vc_dispmanx_element_add(
		DispmanUpdateH,
		DispmanDisplayH,
		0/*layer*/, 
		&dest_rect, 
		0/*source*/,
		&src_rect, 
		DISPMANX_PROTECTION_NONE,
		0/*alpha value*/, 
		0/*clamp*/, 
		(DISPMANX_TRANSFORM_T) 0/*transform*/
	);

	state->nativewindow.element = DispmanElementH;
	state->nativewindow.width = state->width;
	state->nativewindow.height = state->height;
	vc_dispmanx_update_submit_sync(DispmanUpdateH);
	state->surface = eglCreateWindowSurface(state->display, config, &(state->nativewindow), NULL);
	assert(state->surface != EGL_NO_SURFACE);
	// connect the context to the surface
	result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
	assert(EGL_FALSE != result);
}

/*****************************************
Draw a Rectangle with texture this is a hard coded
draw which is only good for the Rectangle
******************************************/
void Draw(Target_State* p_state) {
	GLfloat RectVertices[] = {
		-0.5f, 
		0.5f,
		0.0f, // Position 0
		0.0f,
		0.0f, // TexCoord 0
		-0.5f,
		-0.5f,
		0.0f, // Position 1
		0.0f,
		1.0f, // TexCoord 1
		0.5f,
		-0.5f,
		0.0f, // Position 2
		1.0f, 
		1.0f, // TexCoord 2
		0.5f, 
		0.5f,
		0.0f, // Position 3
		1.0f, 
		0.0f // TexCoord 3
	};

	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

	// setup the viewport
	glViewport(0, 0, p_state->width, p_state->height);
	// clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	// setup the program object
	glUseProgram(p_state->user_data->programObject);
	// load the vertex position
	glVertexAttribPointer(p_state->user_data->positionLoc,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		RectVertices);

	// load the texture coordinate
	glVertexAttribPointer(p_state->user_data->texCoordLoc,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(GLfloat),
		&RectVertices[3]);

	glEnableVertexAttribArray(p_state->user_data->positionLoc);
	glEnableVertexAttribArray(p_state->user_data->texCoordLoc);

	// bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_state->user_data->textureId);

	// actually draw the rect as 2 sets of 3 vertices (2 tris make a rect)
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

	if (glGetError() != GL_NO_ERROR) {
		printf("Oh bugger\n");
	}
}

void esInitContext(Target_State* p_state) {
	if (p_state != NULL) {
		memset(p_state, 0, sizeof(Target_State));
	}
}

void esRegisterDrawFunc(Target_State *p_state, void(*draw_func)(Target_State*)) {
	p_state->draw_func = draw_func;
}

void esMainLoop(Target_State* esContext) {
	while (TRUE) {
		if (esContext->draw_func != NULL) {
			esContext->draw_func(esContext);
		}
		// after our draw we need to swap buffers to display
		eglSwapBuffers(esContext->display, esContext->surface);
	}
}

int main(int argc, char *argv[]) {
	UserData user_data;
	bcm_host_init(); //RPI needs this
	esInitContext(p_state);

	uint width, height;
	int success = graphics_get_display_size(0, &width, &height);
	assert(success >= 0);

	init_ogl(p_state, width, height);
	p_state->user_data = &user_data;

	if (!Init(p_state)) {
		return 0;
	}
	esRegisterDrawFunc(p_state, Draw);

	MyFiles file_handler;
	int file_width, file_height;
	char* raw_data = file_handler.Load((char*)"../Assets/box_fan.png", &file_width, &file_height);
	if (raw_data == NULL) {
		printf("Failed to load image file.\n");
	}

	p_state->user_data->textureId = CreateTexture2D(file_width, file_height, raw_data);

	// now go do the graphic loop
	esMainLoop(p_state);
}
