#version 330

precision highp float;

#extension GL_EXT_shadow_samplers : require
#extension GL_NV_shadow_samplers_cube : enable

out vec4 FragColor;

/* varyings */
in vec2 vTexCoord;
in vec3 vTexCoord1;
in vec3 vViewDirection;
in vec3 vLightDirection;
in vec3 vNormal;

in vec4 vProjectiveCoord;

/* uniforms */
uniform sampler2D texture;
uniform sampler2D normalMap;

uniform sampler2DShadow shadowmap;
uniform samplerCube cubeMap;

uniform vec4 u_ambient;
uniform vec4 u_specular;
uniform vec4 u_diffuse;
uniform float u_specularPower;

//------------------------------
float makeShadow() {

   float shadowMap;
   float offset = 1.0 / 2048.0;

   vec4 crd = vProjectiveCoord;

   for (float y = -1.5 ; y <= 1.5 ; y += 1.0) {
        for (float x = -1.5 ; x <= 1.5 ; x += 1.0) {
			shadowMap += shadow2DProjEXT(shadowmap, crd + vec4(x * offset * crd.w, y * offset * crd.w, 0.0, 0.0));
		}
   }
   
   return (shadowMap / 16.0);
}

//------------------------------
void main() {
   
   float shadow = makeShadow();

   vec3 normal_map = normalize(((2.0 * texture2D(normalMap, vTexCoord).xyz) - 1.0));
 
   vec3  LightDirection = normalize( vLightDirection );
   vec3  Normal         = normalize( vNormal );
   float DotL           = clamp( dot( normal_map, LightDirection ), 0.0, 1.0 ); 
   
   vec3  Reflection     = normalize(  ( ( 2.0 * normal_map ) * ( DotL ) )  - LightDirection ); 
   vec3  ViewDirection  = normalize( vViewDirection );
   float DotV           = max( 0.0, dot( Reflection, ViewDirection ) );
   
   vec4 BaseColor    = vec4(shadow + 0.2) * ( texture2D(texture, vec2(1.0 - vTexCoord.x, 1.0 - vTexCoord.y) ) + textureCube(cubeMap, vec3(Reflection.xy, -Reflection.z)) ); 
   
   vec4 TotalAmbient   = u_ambient * BaseColor; 
   vec4 TotalDiffuse   = u_diffuse * DotL * BaseColor; 
   vec4 TotalSpecular  = u_specular * pow( DotV, u_specularPower ); 

   FragColor =  (TotalAmbient + TotalDiffuse + TotalSpecular );
}