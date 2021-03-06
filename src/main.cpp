#define _USE_MATH_DEFINES
#include <cstdio>
#include <math.h>
#include <GL/glut.h>
#include "bibutil.h"
#include "Vector3d.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const GLdouble ASPECT = (GLdouble) WINDOW_WIDTH / WINDOW_HEIGHT;
const int CENTER_X = WINDOW_WIDTH / 2;
const int CENTER_Y = WINDOW_HEIGHT / 2;
const int FRAME_TIME = 1000 / 30;

void onTimerTick(int step);
void onDisplay();
void onMouseClick(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void drawCube(double size, Vector3d where, Vector3d color);
void Inicializa (void);
void drawObj();

// Parametrização em coordenadas esféricas
GLdouble r = 10; // Distância do entre o observador (camera) e o ponto
GLdouble theta = 0.0; // Angulo em torno do eixo Y
GLdouble phi   = 0.0; // Angulo em torno do eixo X relativo a camera

const GLdouble turn_speed = 0.4;
const GLdouble move_speed = 0.3;
GLdouble move_direction = 0;

Vector3d player = {0, 0, 0};

OBJ *objeto; // Objeto a ser carregado

// Função responsável pela especificação dos parâmetros de iluminação

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("Trabalho 3");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutTimerFunc(FRAME_TIME, onTimerTick, 0);
	glutDisplayFunc(onDisplay);

	glutSetCursor(GLUT_CURSOR_NONE);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMove);
	glutPassiveMotionFunc(onMouseMove);

	// Chama a função responsável por fazer as inicializações
	Inicializa();

	glutMainLoop();

	return 0;
}


void onTimerTick(int step)
{
	glutPostRedisplay();
	glutTimerFunc(FRAME_TIME, onTimerTick, step + 1);
}


void onDisplay()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, ASPECT, 1.0, 100.0);


	double phi_up = phi + M_PI/2;

	// Desloca o jogador (move_direction pode ser 0)
	player += move_direction * coords(-move_speed, theta, phi);

	// Calcula vetores da camera
	Vector3d cam_pos = coords(r, theta, phi) + player;
	Vector3d cam_up  = coords(r, theta, phi_up);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
			cam_pos.x, cam_pos.y, cam_pos.z,
			player.x, player.y, player.z,
			cam_up.x, cam_up.y , cam_up.z);


	// Desenha os eixos XYZ
	glLineWidth(1);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f(-100.0, 0.0, 0.0);
		glVertex3f(100, 0, 0);
	glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, -100.0, 0.0);
		glVertex3f(0.0, 100.0, 0);
	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, -100.0);
		glVertex3f(0.0, 0.0, 100.0);
	glEnd();

	// Define a cor azul para desenhar o objeto
	glColor3f(0.0f, 0.0f, 1.0f);
	drawObj();

	// Desenha um cubo em cada octeto
	GLdouble side = 3;
	GLdouble dist = 5;

	drawCube(side, { dist, dist, dist}, {0.1, 0.1, 0.1});
	drawCube(side, { dist, dist,-dist}, {0.1, 0.1, 1.0});
	drawCube(side, { dist,-dist, dist}, {0.1, 1.0, 0.1});
	drawCube(side, { dist,-dist,-dist}, {0.1, 1.0, 1.0});
	drawCube(side, {-dist, dist, dist}, {1.0, 0.1, 0.1});
	drawCube(side, {-dist, dist,-dist}, {1.0, 0.1, 1.0});
	drawCube(side, {-dist,-dist, dist}, {1.0, 1.0, 0.1});
	drawCube(side, {-dist,-dist,-dist}, {1.0, 1.0, 1.0});


	glutSwapBuffers();
}


void onMouseClick(int button, int state, int x, int y)
{
	(void) x;
	(void) y;

	if(button == GLUT_LEFT_BUTTON)
	{
		// Anda para frente
		move_direction = 1.0 * (state == GLUT_DOWN);
	}
	else if(button == GLUT_RIGHT_BUTTON)
	{
		// Anda para tras
		move_direction = (-1.0) * (state == GLUT_DOWN);
	}
}

void onMouseMove(int x, int y)
{
	int centerX = WINDOW_WIDTH / 2;
	int centerY = WINDOW_HEIGHT / 2;

	// Converte a partir das coordenadas do dispositivo
	y = WINDOW_HEIGHT - y;

	int deltaX = x - centerX;
	int deltaY = y - centerY;

	//printf("THETA = %f\n", theta);
	//printf("PHI   = %f\n", phi);

	theta -= turn_speed * deltaX * (2*M_PI / WINDOW_WIDTH);
	phi   -= turn_speed * deltaY * (2*M_PI / WINDOW_HEIGHT);

	theta = fmod(theta, 2 * M_PI);
	phi = fmod(phi, 2 * M_PI);

	// Mantem o cursor no centro da tela
	if(x != centerX || y != centerY)
	{
		glutWarpPointer(centerX, centerY);
	}
}

void drawCube(double size, Vector3d where, Vector3d color)
{
	glPushMatrix();

	glTranslated(where.x, where.y, where.z);

	glColor3d(0, 0, 0);
	glutWireCube(size);

	// Nao vou fazer outra struct so pra botar rgb em vez de xyz
	glColor3d(color.x, color.y, color.z);
	glutSolidCube(size);

	glPopMatrix();
}

void drawObj()
{
	glTranslatef(player.x, player.y, player.z);	// translada para a posição que está sendo movida
	glRotatef(90, 1.0, 0.0, 0.0);				// deixa o modelo em pé
	
	DesenhaObjeto(objeto);

	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(-player.x, -player.y, -player.z);
}

// Função responsável por inicializar parâmetros e variáveis
void Inicializa (void)
{
	// Define a cor de fundo da janela de visualização como branca
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
	// Habilita o modelo de tonalização de Gouraud
	glShadeModel(GL_SMOOTH);

	// Carrega o objeto 3D
	objeto = CarregaObjeto("models/cow-nonormals.obj",true);

    printf("Objeto carregado!");

	// E calcula o vetor normal em cada face
	if(objeto->normais)
	{
		// Se já existirem normais no arquivo, apaga elas
		free(objeto->normais);
		objeto->normais_por_vertice = false;
	}
	CalculaNormaisPorFace(objeto);
}
