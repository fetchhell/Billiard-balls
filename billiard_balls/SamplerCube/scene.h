#pragma once
#include <shader/shader.h>
#include <object/my_object.h>
#include <camera/my_camera.h>
#include <png/my_png.h>
#include <dds/my_dds.h>

/* screen width & height */
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

/* defines for textures */
#define HALF_FLOAT_OES 0x8D61
#define TEXTURE_COMPARE_MODE_EXT 0x884C
#define TEXTURE_COMPARE_FUNC_EXT 0x884D
#define COMPARE_REF_TO_TEXTURE_EXT 0x884E

/* shadow map size */
#define SHADOW_MAP_SIZE 2048

#define NUM_OF_BALLS   15
#define NUM_OF_OBJ     2
#define NUM_OF_SHADERS 4

typedef unsigned int ui;

class Scene {

	public:

	Scene(int screen_width, int screen_height): m_screen_width(screen_width), m_screen_height(screen_height) {};

	int getScreenWidth() { return m_screen_width; }
	int getScreenHeight() { return m_screen_height; }

	/* ------------ functions ----------- */

	/* init & setup VBO */
	void initVBO(GLuint buffer, object obj, GLvoid *data, GLuint size);
	void setupVBO(object *obj);
	
	/* rotate view vector */
	void rotate_view(vec3f *view, float angle, vec3f up);

	/* set filtering & wrap modes */
	void setTextureParam(png &tex);

	/* load vertex & fragment shaders */
	bool loadShader(string shader_name, int id);

	/* init scene*/
	bool init();

	/* render shadow map from light point */
	void RenderShadowMap();

	/* show depth texture in separate window */
	void showDepthTex(); 

	/* draw scene */
	void draw();

	/* events */
	void KeyboardEvent();
	void Mouse(int button, int state, int x, int y);
	void MouseEvent(int x, int y);

	private:
	
	/* width & height of the screen */
	int m_screen_width, m_screen_height; 

	/* model */
	object obj[NUM_OF_OBJ];
	object balls[NUM_OF_BALLS];


	/* depth texture screen */
	float *quad, *texQuad;

	/* shader */
	Shader shader[NUM_OF_SHADERS];

	/* camera */
	Camera camera, lightCamera, depthTexCamera;

	/* png texture */
	png texture[10];
	png balls_texture[NUM_OF_BALLS];

	/* dds texture */
	dds m_dds;

	/* texture handles */
	ui texture_handle[10];
	ui balls_texture_handle[NUM_OF_BALLS];

	/* framebuffer handles */
	ui frameBuffer_handle[1];

	/* renderbuffer handles */
	ui renderBuffer_handle[1];

	/* framebuffer texture2D attachment */
	ui frameBuffer_texture_attachment[1];
};