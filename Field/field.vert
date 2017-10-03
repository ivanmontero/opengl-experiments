#version 330 core

layout (location = 0) in vec2 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float xPos;
uniform float zPos;
uniform float strMult;
uniform float amp;
uniform int mode;
uniform float scale;
uniform float exponent;

out float Shade;
//out vec3 Normal;
out vec3 Position;

float random(vec2 n);
float noise(vec2 p);
float snoise(vec2 v);
float cnoise(vec2 P);

void noisePlane();		// Classic Noise
void noisePlane2();		// Simplex Noise
void noisePlane3();		// Perlin Noise
void sinPlane();

void main() {
    if(mode == 0) 
        noisePlane();
    if(mode == 1)
        noisePlane2();
	if(mode == 2)
		noisePlane3();
	if(mode == 3)
		sinPlane();

    // Generate normal;
	//float stride = 1.0;
 //    vec3 dz;
 //    dz.x = position.x;
 //    dz.y = noise(vec2(position.x * strMult * stride, (position.z + 1) * strMult * stride));
 //    dz.z = position.z + 1;
 //    vec3 dx;
 //    dx.x = position.x + 1;
 //    dx.y = noise(vec2((position.x + 1) * strMult * stride, position.z * strMult * stride));
 //    dx.z = position.z;
 //    Normal = normalize(cross(dx - position, dz - position));

    // Sin curve
    // float amp = 2.0f;
    // float shift = 0.5f;
    // vec3 pos;
    // pos.x = position.x;
    // pos.y = amp * sin(time + position.x * shift);
    // pos.z = position.y;
    // gl_Position = projection * view * model * vec4(pos, 1.0);
    
    // Flat plane
    // gl_Position = projection * view * model * vec4(position.x, 0.0, position.y, 1.0);
}

void noisePlane() {
    // Perlin noise
    // float amp = 10.0f;
    // float strMult = .2f;
    // float lod = 1.0;
    float stride = 1.0; /// lod;
    vec3 pos;
    pos.x = position.x * scale + xPos;
    pos.z = position.y * scale + zPos;
    float n = noise(vec2(pos.x * strMult * stride, pos.z * strMult * stride));
    pos.y = amp * pow(n, exponent);			// N to power to create valleys
    Shade = n;
    gl_Position = projection * view * model * vec4(pos, 1.0f);
	Position = pos;
}

void noisePlane2() {
    // Perlin noise
    // float amp = 10.0f;
    // float strMult = .2f;
    // float lod = 1.0;
    float stride = 1.0; /// lod;
    vec3 pos;
    pos.x = position.x * scale + xPos;
    pos.z = position.y * scale + zPos;
    float n = .5 * snoise(vec2(pos.x * strMult * stride * .3, pos.z * strMult * stride * .3)) + .5;
    pos.y = amp * pow(n, exponent);
	//pos.y = amp * n;
    Shade = n;
    gl_Position = projection * view * model * vec4(pos, 1.0f);
	Position = pos;
}

void noisePlane3() {
    // Perlin noise
    // float amp = 10.0f;
    // float strMult = .2f;
    // float lod = 1.0;
    float stride = 1.0; /// lod;
    vec3 pos;
    pos.x = position.x * scale + xPos;
    pos.z = position.y * scale + zPos;
    float n = .5 * cnoise(vec2(pos.x * strMult * stride * .3, pos.z * strMult * stride * .3)) + .5;
    pos.y = amp * pow(n, exponent);
	//pos.y = amp * n;
    Shade = n;
    gl_Position = projection * view * model * vec4(pos, 1.0f);
	Position = pos;
}

void sinPlane() {
    // Sin curve
    //float amp = 2.0f;
    //float shift = 0.5f;
    vec3 pos;
    pos.x = position.x * scale;
    pos.y = amp * sin(time + position.x * strMult);
    pos.z = position.y * scale;
    gl_Position = projection * view * model * vec4(pos, 1.0);
	Position = pos;
    Shade = 1.0f;
}


// Classic
float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
        mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
        mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return res*res;
}

// Simplex
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

// Perlin
//	Classic Perlin 2D Noise 
//	by Stefan Gustavson
//
vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}

float cnoise(vec2 P){
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;
  vec4 i = permute(permute(ix) + iy);
  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
  vec4 gy = abs(gx) - 0.5;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;
  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);
  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
    vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;
  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));
  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}