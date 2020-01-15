#define  _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"glut64.lib")
#include <windows.h>
#include "glut\glut.h"
#include <string>
#include <vector>
#include "msModel.h"
#include "DOF.h"
#define GLOBAL_LIGHT 0
#define  GLUT_WHEEL_UP 3
#define  GLUT_WHEEL_DOWN 4  
GLint screenHeight = 1212;
GLint screenWidth = 812;
bool	mouse_isDown = false;
float	x_angle = 0;
float	y_angle = 0;
float	scale = 0.1f;
int		mx, my;

GLuint boxTexture = -1;
std::string modelName;
msModel modelLoader;
int cnt = 1;

static vector<float> sdof = vector<float>(20, 0);



void ImportTextures()
{
	modelLoader.Load(modelName.c_str());
	modelLoader.SetupJoints();
	modelLoader.SetFrame(0);
	modelLoader.reAlignJoint();
}

void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	ImportTextures();

	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1, 6.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display(void)
{
	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(1.0);
	
	glPushMatrix();
	glTranslated(0.0, 0.0, -3.0);
	glScalef(scale, scale, scale);
	glRotated(x_angle, 1.0, 0.0, 0.0);
	glRotated(y_angle, 0.0, 1.0, 0.0);

	glTranslated(-0.4, 0, 0);
			
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	modelLoader.Render(false, false);
	glDisable(GL_TEXTURE_2D);
	modelLoader.RenderJoint();
	glPopMatrix();

	modelLoader.renderWord();
	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	float aspect = (float)w / (float)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, aspect, 0.1, 200.0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void myMouseDown(int button, int state, int x, int y)
{
	if (GLUT_DOWN == state)
	{
		if (GLUT_LEFT_BUTTON == button)
		{
			mx = x; my = y;
			mouse_isDown = true;
		}
	}
	else mouse_isDown = false;
	display();
}

void myMouseMove(int x, int y)
{
	int dx, dy;
	dx = x - mx;
	dy = y - my;
	mx = x;
	my = y;
	if (mouse_isDown)
	{
		y_angle += dx*0.2f;
		x_angle += dy*0.2f;
	}
	display();
}
void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'w': {
			modelLoader.setWaveframe();
			break;
		}
		case 'q': {
			exit(0);
		}
		case ' ': {
			vector<float>df = DOF::randDOF();
			sdof = df;
			modelLoader.getRandomAngle(sdof);
			break;
		}

		case 'v': {
			//vector<float> df = DOF::randDOF();
			//RAND_MAX
			

			//sdof[0] += pi / 10;
			//sdof[1] += pi / 10;
			//sdof[2] += pi / 10;
			sdof[3] += pi / 10;

			modelLoader.getRandomAngle(sdof);
			break;


		}


		case 's': {
			char filename[128] = { 0 };
			for (int i = 0; i < 50; i++) {
				vector<float>df = DOF::randDOF();
				modelLoader.getRandomAngle(df);
				sprintf(filename, "./model/hand_model%d.obj", cnt);
				modelLoader.saveOBJ(filename);
				sprintf(filename, "./model/hand_model%d.txt", cnt++);
				modelLoader.saveJoint(filename);
			}
			break;
		}
		case 'a': {
			modelLoader.setJointArea();
			break;
		}
		case 'e': {
			modelLoader.reset();
			break;
		}
		case 'z': {
			scale += 0.02f;
			break;
		}
		case 'c': {
			if(scale>0.02f)
				scale -= 0.02f;
			break;
		}
	}
	display();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH| GLUT_RGB);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("hand Maker");
	if (argc == 2)
		modelName = argv[1];
	//register callback functions
	glutDisplayFunc(display);
	glutMouseFunc(myMouseDown);
	glutMotionFunc(myMouseMove);
	glutKeyboardFunc(myKeyboard);
	//initialize and main loop
	init();
	glutMainLoop();

	return 0;
}
