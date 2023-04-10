#version 330 core
layout(location = 0) out vec4 color;

uniform sampler2D tex;
uniform float strokeWeight;
uniform float blotchiness;

in vec3 normal;
in vec3 diffuse;
in vec3 specular;
in vec3 ambientVec;
in vec3 texColor;
in vec2 texCoord;


float noise(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

float fractalNoise(vec2 p)
{
		float numOctaves = 3;
		float persistence = 0.5;
    float total = 0.0;
    float frequency = 1.0;
    float amplitude = 1.0;

    for (int i = 0; i < numOctaves; i++) {
        total += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= persistence;
    }

    return total;
}


void main(){ 

	// vec2 noisyTexCoord = texCoord + blotchiness * vec2(noise(texCoord * 500.0), noise(texCoord * 250.0));
	vec2 noisyTexCoord = texCoord + blotchiness * fractalNoise(texCoord * 500.0);

	// vec4 texColor = texture(tex, noisyTexCoord);


	vec4 texColor = texture(tex, texCoord);

  vec4 col1 = vec4(0.4, 0.6, 0.8, 1.0);
  vec4 col2 = vec4(0.2, 0.4, 0.6, 1.0);

  // vec4 mixColor = mix(col1, col2, texColor.r);

  float y = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));

  vec3 waterColor = vec3(1.0, 0.9, 0.7) * vec3(1.0 - y) + vec3(0.0, 0.1, 0.3) * vec3(y);

	// color = vec4(waterColor, 1.0f);

	// vec3 diffuse_tex = texture2D(tex, texCoord).rgb;
	// vec3 diffuse_tex = texture2D(tex, texCoord).rgb;
  float strokeFactor = strokeWeight * max(0.0, 1.0 - 2.0 * length(texCoord - vec2(0.5)));

	// vec3 result = (ambientVec + diffuse + specular) * waterColor * (1.0 - strokeFactor) + strokeFactor * vec3(1.0, 1.0, 1.0);
	vec3 result = (ambientVec + diffuse) * waterColor * (1.0 - strokeFactor) + strokeFactor * vec3(1.0, 1.0, 1.0);
	color = vec4(result, 1.0f);
	// color = vec4(waterColor, 1.0f);
	//color = vec4(specular + texColor * (diffuse_tex * (diffuse + ambientVec)) * waterColor, 1);

}