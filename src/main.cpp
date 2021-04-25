#include <bits/stdc++.h>
#include <string.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
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

vector<string> objectPath = {
	"../obj/base_abajur.obj", // base abajur
	"../obj/cadeira.obj", // cadeira
	"../obj/cama.obj" // cama


};

int objectSize = 3;

Object objects[3];

int WINDOW_WIDTH = 640, WINDOW_HEIGHT = 480;

GLdouble eyeX = 0.0, eyeY = 0.0, eyeZ = 5.0;
GLdouble centerX = 0.0, centerY = 0.0, centerZ = 0.0;
GLdouble upX = 0.0, upY = 1.0, upZ = 0.0;
GLdouble lastPosX = WINDOW_WIDTH / 2.0, lastPosY = WINDOW_HEIGHT / 2.0;
GLdouble sensivity = 0.5;

GLdouble pitch = 0.0, yaw = -90.0;

bool flag = 0;

void display();

void keyboard(unsigned char key, int x, int y);

void mouse(int x, int y);

bool loadObj(const char* path, Object *object) {
	FILE* file = fopen(path, "r");

	if(!file) {
		return false;
	}

	char buffer[512] = "";
	
	vector<Vec3> tempVertices;
	vector<Vec3> tempNormals;
	vector<Vec2> tempTexCoords;

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

bool loadAllobjects() {
	for(int i = 0; i < objectSize; i++) {
		if(!loadObj(objectPath[i].c_str(), &objects[i])) {
			perror("Erro ao abrir o arquivo");
			return false;
		}
	}
	
	return true;
}

int main(int argc, char** argv) {
	if(argc == 3) {
		WINDOW_WIDTH = atoi(argv[1]);
		WINDOW_HEIGHT = atoi(argv[2]);	
	}

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
	glEnable(GL_COLOR_MATERIAL);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, (WINDOW_WIDTH * 1.0) / (WINDOW_HEIGHT * 1.0), 10e-3, 10e3);

	// TODO -> CREATE A FUNC TO LOAD ALL THE objects[0]

	if(!loadAllobjects()) {
		perror("Erro ao carregar todos os arquivos");
		return -1;
	}

	unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load("../../madeira.jpg", &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        perror("Texture failed to load");
        stbi_image_free(data);
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
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	
	int i, index = 0;

	// drawing base abajur
	glBegin(GL_TRIANGLES);
	for(i = 0; i < objects[index].vertices.size(); i++){
		glNormal3f(objects[index].normals[i].x, objects[index].normals[i].y, objects[index].normals[i].z);
		glTexCoord2f(objects[index].texCoords[i].x, objects[index].texCoords[i].y);
		glVertex3f(objects[index].vertices[i].x, objects[index].vertices[i].y, objects[index].vertices[i].z);
	}
	glEnd();

	index++;

	// drawing cadeira
	glBegin(GL_TRIANGLES);
	for(i = 0; i < objects[index].vertices.size(); i++){
		glNormal3f(objects[index].normals[i].x, objects[index].normals[i].y, objects[index].normals[i].z);
		glTexCoord2f(objects[index].texCoords[i].x, objects[index].texCoords[i].y);
		glVertex3f(objects[index].vertices[i].x, objects[index].vertices[i].y, objects[index].vertices[i].z);
	}
	glEnd();

	index++;

	// drawing cama
	glBegin(GL_TRIANGLES);
	for(i = 0; i < objects[index].vertices.size(); i++){
		glNormal3f(objects[index].normals[i].x, objects[index].normals[i].y, objects[index].normals[i].z);
		glTexCoord2f(objects[index].texCoords[i].x, objects[index].texCoords[i].y);
		glVertex3f(objects[index].vertices[i].x, objects[index].vertices[i].y, objects[index].vertices[i].z);
	}
	glEnd();

	glutSwapBuffers();
}

void mouse(int x, int y) {
	if(!flag) {
		float xoffset = x - (WINDOW_WIDTH / 2);
		float yoffset = (WINDOW_HEIGHT / 2) - y; // reversed since y-coordinates go from bottom to top

		yaw += xoffset * sensivity;
		pitch += yoffset * sensivity;

		if(pitch > 89.0f) {
			pitch =  89.0f;
		}
		else if(pitch < -89.0f) {
			pitch = -89.0f;
		}

		glm::vec3 direction, cameraFront;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);

		centerX = cameraFront.x;
		centerY = cameraFront.y;
		centerZ = cameraFront.z;

		glutWarpPointer(WINDOW_WIDTH / 2 , WINDOW_HEIGHT / 2);  //centers the cursor
		flag = 1;
	}
	else {
		flag = 0;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	GLdouble movementSpeed = 0.5;
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
		eyeX += cos(glm::radians(yaw)) * movementSpeed; 
		eyeY += sin(glm::radians(pitch)) * movementSpeed;
		eyeZ += sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * movementSpeed;
		glutPostRedisplay();
		break;
	case 's':
		eyeX -= cos(glm::radians(yaw)) * movementSpeed; 
		eyeY -= sin(glm::radians(pitch)) * movementSpeed;
		eyeZ -= sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * movementSpeed;
		glutPostRedisplay();
		break;
	}
}