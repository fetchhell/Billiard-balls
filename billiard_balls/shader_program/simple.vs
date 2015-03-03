#version 330

/* attributes */
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

/* uniforms */
uniform	mat4 u_shadowViewProjection;
uniform	mat4 u_viewProjection;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform vec3 u_eyePosition;
uniform vec3 u_lightPosition;

/* varyings */
out vec2 vTexCoord; 
out vec3 vViewDirection;
out vec3 vLightDirection;
out vec3 vNormal;
out vec3 vTexCoord1;

out vec4 vProjectiveCoord;

void main() {

    mat3 model = mat3 ( u_model[0].xyz, u_model[1].xyz, u_model[2].xyz ) ;
    vec3 objectPosition = ( u_model * vec4(aPosition, 1.0) ).xyz;
	 
    vViewDirection = u_eyePosition - objectPosition;
    vLightDirection = u_lightPosition - objectPosition;

    vec3 normal   = model * aNormal;
    vec3 binormal = cross( vec3( 1.0, 0.0, 0.0 ), normal);
    vec3 tangent  = cross( normal, binormal );
  
    mat3 transform = mat3( tangent, binormal, normal );
    
    vViewDirection = transform * vViewDirection;
    vLightDirection = transform * vLightDirection; 

    vNormal = aNormal;
    vTexCoord   = aTexCoord;
    vTexCoord1  = aPosition;

    vec4 projective_coord = u_shadowViewProjection * u_model * vec4(aPosition, 1.0);
    gl_Position = u_viewProjection * u_model * vec4(aPosition, 1.0);

    mat4 biasMatrix = mat4(0.5, 0.0, 0.0, 0.0,
			   0.0, 0.5, 0.0, 0.0,
			   0.0, 0.0, 0.5, 0.0,
			   0.5, 0.5, 0.49999, 1.0);
    
    vProjectiveCoord = biasMatrix * projective_coord;
}