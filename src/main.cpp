#include <bits/stdc++.h>
#include <string.h>
#include <sstream>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14

using namespace std;
using namespace glm;

typedef struct Vec2 {
	float x, y;
} Vec2;

typedef struct Vec3 {
	float x, y, z;
} Vec3;

typedef struct Object {
	vector<Vec3> vertices, normals;
	vector<Vec2> texCoords;
	int textureId;
} Object;

vector<string> objectPath = {
	"../obj/base_abajur.obj", // base abajur
	"../obj/cadeira.obj", // cadeira
	"../obj/cama.obj", // cama
    "../obj/colchao.obj", // colchao
    "../obj/mesa.obj", // mesa
    "../obj/moldura.obj", // moldura 
    "../obj/pernas.obj", // pernas
    "../obj/quadro.obj", // quadro
    "../obj/quarto.obj", // quarto
    "../obj/topo_abajur.obj", // topo abajur
    "../obj/travesseiro.obj", // travesseiro
    "../obj/ventilador.obj", // ventilador
    "../obj/helices.obj", // helices ventilador
    "../obj/janela_direita.obj", // janela direita
    "../obj/janela_esquerda.obj", // janela esquerda
    "../obj/macaneta_direita.obj", // macaneta direita
    "../obj/macaneta_esquerda.obj", // macaneta esquerda
    "../obj/porta_direita.obj", // porta direita
    "../obj/porta_esquerda.obj", // porta esquerda
	"../obj/lixo.obj", // lixeira
    "../obj/base_puff_final.obj",  // base do puff
    "../obj/pes_puff_final.obj" , // pernas do puff
};

vector<string> texturePath = {
	"../textures/cadeira1.jpg", // cadeira
	"../textures/travesseiro.jpg", // travesseiro
	"../textures/colchao.png", // colchao
	"../textures/janela.png", // janela
	"../textures/madeira.jpg", // madeira clara
	"../textures/madeira2.jpg", // madeira porta
	"../textures/madeira2.jpg", // madeira escura 
	"../textures/metal2.jpg", // metal escuro
	"../textures/pintura.jpg", // pintura 
	"../textures/vidro.jpg", // vidro 
	"../textures/metal1.jpg", // metal claro
	"../textures/lixo.jpg", // lixeira
    "../textures/velvet.jpeg",
    "../textures/wood.jpeg",
};

const int objectCount = 22;

Object objects[objectCount];

const int textureCount = 14;

GLuint textures[textureCount];

int WINDOW_WIDTH = 1920, WINDOW_HEIGHT = 1080;

int displayFlag = 0;

GLdouble eyeX = 0.0, eyeY = 0.0, eyeZ = 5.0;
GLdouble centerX = 0.0, centerY = 0.0, centerZ = 0.0;
GLdouble upX = 0.0, upY = 1.0, upZ = 0.0;
GLdouble lastPosX = WINDOW_WIDTH / 2.0, lastPosY = WINDOW_HEIGHT / 2.0;
GLdouble sensivity = 0.01;
GLdouble doorAngle = 0.0;
GLdouble windowAngle = 0.0;

vec3 direction;
GLdouble pitch = 0.0, yaw = -90.0;

bool flag = 0;

void display();

void keyboard(unsigned char key, int x, int y);

void mouse(int x, int y);

void bindObjectTexture() {
	objects[0].textureId = objects[11].textureId = 7;
	objects[1].textureId = 0;
	objects[2].textureId = objects[4].textureId = objects[5].textureId =  4;
	objects[3].textureId = 2;
	objects[6].textureId = objects[12].textureId = 10;
	objects[7].textureId = 8;
	objects[8].textureId = 6;
	objects[9].textureId = 9;
	objects[10].textureId = 1;
	objects[13].textureId = objects[14].textureId = 3;
	objects[15].textureId = objects[16].textureId = 4;
	objects[17].textureId = objects[18].textureId = objects[21].textureId = 13;
	objects[19].textureId = 11;
    objects[20].textureId = 1;
}

bool loadObj(const char* path, Object *object) {
	ifstream file(path);

	if(!file) {
		return false;
	}

	vector<Vec3> tempVertices;
	vector<Vec3> tempNormals;
	vector<Vec2> tempTexCoords;

	float x, y, z;

	string line, keyword;

	while(getline(file, line)) {
		istringstream iss(line);
		iss >> keyword;

		if(keyword.compare("#") == 0) { // Comment
			continue;
		}
	
		if(keyword.compare("v") == 0){
			// Parse vertex
			iss >> x >> y >> z;
			tempVertices.push_back({x, z, y}); // swapping y-axis with z-axis 
		}
		else if(keyword.compare("vn") == 0) {
			// Parse normal vector
			iss >> x >> y >> z;
			tempNormals.push_back({x, z, y}); // swapping y-axis with z-axis
		}
		else if(keyword.compare("vt") == 0) {
			// Parse texture coordinate
			iss >> x >> y;
			tempTexCoords.push_back({x, -y});
		}
		else if(keyword.compare("f") == 0) {
			// Parse face
			int i, posVertices, posTexCoords, posNormals;
			char slash;
			for(i = 0; i < 3 ;i++) {
				iss >> posVertices >> slash >> posTexCoords >> slash >> posNormals;
				object->vertices.push_back(tempVertices[posVertices - 1]);
				object->texCoords.push_back(tempTexCoords[posTexCoords - 1]);
				object->normals.push_back(tempNormals[posNormals - 1]);
			}
		}
	}

	file.close();

	return true;
}

bool loadAllObjects() {
	for(int i = 0; i < objectCount; i++) {
		if(!loadObj(objectPath[i].c_str(), &objects[i])) {
			perror("Erro ao abrir o arquivo: ");
            perror(objectPath[i].c_str());
			return false;
		}
	}
	
	return true;
}

bool loadTexture(int index) {
    int width, height, nrComponents;
    unsigned char *data = stbi_load(texturePath[index].c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textures[index]);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // define the parameters of the texture

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        stbi_image_free(data);
		return false;
    }
	
	return true;
}

bool loadAllTextures() {
	glGenTextures(textureCount, textures);

	for(int i = 0; i < textureCount; i++) {
		if(!loadTexture(i)) {
			perror("Erro ao carregar a textura: ");
            perror(texturePath[i].c_str());
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
		
	if(!loadAllObjects()) {
		perror("Erro ao carregar todos os arquivos");
		return -1;
	}
	
	if(!loadAllTextures()) {
		perror("Erro ao carregar todos as texturas");
		return -1;
	}
	
    bindObjectTexture();
		
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

    for(index = 0; index < objectCount; index++) {
        if(index == 13) { // janela direita
		    glPushMatrix();
            glTranslatef(objects[index].vertices[0].x, objects[index].vertices[0].y, objects[index].vertices[0].z);
            glRotatef(-windowAngle, 0.0, 0.0f, 1.0f);
            glTranslatef(-objects[index].vertices[0].x, -objects[index].vertices[0].y, -objects[index].vertices[0].z);
		}

        else if(index == 14) { // janela esquerda
            glPushMatrix();
            float larguraJanela = 0.33;
            float comprimentoJanela = 3.78;
            glTranslatef(objects[index].vertices[0].x + comprimentoJanela, objects[index].vertices[0].y - larguraJanela, objects[index].vertices[0].z);
            glRotatef(windowAngle, 0.0, 0.0f, 1.0f);
            glTranslatef(-objects[index].vertices[0].x - comprimentoJanela, -objects[index].vertices[0].y + larguraJanela, -objects[index].vertices[0].z);
        }

		else if(index == 17 || index == 15) { // porta e macaneta direita
		    glPushMatrix();
            glTranslatef(objects[17].vertices[0].x - 0.05, objects[17].vertices[0].y, objects[17].vertices[0].z);
            glRotatef(-doorAngle, 0.0, 0.0f, 1.0f);
            glTranslatef(-objects[17].vertices[0].x + 0.05, -objects[17].vertices[0].y, -objects[17].vertices[0].z);
		}
        else if(index == 18 || index == 16) { // porta e macaneta esquerda
            glPushMatrix();
            float tamanhoPorta = 3.01;
            glTranslatef(objects[18].vertices[0].x + tamanhoPorta, objects[18].vertices[0].y, objects[18].vertices[0].z);
            glRotatef(doorAngle, 0.0, 0.0f, 1.0f);
            glTranslatef(-objects[18].vertices[0].x - tamanhoPorta, -objects[18].vertices[0].y, -objects[18].vertices[0].z);
        }

		glBindTexture(GL_TEXTURE_2D, textures[objects[index].textureId]);
        glBegin(GL_TRIANGLES);
        for(i = 0; i < objects[index].vertices.size(); i++) {
            glNormal3f(objects[index].normals[i].x, objects[index].normals[i].y, objects[index].normals[i].z);
            glTexCoord2f(objects[index].texCoords[i].x, objects[index].texCoords[i].y);
            glVertex3f(objects[index].vertices[i].x, objects[index].vertices[i].y, objects[index].vertices[i].z);
        }
        glEnd();

		if(index >= 13 && index <= 18) {
			glPopMatrix();
        }
        
    }
    displayFlag++;
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

		direction.x = cos(radians(yaw)) * cos(radians(pitch));
		direction.y = sin(radians(pitch));
		direction.z = sin(radians(yaw)) * cos(radians(pitch));

		centerX = direction.x;
		centerY = direction.y;
		centerZ = direction.z;

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
		break;
	case 'b':
		eyeY -= movementSpeed;
		break;
	case 'w':
		eyeX += direction.x * movementSpeed; 
		eyeY += direction.y * movementSpeed;
		eyeZ += direction.z * movementSpeed;
		break;
	case 's':
		eyeX -= direction.x * movementSpeed; 
		eyeY -= direction.y * movementSpeed;
		eyeZ -= direction.z * movementSpeed;
		break;
    case 'J':
        if(windowAngle > 0.0) {
            windowAngle -= 3.0;
        }
        break;
    case 'j':
        if(windowAngle < 90) {
            windowAngle += 3.0;
        }
        break;
    case 'P':
        if(doorAngle > 0.0) {
            doorAngle -= 3.0;
        }
        break;
    case 'p':
        if(doorAngle < 90.0) {
            doorAngle += 3.0;
        }
        break;
	}

    glutPostRedisplay();
}