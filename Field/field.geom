#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 Position[];
in float Shade[];

out vec3 Normal;
out float shade;

void main() {
	// Getting the normal
    vec3 A = Position[2] - Position[0];
    vec3 B = Position[1] - Position[0];
    vec3 norm = normalize(cross(A, B));

	for(int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        Normal = norm;
		shade = Shade[i];
        EmitVertex();
    }
    EndPrimitive();
}