#version 330 core

in vec3 Normal;
in float shade;

uniform vec3 lightDir;
uniform int flatShade;

out vec4 color;

vec3 amb = vec3(.1);
vec3 diff = vec3(.8);

void main() {
	if(flatShade != 0) {
		/**************** LOW POLY ********************/
		vec3 ld = -lightDir;
		float d = max(dot(Normal, ld), 0.0);
		vec3 diffuse = diff * d;
	
		color = vec4(amb + diffuse, 1.0);
	} else {
		/**************** SHADE ********************/
		float percent = (shade + 1.0f) / 2.0f;
		color = vec4(percent, percent, percent, 1.0f);
	}

	//vec3 ambient = .1;
	//vec3 norm - normalize(Normal);
	//vec3 light(-0.2f, -1.0f, -0.3f);
	//vec3 lightDir = normalize(-light);
	//float diff = max(dot(norm, lightDir),0.0);
	//vec3 diffuse =  1.0 * diff;

	//color = vec4(ambient + diffuse, 1.0f);
    
    // if(Noise < .05) color = vec4(0.0, 0.0, 1.0, 1.0f);
    // else           color = vec4(1.0, 0.0, 0.0, 1.0f);
    
}