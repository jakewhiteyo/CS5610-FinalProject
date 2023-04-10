#version 330 core
layout(location = 0) out vec4 color;

uniform sampler2D tex;

in vec3 normal;
in vec3 diffuse;
in vec3 specular;
in vec3 ambientVec;
in vec3 potColor;
in vec2 texCoord;

void main(){ 
	vec3 diffuse_tex = texture2D(tex, texCoord).rgb;
	color = vec4(specular + potColor * (diffuse_tex * (diffuse + ambientVec)), 1);

}