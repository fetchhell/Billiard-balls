#version 330

/* attributes */
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

/* uniforms */
uniform	mat4 u_viewProjection;
uniform mat4 u_model;

/*varyings*/
out vec3 vTexCoord;

void main() {

   gl_Position = u_viewProjection * vec4(aPosition * 100.0, 1.0);
   vTexCoord = vec3(aPosition.x, aPosition.y, -aPosition.z);
}