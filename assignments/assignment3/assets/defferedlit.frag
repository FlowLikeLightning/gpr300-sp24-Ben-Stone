#version 450
in vec2 UV; //From fsTriangle.vert
out vec4 FragColor; //The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space
	vec2 TexCoord;
}fs_in;
uniform layout(binding = 0) sampler2D _gPositions;
uniform layout(binding = 1) sampler2D _gNormals;
uniform layout(binding = 2) sampler2D _gAlbedo;
struct PointLight{
	vec3 position;
	float radius;
	vec3 color;
};
#define MAX_POINT_LIGHTS 64
uniform PointLight _PointLights[MAX_POINT_LIGHTS];

uniform sampler2D _MainTex; 
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
in vec4 LightSpacePos;
uniform float minBias = 0.005; //Example values! 
uniform float maxBias = 0.015;
uniform sampler2D _ShadowMap;
float bias; 
struct Material{
	float Ka; //Ambient coefficient (0-1)
	float Kd; //Diffuse coefficient (0-1)
	float Ks; //Specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;
float calcShadow(sampler2D shadowMap, vec4 lightSpacePos,float bias)
{
	//Homogeneous Clip space to NDC [-w,w] to [-1,1]
    vec3 sampleCoord = lightSpacePos.xyz / lightSpacePos.w;
    //Convert from [-1,1] to [0,1]
    sampleCoord = sampleCoord * 0.5 + 0.5;
	float myDepth = sampleCoord.z- bias; 
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;
	//step(a,b) returns 1.0 if a >= b, 0.0 otherwise
	float totalShadow = 0;
	vec2 texelOffset = 1.0 /  textureSize(_ShadowMap,0);
	for(int y = -1; y <=1; y++)
	{
		for(int x = -1; x <=1; x++)
		{
			vec2 uv = sampleCoord.xy + vec2(x * texelOffset.x, y * texelOffset.y);
			totalShadow+=step(texture(_ShadowMap,uv).r,myDepth);
		}
	}
	totalShadow/=9.0;

	
	return totalShadow;

}
vec3 calcLighting(vec3 normal,vec3 worldpos,vec3 albedo)
{
vec3 toLight = -_LightDirection;
	bias = max(maxBias * (1.0 - dot(normal,toLight)),minBias);	
	float shadow = calcShadow(_ShadowMap, LightSpacePos,bias);
	float diffuseFactor = max(dot(normal,toLight),0.0);
	//Calculate specularly reflected light
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	//Blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	//Combination of specular and diffuse reflection
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	lightColor*=1-shadow;
	lightColor+=_AmbientColor * _Material.Ka;
	return lightColor;
}
float attenuateLinear(float distance, float radius){
	return clamp((radius-distance)/radius,0.0,1.0);
}
float attenuateExponential(float distance, float radius){
	float i = clamp(1.0 - pow(distance/radius,4.0),0.0,1.0);
	return i * i;
	
}

vec3 calcPointLight(PointLight light,vec3 normal,vec3 pos){
	vec3 diff = light.position - pos;
	//Direction toward light position
	vec3 toLight = normalize(diff);
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 h = normalize(toLight + toEye);
	float diffuseFactor = max(dot(normal,toLight),0.0);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	//TODO: Usual blinn-phong calculations for diffuse + specular
	vec3 lightColor = (diffuseFactor + specularFactor) * light.color;
	//Attenuation
	float d = length(diff); //Distance to light
	lightColor*=attenuateLinear(d,light.radius); //See below for attenuation options
	return lightColor;
}
void main(){


	vec3 normal = texture(_gNormals,UV).xyz;
	vec3 worldPos = texture(_gPositions,UV).xyz;
	vec3 albedo = texture(_gAlbedo,UV).xyz;
	//Worldspace lighting calculations, same as in forward shading
	vec3 lightColor = calcLighting(normal,worldPos,albedo);
	for(int i=0;i<MAX_POINT_LIGHTS;i++){
	lightColor+=calcPointLight(_PointLights[i],normal,worldPos);
	}

	FragColor = vec4(albedo * lightColor,1.0);
}
