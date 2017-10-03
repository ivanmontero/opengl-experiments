#version 330 core

in GS_OUT {
	vec3 Normal;
	vec3 FragPos;
} ps_in;  

out vec4 color;

uniform vec3 viewPos;
uniform vec3 lightDir;

vec3 amb = vec3(.1, 0, 0);
vec3 diff = vec3(.3, 0, 0);

void main() {
    vec3 ld = -lightDir;
    float d = max(dot(ps_in.Normal, ld), 0.0);
    vec3 diffuse = diff * d;
	
	// TODO: Fix specular
	vec3 viewDir = normalize(viewPos - ps_in.FragPos);
    vec3 reflectDir = reflect(lightDir, ps_in.Normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec3 specular = vec3(.3) * spec;

    color = vec4(amb + diffuse + specular, 1.0);

	//if(ps_in.Normal.y > 0)
	//	color = vec4(1, 0, 0, 1);
	//else
	//	color = vec4(0, 1, 0, 1);
}