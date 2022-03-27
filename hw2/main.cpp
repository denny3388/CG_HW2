#define GLM_ENABLE_EXPERIMENTAL

#include "Object.h"
#include "FreeImage.h"
#include "glew.h"
#include "freeglut.h"
#include "shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <string>
#include <math.h>
#include <stb_image.h>

using namespace std;

void shaderInit();
void bindbufferInit();
void textureInit();
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void SpecialInput(int key, int x, int y);
void DrawBasis();
void DrawUmbreon();
void LoadTexture(unsigned int&, const char*);

const GLfloat Pi = 3.14159265359f;
GLuint program;
GLuint VAO, VBO[3];
unsigned int basistexture, modeltexture;
int windowSize[2] = { 600, 600 };
float angle = 0.0f;
float cam_h = 7.5f;
float transparency = 1.0f;
float r = 0, g = 0, b = 0;

Object* model = new Object("UmbreonHighPoly.obj");

class VertexAttribute {
public:
	GLfloat position[3];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
};

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw2");

	glewInit();
	shaderInit();
	bindbufferInit();
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialInput);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	//// TODO: ////
	//
	// Hint:
	// 1. createShader
	// 2. createProgram

	GLuint vert = createShader("Shaders/vertexShader.vert", "vertex");
	GLuint frag = createShader("Shaders/fragmentShader.frag", "fragment");
	program = createProgram(vert, frag);

}

void bindbufferInit() {
	//// TODO: ////
	//
	//	 Hint:
	// 1. Setup VAO
	// 2. Setup VBO of vertex positions, normals, and texcoords

	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// vertex position
	GLuint vertex_vbo;
	glGenBuffers(1, &vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->positions.size(), model->positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// vertex normal
	GLuint normal_vbo;
	glGenBuffers(1, &normal_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->normals.size(), model->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	// vertex texture coordinate
	GLuint tex_vbo;
	glGenBuffers(1, &tex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->texcoords.size(), model->texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

void textureInit() {
	LoadTexture(basistexture, "basis.jpg");
	LoadTexture(modeltexture, "Umbreon.jpg");
}

glm::mat4 getV()
{
	// set camera position and configuration
	return glm::lookAt(glm::vec3(7.5, cam_h, 7.5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}
glm::mat4 getP()
{
	// set perspective view
	float fov = 45.0f;
	float aspect = windowSize[0] / windowSize[1];
	float nearDistance = 1.0f;
	float farDistance = 1000.0f;
	return glm::perspective(glm::radians(fov), aspect, nearDistance, farDistance);
}

void display() {
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawUmbreon();

	DrawBasis();

	angle += 0.05;
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void LoadTexture(unsigned int& texture, const char* tFileName) {
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tFileName, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

void keyboard(unsigned char key, int x, int y) {
	if (key == '0' && transparency > 0)
		transparency -= 0.05;
	else if (key == '1' && transparency < 1)
		transparency += 0.05;
	else if (key == 'r' && r < 1)
		r += 0.05;
	else if (key == 'R' && r > -1)
		r -= 0.05;
	else if (key == 'g' && g < 1)
		g += 0.05;
	else if (key == 'G' && g > -1)
		g -= 0.05;
	else if (key == 'b' && b < 1)
		b += 0.05;
	else if (key == 'B' && b > -1)
		b -= 0.05;
	else if (key == ' ') {
		transparency = 1;
		r = g = b = 0;
	}
		
}

void SpecialInput(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		cam_h++;
	else if (key == GLUT_KEY_DOWN)
		cam_h--;
}

void idle() {
	glutPostRedisplay();
}

void DrawBasis() {
	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(7.5, cam_h, 7.5,// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up

	glPushMatrix();
	glRotatef(angle, 0, 1, 0);

	//// if you don't need this, you can just deleting
	float edge = 20;
	float radius = 4;
	float pi = 3.1415926;

	//// TODO: ////
	//
	// draw the basis and make the side of basis with texture
	static float vertex_list[20][3];
	for (int i = 0; i < 20; ++i) {
		vertex_list[i][0] = radius * cos(2 * Pi / 20 * i);
		vertex_list[i][1] = 0;
		vertex_list[i][2] = radius * sin(2 * Pi / 20 * i);
	}

	glColor3f(0.6, 0.3, 0.1);
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	for (int i = 0; i < 20; ++i)
		glVertex3f(vertex_list[i][0], vertex_list[i][1], vertex_list[i][2]);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(0, -1, 0);
	for (int i = 0; i < 20; ++i)
		glVertex3f(vertex_list[i][0], vertex_list[i][1] - 5, vertex_list[i][2]);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, basistexture);
	for (int i = 0; i < 20; i++) {
		glBegin(GL_POLYGON);
		int j = (i + 1) % 20, h = 5;
		float nor_x = (vertex_list[i][0] + vertex_list[j][0]) / 2.0f;
		float nor_z = (vertex_list[i][2] + vertex_list[j][2]) / 2.0f;
		float div = sqrt((nor_x * nor_x) + (nor_z * nor_z));
		glNormal3f(nor_x / div, 0, nor_z / div);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(vertex_list[i][0], vertex_list[i][1], vertex_list[i][2]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(vertex_list[j][0], vertex_list[j][1], vertex_list[j][2]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(vertex_list[j][0], vertex_list[j][1] - h, vertex_list[j][2]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(vertex_list[i][0], vertex_list[i][1] - h, vertex_list[i][2]);
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(0);

	glPopMatrix();
}

void DrawUmbreon()
{
	glUseProgram(program);

	glm::mat4 M(1.0f);
	M = glm::rotate(M, glm::radians(angle), glm::vec3(0, 1, 0));
	M = glm::translate(M, glm::vec3(0, 1.3, 0));

	GLuint ModelMatrixID = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);

	//// TODO: ////
	// pass projection matrix, and view matrix and trigger by Uniform (use getP() amd getV())
	// also pass modeltexture to shader and trigger by Uniform

	glm::mat4 pmtx = getP();
	GLuint pmatLoc = glGetUniformLocation(program, "Projection");
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, &pmtx[0][0]);


	glm::mat4 vmtx = getV();
	GLuint vmatLoc = glGetUniformLocation(program, "View");
	glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, &vmtx[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	GLint texLoc = glGetUniformLocation(program, "Texture");
	glUniform1i(texLoc, 0);

	GLuint transLoc = glGetUniformLocation(program, "transparency");
	glUniform1f(transLoc, transparency);

	GLuint rLoc = glGetUniformLocation(program, "r");
	glUniform1f(rLoc, r);

	GLuint gLoc = glGetUniformLocation(program, "g");
	glUniform1f(gLoc, g);

	GLuint bLoc = glGetUniformLocation(program, "b");
	glUniform1f(bLoc, b);


	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(VAO);
	glDrawArrays(GL_QUADS, 0, 4 * model->fNum);

	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}