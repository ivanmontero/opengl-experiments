#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 model;
uniform mat4 normalMatrix;

in VS_OUT {
	vec3 Position;
	vec3 FragPos;
} gs_in[];

out GS_OUT {
	vec3 Normal;
	vec3 FragPos;
} gs_out;

vec3 triangle_centroid(vec3 p1, vec3 p2, vec3 p3);

void main() {
    // Getting the normal
    vec3 A = gs_in[2].Position - gs_in[0].Position;
    vec3 B = gs_in[1].Position - gs_in[0].Position;
    vec3 norm = normalize(vec3(normalMatrix * vec4(cross(A, B), 1.0)));
	// TODO: fix frag pos and specular
	vec3 fragPos = triangle_centroid(gs_in[0].FragPos, gs_in[1].FragPos, gs_in[2].FragPos);

    for(int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        gs_out.Normal = norm;
		//gs_out.FragPos = gs_in[i].FragPos;
		gs_out.FragPos = fragPos;
        EmitVertex();
    }
    EndPrimitive();
    
}

vec3 triangle_centroid(vec3 p1, vec3 p2, vec3 p3) {
	return vec3(
		(p1.x + p2.x + p3.x) / 3.0f,
		(p1.y + p2.y + p3.y) / 3.0f,
		(p1.z + p2.z + p3.z) / 3.0f
	);
}