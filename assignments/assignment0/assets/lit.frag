#version 450
out vec4 FragColor; //The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space
	vec2 TexCoord;
	mat3 TBN;

}fs_in;


uniform sampler2D _MainTex; 
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
uniform sampler2D normalMap;
struct Material{
	float Ka; //Ambient coefficient (0-1)
	float Kd; //Diffuse coefficient (0-1)
	float Ks; //Specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;

void main(){
    
	//sample from normal map, put in -1 to one, multiply w tbn matrix to transform from tangent space to world space
	//Make sure fragment normal is still length 1 after interpolation.
	//vec3 normal = normalize(fs_in.WorldNormal);
	//places to check: model loading code (making sure tangents are set (mesh load function making sure attribute is sent)) 
	//
	vec3 normal = texture(normalMap,fs_in.TexCoord).rgb;
	
	normal = normalize(normal*2.0-1.0);
	normal =normalize(fs_in.TBN*normal);
	//Light pointing straight down
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal,toLight),0.0);
	//Calculate specularly reflected light
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	//Blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	//Combination of specular and diffuse reflection
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	lightColor+=_AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	
	FragColor = vec4(objectColor * lightColor,1.0);//vec4(normal,1.0);
}
