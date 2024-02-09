#version 450
out vec4 FragColor;
in vec2 UV;
uniform sampler2D _ColorBuffer;

void main(){
	vec3 color = texture(_ColorBuffer,UV).rgb;
	float average = (color.r+color.g+color.b)/3.0f;//GRAY SCALING!

	FragColor = vec4(average,average,average,1.0);
	//FragColor = vec4(color,1.0);
}
