#version 330

#extension GL_NV_shadow_samplers_cube : enable

out vec4 FragColor;

/* uniforms */
uniform samplerCube cubeMap;

/* varyings */
in vec3 vTexCoord;

void main() {

   FragColor = textureCube(cubeMap, vTexCoord);
}