#include <bits/stdc++.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14

using namespace std;

typedef struct Vec2 {
	float x, y;
} Vec2;

typedef struct Vec3 {
	float x, y, z;
} Vec3;


typedef struct Object {
	vector<Vec3> vertices, normals;
	vector<Vec2> texCoords;
} Object;


Object mesa = {};

GLdouble eyeX = 0.0, eyeY = 0.0, eyeZ = 5.0;
GLdouble centerX = 0.0, centerY = 0.0, centerZ = -5.0;
GLdouble upX = 0.0, upY = 1.0, upZ = 0.0;

int WINDOW_WIDTH = 640, WINDOW_HEIGHT = 480;

GLdouble pitch = 0.0, yaw = 0.0;
GLdouble lastPosX = (WINDOW_WIDTH / 2.0), lastPosY = (WINDOW_HEIGHT / 2.0);

void display();

void keyboard(unsigned char key, int x, int y);

void mouse(int x, int y);

bool load_obj(const char* path, Object *object);

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Quarto");
	glutWarpPointer(WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2);  //centers the cursor

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, (WINDOW_WIDTH * 1.0) / (WINDOW_HEIGHT * 1.0), 10e-3, 10e3);

	// TODO -> CREATE A FUNC TO LOAD ALL THE OBJECTS

	if(!load_obj("../obj/mesa.obj", &mesa)) {
		perror("Erro ao abrir o arquivo");
		return -1;
	}
	
	glutMainLoop();

	return 0;
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// View matrix
	gluLookAt(eyeX, eyeY, eyeZ, eyeX + centerX, eyeY + centerY, eyeZ + centerZ, upX, upY, upZ);
	// Model matrix
	//float t = 1.0f * glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	//glRotatef(10.0f * t, 0.0f, 1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	
	int i;

	// drawing table
	glBegin(GL_TRIANGLES);
	for(i = 0; i < mesa.vertices.size(); i++){
		glNormal3f(mesa.normals[i].x, mesa.normals[i].y, mesa.normals[i].z);
		glTexCoord2f(mesa.texCoords[i].x, mesa.texCoords[i].y);
		glVertex3f(mesa.vertices[i].x, mesa.vertices[i].y, mesa.vertices[i].z);
	}
	glEnd();


	glutSwapBuffers();
}

void mouse(int x, int y) {
	GLdouble sensivity = 0.1;
	yaw += (x - lastPosX) * sensivity;
	pitch += (lastPosY - y) * sensivity;
	
	lastPosX = x;
	lastPosY = y;
	
	if(pitch > 89.0f) {
        pitch = 89.0;
	}
    else if(pitch < -89.0f) {
        pitch = -89.0;
	}

	GLdouble yawRadians = (yaw * PI) / 180.0;
	GLdouble pitchRadians = (pitch * PI) / 180.0;

	//eyeX = cos(yawRadians) * cos(pitchRadians);
    //eyeY = sin(pitchRadians);
    //eyeZ = sin(yawRadians) * cos(pitchRadians);
	
	if(x < ((WINDOW_WIDTH / 2) - (WINDOW_WIDTH / 10)) || x > ((WINDOW_WIDTH / 2) + (WINDOW_WIDTH / 10))) {  
        glutWarpPointer(WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2);  //centers the cursor
    } 
	else if(y < ((WINDOW_HEIGHT / 2) - (WINDOW_HEIGHT / 10)) || y > ((WINDOW_HEIGHT / 2) + (WINDOW_HEIGHT / 10))) {
        glutWarpPointer(WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2);  //centers the cursor
    } 

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	GLdouble movementSpeed = 0.05;
  	switch (key) {
	case 27: // esc
		exit(0);
		break;
	case 32: // spacebar
		eyeY += movementSpeed;
		glutPostRedisplay();
		break;
	case 'b':
		eyeY -= movementSpeed;
		glutPostRedisplay();
		break;
	case 'w':
		eyeZ -= movementSpeed;
		glutPostRedisplay();
		break;
	case 'a':
		eyeX -= movementSpeed;
		glutPostRedisplay();
		break;
	case 's':
		eyeZ += movementSpeed;
		glutPostRedisplay();
		break;
	case 'd':
		eyeX += movementSpeed;
		glutPostRedisplay();
		break;
	}
}


bool load_obj(const char* path, Object *object) {
	FILE* file = fopen(path, "r");

	if(!file) {
		return false;
	}

	char buffer[512] = "";
	
	vector<Vec3> tempVertices;
	vector<Vec3> tempNormals;
	vector<Vec2> tempTexCoords;

	Object tempvertices;
	float x, y, z;

	while(fgets(buffer, 512, file)){
		if(buffer[0] == '#') { // Comment
			continue;
		}

		char *token = strtok(buffer, " ");
	
		if(strcmp(token, "v") == 0){
			// Parse vertex
			x = atof(strtok(NULL, " "));
			y = atof(strtok(NULL, " "));
			z = atof(strtok(NULL, " "));
			tempVertices.push_back({x, z, y}); // swapping y-axis with z-axis 
		}
		else if(strcmp(token, "vn") == 0) {
			// Parse normal vector
			x = atof(strtok(NULL, " "));
			y = atof(strtok(NULL, " "));
			z = atof(strtok(NULL, " "));
			tempNormals.push_back({x, z, y}); // swapping y-axis with z-axis
		}
		else if(strcmp(token, "vt") == 0) {
			// Parse texture coordinate
			x = atof(strtok(NULL, " "));
			y = -atof(strtok(NULL, " "));
			tempTexCoords.push_back({x, y});
		}
		else if(strcmp(token, "f") == 0) {
			// Parse face
			int i;
			for(i = 0; i < 3 ;i++){
				object->vertices.push_back(tempVertices[atoi(strtok(NULL, "/")) - 1]);
				object->texCoords.push_back(tempTexCoords[atoi(strtok(NULL, "/")) - 1]);
				object->normals.push_back(tempNormals[atoi(strtok(NULL, " ")) - 1]);
			}
		}
	}

	fclose(file);

	return true;
}