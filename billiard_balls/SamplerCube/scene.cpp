#include <include/includeAll.h>
#include "scene.h"

const float walk_speed = 5.0;
const float sensitivity = 0.0015;

//----------------------------
char* shader_program [] = { "simple", "shadow", "depthTex", "environment" };

//----------------------------
void Scene::initVBO(GLuint buffer, object obj, GLvoid *data, GLuint size) {

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size * obj.m_mesh.getNumFaces() * 3, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

//-----------------------------
void Scene::setupVBO(object *obj) {

	glGenBuffers(3, obj->VBO);
	
	initVBO(*obj->VBO, *obj, obj->m_mesh.getVertex(), sizeof(Lib3dsVector));
	initVBO(*(obj->VBO + 1), *obj, obj->m_mesh.getNormals(), sizeof(Lib3dsVector));
	initVBO(*(obj->VBO + 2), *obj, obj->m_mesh.getTexCoord(), sizeof(Lib3dsTexel));
}

//--------------------------
void Scene::KeyboardEvent() {

	/* handle keyboard event; translate camera */
	Camera *cam = &camera;

	if(GetAsyncKeyState('W')) {
		cam->m_eyePos -= cam->m_viewVec * walk_speed;  
	}

	if(GetAsyncKeyState('S')) {
	    cam->m_eyePos += cam->m_viewVec * walk_speed; 
	}

	if(GetAsyncKeyState('A')){
	   cam->m_eyePos.x -= cam->m_viewVec.z * walk_speed;
	   cam->m_eyePos.z += cam->m_viewVec.x * walk_speed;
	}

	if(GetAsyncKeyState('D')){
	   cam->m_eyePos.x += cam->m_viewVec.z * walk_speed;
	   cam->m_eyePos.z -= cam->m_viewVec.x * walk_speed;
	}

	glutPostRedisplay();
}

//--------------------------
void Scene::Mouse(int button, int state, int x, int y)
{
	/* set mouse position */
	Camera *cam = &camera;

	cam->m_mousePosition.x = x - (glutGet(GLUT_WINDOW_WIDTH) / 2);
	cam->m_mousePosition.y = y - (glutGet(GLUT_WINDOW_HEIGHT) / 2);

    glutPostRedisplay();
}

//--------------------------
void Scene::rotate_view(vec3f *view, float angle, vec3f up) {
	
    float new_x, new_y, new_z;

	float c = cos(angle);
	float s = sin(angle);

	new_x  = (up.x*up.x*(1-c) + c) * view->x;
	new_x += (up.x*up.y*(1-c) - up.z*s)	* view->y;
	new_x += (up.x*up.z*(1-c) + up.y*s)	* view->z;
	
	new_y  = (up.y*up.x*(1-c) + up.z*s)	* view->x;
	new_y += (up.y*up.y*(1-c) + c)	* view->y;
	new_y += (up.y*up.z*(1-c) - up.x*s)	* view->z;

	new_z  = (up.x*up.z*(1-c) - up.y*s)	* view->x;
	new_z += (up.y*up.z*(1-c) + up.x*s)	* view->y;
	new_z += (up.z*up.z*(1-c) + c)	* view->z;

	view->x = new_x;
	view->y = new_y;
	view->z = new_z;

	view->normalize();
}

//---------------------------
void Scene::MouseEvent(int x, int y) {

	/* handle mouse event; rotate camera */
	Camera *cam = &camera;
	vec3f rot_axis;

	int ww = glutGet(GLUT_WINDOW_WIDTH);
    int wh = glutGet(GLUT_WINDOW_HEIGHT);

	x -= (glutGet(GLUT_WINDOW_WIDTH) / 2);
	y -= (glutGet(GLUT_WINDOW_HEIGHT) / 2);

	int rot_x = (float)(x - cam->m_mousePosition.x);
	int rot_y = (float)(y - cam->m_mousePosition.y);

	cam->m_mousePosition.x = x;
	cam->m_mousePosition.y = y;

	rotate_view(&cam->m_viewVec, -rot_x * sensitivity, vec3f(0.0f, 1.0f, 0.0f));

	rot_axis = vec3f(-cam->m_viewVec.z, 0.0f, cam->m_viewVec.x);
	rot_axis.normalize();

	rotate_view(&cam->m_viewVec, -rot_y * sensitivity, rot_axis);
}

//---------------------------
void Scene::setTextureParam(png &tex) {

	glTexImage2D(GL_TEXTURE_2D, 0, tex.getFormat2(), tex.getWidth(), tex.getHeight(), 0, tex.getFormat2(), GL_UNSIGNED_BYTE, tex.getImageData());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex.config->getMinFilterMode());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex.config->getMagFilterMode());

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex.config->getWrapsMode());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex.config->getWraptMode());

	if(tex.config->generateMipmaps())
       glGenerateMipmap(GL_TEXTURE_2D);
}

//----------------------------
bool Scene::loadShader(string shader_name, int id) {

	string shaderPath("../shader_program/");
	shaderPath.append(shader_name);

	if(!shader[id].loadShader(string(shaderPath + ".vs").c_str(), 
		string(shaderPath + ".fs").c_str())) return false;
}

//----------------------------
bool Scene::init() {

	/* load shaders */
	for(int i = 0; i < NUM_OF_SHADERS; i++)
		loadShader(shader_program[i], i);

	/* load 3ds models */
	obj[0].m_mesh.loadModel("../objects/Sphere.3ds");
	obj[1].m_mesh.loadModel("../objects/Disc.3ds");

	for(int i = 0; i < NUM_OF_BALLS; i++) {

		char filename[256];
		sprintf(filename, "../objects/ball%d.3DS", i + 1);
		balls[i].m_mesh.loadModel(filename);
		balls[i].rotateX(-90);
	}

	/* build camera projection matrix */
	camera.buildFrustrumMatf(getScreenWidth(), getScreenHeight(), 2.f, 10000.f);

	/* set view vector & eye position */
	camera.m_eyePos = vec3f(81.02f, 190.109f, -77.094f); 
	camera.m_viewVec= camera.m_eyePos - vec3f(0,0,-1);
	camera.m_viewVec.normalize();

	/* build light camera view & projection matrices */
	lightCamera.buildFrustrumMatf(getScreenWidth(), getScreenHeight(), 2.f, 10000.f);
	lightCamera.buildViewMatf(vec3f(0, 250, -1000.0f), vec3f(0, 250, -1), vec3f(0,1,0));
	lightCamera.buildViewProjection();
	
	/* scale & translate objects */
	obj[1].translateObject(vec3f(0.f, -10.f, 0.f));
	obj[1].scaleObject(vec3f(2.f, 1.f, 2.f));

	/* generate the VBO object */
	for(int i = 0; i < NUM_OF_BALLS; i++) 
		setupVBO(&balls[i]);

	for(int i = 0; i < NUM_OF_OBJ; i++)
		setupVBO(&obj[i]);

	/* load textures */
	for(int i = 0; i < NUM_OF_BALLS; i++) {

		char filename[256];
		sprintf(filename, "../textures/ball%d.png", i + 1);

		if(balls_texture[i].png_texture_load(filename)) {

			balls_texture[i].config = new Config(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, true);
		
			glGenTextures(1, &balls_texture_handle[i]);
			glBindTexture(GL_TEXTURE_2D, balls_texture_handle[i]);

			setTextureParam(balls_texture[i]);
		}
	}

	if(texture[0].png_texture_load("../textures/table_2.png")) {

		texture[0].config = new Config(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, true);
		
		glGenTextures(1, &texture_handle[0]);
		glBindTexture(GL_TEXTURE_2D, texture_handle[0]);

		setTextureParam(texture[0]);
	}

	if(texture[1].png_texture_load("../textures/normal_2.png")) {

		texture[1].config = new Config(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, true);
		
		glGenTextures(1, &texture_handle[1]);
		glBindTexture(GL_TEXTURE_2D, texture_handle[1]);

		setTextureParam(texture[1]);
	}

	if(texture[3].png_texture_load("../textures/normal_billiard.png")) {

		texture[3].config = new Config(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, true);
		
		glGenTextures(1, &texture_handle[3]);
		glBindTexture(GL_TEXTURE_2D, texture_handle[3]);

		setTextureParam(texture[3]);
	}

	if(m_dds.dds_texture_load("../textures/Snow.dds")) {

		glGenTextures(1, &texture_handle[2]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_handle[2]);

		GLenum cur_format = m_dds.getFormat2();

		if(cur_format == GL_RGB || cur_format == GL_RGBA)
		{
			for(int i = 0; i < 6; i++)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cur_format, m_dds.getWidth(i), 
							 m_dds.getHeight(i), 0, cur_format, GL_UNSIGNED_BYTE, m_dds.getImageData(i));
		}
		else
		{
			for(int i = 0; i < 6; i++)
				glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cur_format, m_dds.getWidth(i), 
				                       m_dds.getHeight(i), 0, m_dds.getImageSize(i), m_dds.getImageData(i));
		}

		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	/* init framebuffer */
	glGenFramebuffers(1, &frameBuffer_handle[0]);
	glGenRenderbuffers(1, &renderBuffer_handle[0]);
	glGenTextures(1, &frameBuffer_texture_attachment[0]);
	
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_handle[0]);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_handle[0]);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

	glBindTexture(GL_TEXTURE_2D, frameBuffer_texture_attachment[0]);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); 
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 

	glTexParameteri(GL_TEXTURE_2D, TEXTURE_COMPARE_MODE_EXT, COMPARE_REF_TO_TEXTURE_EXT);
    glTexParameteri(GL_TEXTURE_2D, TEXTURE_COMPARE_FUNC_EXT, GL_LEQUAL);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0); 
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBuffer_texture_attachment[0], 0); 

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

//----------------------------
void Scene::RenderShadowMap() {

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_handle[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUseProgram(shader[1].getProgramHandle());

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader[1].setUniform("u_shadowViewProjection", lightCamera.m_viewProjection);

	for(int i = 0; i < NUM_OF_BALLS; i++) {

		shader[1].setUniform("u_model", balls[i].m_model);
		shader[1].drawScene(balls[i], balls[i].VBO);
	}

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//----------------------------
void Scene::draw() {

	KeyboardEvent();
	
	/* render shadowmap into framebuffer (to attached texture) */
	RenderShadowMap();
	
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	/* draw scene */
	glUseProgram(shader[0].getProgramHandle());

	/* set textures */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameBuffer_texture_attachment[0]);
	glTexParameteri(GL_TEXTURE_2D, TEXTURE_COMPARE_MODE_EXT, COMPARE_REF_TO_TEXTURE_EXT);
	glUniform1i(glGetUniformLocation(shader[0].getProgramHandle(), "shadowmap"), 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_handle[1]);
	glUniform1i(glGetUniformLocation(shader[0].getProgramHandle(), "normalMap"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_handle[2]);
	glUniform1i(glGetUniformLocation(shader[0].getProgramHandle(), "cubeMap"), 3);

	/* set uniforms */
	camera.buildViewMatf(camera.m_eyePos, camera.m_eyePos - camera.m_viewVec, vec3f(0.0f, 1.0f, 0.0f));

	camera.buildViewProjection();

	shader[0].setUniform("u_shadowViewProjection", lightCamera.m_viewProjection);
	shader[0].setUniform("u_viewProjection", camera.m_viewProjection);

	shader[0].setUniform("u_projection", camera.m_projection);
	shader[0].setUniform("u_view", camera.m_view);

	float eyePos[] = { camera.m_eyePos.x, camera.m_eyePos.y, camera.m_eyePos.z }; 
	shader[0].setUniform("u_eyePosition", eyePos); 

	float lightPosition[] = { 0, 250, -1000.0 };
	shader[0].setUniform("u_lightPosition", lightPosition);

	float specularPower  = 25.f;
	float ambient[]      = { 0.36f, 0.36f, 0.36f, 1.0f };
	float diffuse[]      = { 0.7f, 0.7f, 0.7f, 1.0f };
	float specular[]     = { 1.0f, 1.0f, 1.0f, 1.0f }; 

	shader[0].setUniform("u_ambient", ambient);
	shader[0].setUniform("u_diffuse", diffuse);
	shader[0].setUniform("u_specular", specular);
	shader[0].setUniform("u_specularPower", specularPower);

	NvBuildIdentityMatf(camera.m_cameraRotateXY);
	NvBuildIdentityMatf(camera.m_cameraTranslate);

	/* draw balls */
	for(int i = 0; i < NUM_OF_BALLS; i++) {

		/* for each ball we have separate texture */
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, balls_texture_handle[i]);
		glUniform1i(glGetUniformLocation(shader[0].getProgramHandle(), "texture"), 1);

		shader[0].setUniform("u_model", balls[i].m_model);

		shader[0].drawScene(balls[i], balls[i].VBO);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_handle[0]);
	glUniform1i(glGetUniformLocation(shader[0].getProgramHandle(), "texture"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture_handle[3]);
	glUniform1i(glGetUniformLocation(shader[0].getProgramHandle(), "normalMap"), 2);

	shader[0].setUniform("u_model", obj[1].m_model);

	/* draw plane */
	shader[0].drawScene(obj[1], obj[1].VBO);

	/* draw environment */
	glUseProgram(shader[3].getProgramHandle());
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_handle[2]);
	
	shader[3].setUniform("u_viewProjection", camera.m_viewProjection);
	shader[3].drawScene(obj[0], obj[0].VBO);
	
	/* swap front and back buffers */
	glutSwapBuffers();
}