#include <iostream>
#include "glut\glut.h"
#include "Camera.h"
#include "ObjModel.h"
#include <vector>
#include <cmath>
using namespace std;
Camera cam;
OBJModel objloader;
void init(void)
{

	/** �û��Զ���ĳ�ʼ������ */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	cam.setCamera(20, 20, 20, 0, 0, 0, 0, 0, 1);

	objloader.loadFromFile("F:/���ڿ���/handMaker/cube/mycube.obj", "F:/���ڿ���/handMaker/cube/mycube.mtl");
}

void display(void)
{
	/** �û��Զ���Ļ��ƹ��� */
	double size =0.1;
	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glPushMatrix();
	glScaled(size, size, size);
	objloader.renderObj();
	cam.setLook();
	glPopMatrix();
	glutSwapBuffers();
	glFlush();

}
void motion(int x, int y)
{
	
	cam.setViewByMouse(x, y);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	
	switch (key) {
	case 'q':
		exit(0);
		break;

	case 'a':
		cam.moveCamera(10);
		break;
	case 'd':
		cam.moveCamera(-10);
		break;
	case 'w':
		cam.goCamera(10);
		break;
	case 's':
		cam.goCamera(-10);
		break;
	}
	glutPostRedisplay();
}
void ChangeSize(int width, int height)
{
	glViewport(0, 0, width, height);                                    /**< ���������ӿ� */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 4000000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB);
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	int h = screenHeight * 2 / 4;
	int w = h * 4 / 3;
	int x = (screenWidth - w) / 2;
	int y = (screenHeight - h) / 2;
	glutInitWindowPosition(x, y);
	glutInitWindowSize(w, h);
	glutCreateWindow("���⻷������");
	init();
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutPassiveMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}