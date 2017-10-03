#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <set>
#include <algorithm>

#include "shader.h"
#include "camera.h"

#define PI 3.14159265358979f

GLint WIDTH = 800, HEIGHT = 600;
const float STR_MUL = .2f;
const int DEFAULT_WIDTH = 100, DEFAULT_DEPTH = 100;

int field_width = DEFAULT_WIDTH, field_depth = DEFAULT_DEPTH;

bool keys[1024];
bool firstMouse = true;
bool flat = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
GLfloat fov = PI / 3.0f;
float strMult = .2f, amp = 10.0f;
int lod = 1, planeVertices, icosVertices;
float icosScale = 3;
glm::mat4 projection = glm::perspective(PI / 3.0f, (float)WIDTH / HEIGHT, 0.01f, 100.0f);
GLuint planeIndices = 0, icosIndices = 0;
GLuint planeVAO, planeVBO, planeEBO;
GLuint icosVAO, icosVBO, icosEBO;
GLFWwindow* window;
bool fill = false;
int mode;
float scale = 1.0f, exponent = 1.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyboard_actions();
void create_plane(GLuint VAO, GLuint VBO, GLuint EBO);
void create_icosahedron(GLuint VAO, GLuint VBO, GLuint EBO, int subdivisionLevel);
void subdivide_triangle(int vi0, int vi1, int vi2, std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, int subLevel);
glm::vec3 midpoint(glm::vec3 p1, glm::vec3 p2);
//glm::vec3 centroid(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Tutorial", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);	// VSync

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);

	Shader planeShader("field.vert", "field.geom", "field.frag");
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);

	create_plane(planeVAO, planeVBO, planeEBO);

	//Shader icosShader("icos.vert", "icos.geom", "icos.frag");
	Shader icosShader("icos.vert", "icos.geom", "icos.frag");
	glGenVertexArrays(1, &icosVAO);
	glGenBuffers(1, &icosVBO);
	glGenBuffers(1, &icosEBO);

	create_icosahedron(icosVAO, icosVBO, icosEBO, 1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glCullFace(GL_CW);

	float time = 0;
	mode = 0;

	Camera::SetPosition(0.0f, 20.0f, 10.0f);

	glm::vec3 lightDir(-.2f, -1.0f, -.3f);

	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check and call events
		glfwPollEvents();
		keyboard_actions();
		// Rendering commands
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		planeShader.Use();

		time += deltaTime;
		//model = glm::translate(model, glm::vec3(-field_width / 2, 0, -field_depth / 2));
		//model = glm::scale(model, glm::vec3(1.0f / 2, 1.0f, 1.0f / 2 ));
		glm::mat4 model;
		glm::mat4 view = Camera::GetViewMatrix();
		glm::mat4 projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);

		glUniform1f(glGetUniformLocation(planeShader.Program, "time"), time);
		glUniform1f(glGetUniformLocation(planeShader.Program, "xPos"), Camera::GetX());
		glUniform1f(glGetUniformLocation(planeShader.Program, "zPos"), Camera::GetZ());
		glUniform1f(glGetUniformLocation(planeShader.Program, "strMult"), strMult);
		glUniform1f(glGetUniformLocation(planeShader.Program, "amp"), amp);
		glUniform1i(glGetUniformLocation(planeShader.Program, "mode"), mode);
		glUniform1i(glGetUniformLocation(planeShader.Program, "flatShade"), flat);
		glUniform1f(glGetUniformLocation(planeShader.Program, "scale"), scale);
		glUniform1f(glGetUniformLocation(planeShader.Program, "exponent"), exponent);
		glUniform3f(glGetUniformLocation(planeShader.Program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
		glUniformMatrix4fv(glGetUniformLocation(planeShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(planeShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(planeShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(planeVAO);
		glDrawElements(GL_TRIANGLES, planeIndices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		icosShader.Use();

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 20.0f, 0.0f));
		//model = glm::rotate(model, time, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::scale(model, glm::vec3(icosScale, icosScale, icosScale));

		glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));

		glUniformMatrix4fv(glGetUniformLocation(icosShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(icosShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(icosShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(icosShader.Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniform3f(glGetUniformLocation(icosShader.Program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
		glUniform3f(glGetUniformLocation(icosShader.Program, "viewPos"), Camera::GetX(), Camera::GetY(), Camera::GetZ());
		glUniform1f(glGetUniformLocation(icosShader.Program, "time"), time);
		glUniform1i(glGetUniformLocation(icosShader.Program, "lod"), lod);

		glBindVertexArray(icosVAO);
		glDrawElements(GL_TRIANGLES, icosIndices, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// Close window on escape key
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		fill ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		fill = !fill;
	}
	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
		flat = !flat;
	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
		if (icosScale > .5)
			icosScale -= .5;
	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
		icosScale += .5;

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = lastX - xpos;
	GLfloat yoffset = lastY - ypos;	// Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.0025;// *deltaTime;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Camera::RotateYaw(xoffset);
	Camera::RotatePitch(yoffset);

	if (Camera::GetPitch() > PI / 2.0f - 0.01f) Camera::SetPitch(PI / 2.0f - 0.01f);
	if (Camera::GetPitch() < -PI / 2.0f + 0.01f) Camera::SetPitch(-PI / 2.0f + 0.01f);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (yoffset < 0) {
		lod--;
		if (lod < 1)
			lod = 1;
	}
	else {
		lod++;
	}
	create_plane(planeVAO, planeVBO, planeEBO);
	create_icosahedron(icosVAO, icosVBO, icosEBO, lod);
	//std::cout << lod << std::endl;
	//std::cout << field_depth << std::endl;
	//std::cout << field_width << std::endl;
	std::cout << "vertices: (plane) " << planeVertices << " + (icos) " << icosVertices << std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
}

void keyboard_actions() {
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_LEFT_SHIFT]) cameraSpeed *= 2.0f;
	if (keys[GLFW_KEY_W]) Camera::Move(Camera::GetFront() * cameraSpeed);
	if (keys[GLFW_KEY_S]) Camera::Move(-Camera::GetFront() * cameraSpeed);
	if (keys[GLFW_KEY_A]) Camera::Move(-Camera::GetRight() * cameraSpeed);
	if (keys[GLFW_KEY_D]) Camera::Move(Camera::GetRight() * cameraSpeed);
	if (keys[GLFW_KEY_SPACE]) Camera::Move(Camera::Y_AXIS * cameraSpeed);
	if (keys[GLFW_KEY_LEFT_CONTROL]) Camera::Move(Camera::Y_AXIS * -cameraSpeed);

	//if (keys[GLFW_KEY_UP]) Camera::Rotate(Camera::PITCH, 1.0f * cameraSpeed / 5.0f);
	//if (keys[GLFW_KEY_DOWN]) Camera::Rotate(Camera::PITCH, -1.0f * cameraSpeed / 5.0f);
	//if (keys[GLFW_KEY_RIGHT]) Camera::Rotate(Camera::YAW, -1.0f * cameraSpeed / 5.0f);
	//if (keys[GLFW_KEY_LEFT]) Camera::Rotate(Camera::YAW, 1.0f * cameraSpeed / 5.0f);

	if (keys[GLFW_KEY_RIGHT]) strMult += .001;
	if (keys[GLFW_KEY_LEFT]) strMult -= .001;
	if (keys[GLFW_KEY_UP]) amp += .05;
	if (keys[GLFW_KEY_DOWN]) amp -= .05;
	if (keys[GLFW_KEY_EQUAL]) scale += .01;
	if (keys[GLFW_KEY_MINUS]) scale -= .01;
	if (keys[GLFW_KEY_PERIOD]) exponent += .005;
	if (keys[GLFW_KEY_COMMA]) exponent -= .005;
	if (keys[GLFW_KEY_1]) mode = 0;
	if (keys[GLFW_KEY_2]) mode = 1;
	if (keys[GLFW_KEY_3]) mode = 2;
	if (keys[GLFW_KEY_4]) mode = 3;
	if (keys[GLFW_KEY_BACKSPACE]) {
		strMult = .2f;
		amp = 10.0f;
		lod = 1;
		icosScale = 3;
		scale = 1.0f;
		exponent = 1.0f;
	}
}

void create_plane(GLuint VAO, GLuint VBO, GLuint EBO) {
	field_depth = DEFAULT_DEPTH * lod;
	field_width = DEFAULT_WIDTH * lod;
	//Create vertices
	std::vector<GLfloat> vertices;
	//int field[field_depth][field_width];
	for (int z = -field_depth/2; z < field_depth/2; z++) {
		for (int x = -field_depth/2; x < field_width/2; x++) {
			//field[z][x] = z * field_depth + x;
			vertices.push_back(float(x) / float(lod));
			vertices.push_back(float(z) / float(lod));
		}
	}
	//for (int z = 0; z < field_depth; z++) {
	//	for (int x = 0; x < field_width; x++) {
	//		vertices.push_back(x);
	//		//vertices.push_back(0);
	//		vertices.push_back(z);
	//	}
	//}
	/*vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(1);
	vertices.push_back(1);
	vertices.push_back(0);*/


	std::vector<GLuint> indices;
	//for (int z = 0; z < field_depth - 1; z++) {
	//	for (int x = 0; x < field_width - 1; x++) {
	//		indices.push_back(field[z][x]);
	//		indices.push_back(field[z][x + 1]);
	//		indices.push_back(field[z + 1][x]);

//		indices.push_back(field[z][x + 1]);
//		indices.push_back(field[z + 1][x + 1]);
//		indices.push_back(field[z + 1][x]);
//	}
//}

	for (int z = 0; z < field_depth - 1; z++) {
		for (int x = 0; x < field_width - 1; x++) {
			int curr = z * (field_width)+x;
			indices.push_back(curr);			// First triangle
			indices.push_back(curr + 1);
			indices.push_back(curr + field_width);
			indices.push_back(curr + 1);		// Second triangle
			indices.push_back(curr + field_width + 1);
			indices.push_back(curr + field_width);
		}
	}

	planeIndices = indices.size();
	planeVertices = vertices.size();

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void create_icosahedron(GLuint VAO, GLuint VBO, GLuint EBO, int subdivisionLevel) {
	float rad = 1.0f;
	float X = cos(rad), Z = sin(rad);
	GLfloat icosV[36] = {
		-X, 0.0, Z,
		X, 0.0, Z,
		-X, 0.0, -Z,
		X, 0.0, -Z,
		0.0, Z, X,
		0.0, Z, -X,
		0.0, -Z, X,
		0.0, -Z, -X,
		Z, X, 0.0,
		-Z, X, 0.0,
		Z, -X, 0.0,
		-Z, -X, 0.0
	};

	GLuint icosI[60] = {
		0,4,1,
		0,9,4,
		9,5,4,
		4,5,8,
		4,8,1,
		8,10,1,
		8,3,10,
		5,3,8,
		5,2,3,
		2,7,3,
		7,10,3,
		7,6,10,
		7,11,6,
		11,0,6,
		0,1,6 ,
		6,1,10,
		9,0,11,
		9,11,2,
		9,2,5 ,
		7,2,11,
	};

	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;//(std::begin(icosI), std::end(icosI));

	for (int i = 0; i < 12; i++) {	// Placeholder vertex loading
		vertices.push_back(glm::vec3(icosV[i * 3], icosV[i * 3 + 1], icosV[i * 3 + 2]));
	}


	/**********************************************************************/

	// DO subdivision here. The subdivision will return 
	// Vectors of:
	// - vertices
	// - indices
	// * Subdivides each triangle.
	for (int i = 0; i < sizeof(icosI) / sizeof(GLuint); i += 3) {
		subdivide_triangle(icosI[i], icosI[i + 1], icosI[i + 2], &vertices, &indices, subdivisionLevel);
	}

	// Check for duplicates		TODO
	//for (int i = 0; i < vertices.size(); i++) {
	//	glm::vec3 curr = vertices.at(i);
	//	std::vector<glm::vec3>::iterator startIter = vertices.begin() + i + 1;
	//	std::vector<glm::vec3>::iterator currIter = std::find(startIter, vertices.end(), curr);
	//	while (currIter != vertices.end()) {
	//		int ind = currIter - vertices.begin();

	//	}

	//}
	
	/**********************************************************************/

	icosIndices = indices.size();
	icosVertices = vertices.size();

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
}

// vi = Vertex Index
void subdivide_triangle(int vi0, int vi1, int vi2, std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices, int subLevel) {
	if (subLevel < 2) {
		indices->push_back(vi0);
		indices->push_back(vi1);
		indices->push_back(vi2);
	} else {
		// TODO: Use arrays
		glm::vec3 mv0(glm::normalize(midpoint(vertices->at(vi0), vertices->at(vi1))));
		glm::vec3 mv1(glm::normalize(midpoint(vertices->at(vi1), vertices->at(vi2))));
		glm::vec3 mv2(glm::normalize(midpoint(vertices->at(vi2), vertices->at(vi0))));

		bool dupes = false;

		int mvi0, mvi1, mvi2;
		if (dupes) {	// TODO: Optimize....
			mvi0 = vertices->size();
			//vertices->push_back(glm::normalize(midpoint(vertices->at(vi0), vertices->at(vi1))));
			vertices->push_back(mv0);
			mvi1 = vertices->size();
			//vertices->push_back(glm::normalize(midpoint(vertices->at(vi1), vertices->at(vi2))));
			vertices->push_back(mv1);
			mvi2 = vertices->size();
			//vertices->push_back(glm::normalize(midpoint(vertices->at(vi2), vertices->at(vi0))));
			vertices->push_back(mv2);
		} else {
			std::vector<glm::vec3>::iterator curr = std::find(vertices->begin(), vertices->end(), mv0);
			if (curr != vertices->end()) {
				int ind = curr - vertices->begin();
				mvi0 = ind;
			}
			else {
				mvi0 = vertices->size();
				vertices->push_back(mv0);
			}

			curr = std::find(vertices->begin(), vertices->end(), mv1);
			if (curr != vertices->end()) {
				int ind = curr - vertices->begin();
				mvi1 = ind;
			}
			else {
				mvi1 = vertices->size();
				vertices->push_back(mv1);
			}

			curr = std::find(vertices->begin(), vertices->end(), mv2);
			if (curr != vertices->end()) {
				int ind = curr - vertices->begin();
				mvi2 = ind;
			}
			else {
				mvi2 = vertices->size();
				vertices->push_back(mv2);
			}
		}

		// No dups



		subdivide_triangle(vi0, mvi0, mvi2, vertices, indices, subLevel - 1);
		subdivide_triangle(mvi0, vi1, mvi1, vertices, indices, subLevel - 1);
		subdivide_triangle(mvi2, mvi1, vi2, vertices, indices, subLevel - 1);
		subdivide_triangle(mvi0, mvi1, mvi2, vertices, indices, subLevel - 1);
	}
}	

glm::vec3 midpoint(glm::vec3 p1, glm::vec3 p2) {
	return glm::vec3((p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f, (p1.z + p2.z) / 2.0f);
}


//int cvi = vertices->size();
//vertices->push_back(glm::normalize(centroid(vertices->at(vi0), vertices->at(vi1), vertices->at(vi2))));
//glm::vec3 centroid(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
//	return glm::vec3(
//		(p1.x + p2.x + p3.x) / 3.0f,
//		(p1.y + p2.y + p3.y) / 3.0f,
//		(p1.z + p2.z + p3.z) / 3.0f
//	);
//}


// Triangle normals using provoking vertex

//struct Vertex {
//	Vertex(glm::vec3 pos) : position(pos) {}
//	Vertex(glm::vec3 pos, glm::vec3 norm) : position(pos), normal(norm) {}
//	glm::vec3 position;
//	glm::vec3 normal;
//};

//glEnableVertexAttribArray(1);
//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));

//GLuint icosI[60] = {
//	0,11,9,
//	1,0,4,
//	2,7,3,
//	3,10,8,
//	4,9,5,
//	5,8,4,
//	6,7,11,
//	7,10,3,
//	8,1,4,
//	9,11,2,
//	10,1,8,
//	11,0,6,		// Not enough vertices to represent 

//	0,1,6,
//	1,10,6,
//	2,3,5,
//	3,8,5,
//	4,0,9,
//	5,9,2,
//	6,7,10,
//	7,2,11,
//};

//glm::vec3 triangle_centroid(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
//	return glm::vec3(
//		(p1.x + p2.x + p3.x) / 3.0f,
//		(p1.y + p2.y + p3.y) / 3.0f,
//		(p1.z + p2.z + p3.z) / 3.0f
//	);
//}

//std::set<GLuint> usedV;
//for (int i = 0; i < indices.size() / 3; i++) {
//	// Calculates normal
//	glm::vec3 normal = glm::normalize(triangle_centroid(
//		vertices.at(indices.at(i * 3)).position,
//		vertices.at(indices.at(i * 3 + 1)).position,
//		vertices.at(indices.at(i * 3 + 2)).position
//	));
//	if (usedV.insert(indices.at(i * 3)).second) // Sets the FIRST vertex (provoking vertex) normal if it currently does not have normal
//		vertices.at(indices.at(i * 3)).normal = normal;
//	else {	// Creates a new vertex if the first vertex does have a normal
//		Vertex v(vertices.at(indices.at(i * 3)).position);
//		indices.at(i * 3) = vertices.size();
//		v.normal = normal;
//		vertices.push_back(v);
//		usedV.insert(vertices.size() - 1);
//
//	}
//}