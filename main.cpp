#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <Windows.h>
#include <GL/glut.h>
#include <string>
#include <vector>
#include "myObj.h"
#include "myPose.h"

#define GLOBAL_LIGHT 0
#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4
#define BMP_Header_Length 54
#define MAX_NUM_TEX 7

#define SHOW_SKELETON 1




GLint screenHeight = 800;
GLint screenWidth = 1000;
bool mouse_isDown = false;
float x_angle = 0;
float y_angle = 0;
float x_trans = 0.0;
float y_trans = 0.0;
float z_trans = -0.3;
float scale = 1.0f;
int	mx, my;
int index_dof_R = 0;
glm::mat4 global_R(glm::mat4(0.0)), global_L(glm::mat4(0.0));
float step = 0;
int save_img_id = 0;


GLuint boxTexture = -1;
std::string modelName;

int cnt = 1;
std::vector<float> dof_R(25,0),dof_L(25,0);
std::vector<float> global_dof(3,0);

static std::vector<float> sdof = std::vector<float>(20, 0);
myObjModel mom;
lbsPose  plp;

GLuint texhand;
GLuint ImportTextures(const char* file_name);
static int index_tex = 1;



void init(void);
void render();
void showInfo();
void print(int x, int y, const char *string);
void grab();
void display(void);
void reshape(int w, int h);
void myMouseDown(int button, int state, int x, int y);
void myMouseMove(int x, int y);
void myKeyboard(unsigned char key, int x, int y);











int main(int argc, char** argv)
{

	//lbsPose * plp =new lbsPose();
	plp.load_weight_matrix_from_txt("weight.txt");
	//plp->random_pose();
	
 	mom.Load("./res/lj/lj.obj");
	mom.Mirror();
#ifdef CHANGE

	for (int i = 0; i < mom.m_vertices.size(); i++)
	{
		//X = -Z Y = -X  Z = -Y
		glm::vec3 t(-plp.init_vertexs[i].z, -plp.init_vertexs[i].x, -plp.init_vertexs[i].y);
		mom.m_vertices[i] = t;
		plp.init_vertexs[i].x = t.x;
		plp.init_vertexs[i].y = t.y;
		plp.init_vertexs[i].z = t.z;
	
		glm::vec4 ct(-plp.cur_vertexs[i].z, -plp.cur_vertexs[i].x, -plp.cur_vertexs[i].y, plp.cur_vertexs[i].w);
		plp.cur_vertexs[i] = ct;	
	}

	for (int i = 0; i < plp.init_palm_pos.size(); i++)
	{
		glm::vec3 t(-plp.init_palm_pos[i].z, -plp.init_palm_pos[i].x, -plp.init_palm_pos[i].y);
		plp.init_palm_pos[i] = t;
	}

#endif

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(1000, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Jonathan");

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



void render()
{





	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(0.5f);
	glClear(GL_COLOR_BUFFER_BIT);
	//glColor3f(0,1.0,0);
	glEnable(GL_COLOR);
	//glColor3f(38 / 255.0, 188 / 255.0, 213 / 255.0);
	glBegin(GL_TRIANGLES);



	for (int i = 0; i < mom.m_faces_index_v.size(); i++)
	{

		//std::cout << i << " ";
		//if (i == 259)
		//	std::cout << "sdf" << std::endl;
		for (int j = 0; j < 3; j++)
		{
			//std::cout << j << " ";
			int index_tex = mom.m_faces_index_vt[i][j]-1;
			int index_vert = mom.m_faces_index_v[i][j]-1;

			
			glTexCoord2f(mom.m_vert_tex[index_tex][0], 1.0-mom.m_vert_tex[index_tex][1]);

			//no normal

			glVertex3f(mom.m_vertices[index_vert][0], mom.m_vertices[index_vert][1], mom.m_vertices[index_vert][2]);
		}

		//std::cout << std::endl;
	}


	//std::cout << "down" << std::endl;
	glEnd();

#ifdef SHOW_SKELETON
	float interval = 13.0;
	glColor3f(0 / 255.0, 188 / 255.0, 213 / 255.0);
	//glEnable(GL_COLOR);
	glLineWidth(6.0);
	glBegin(GL_LINES);
	//glVertex3f(plp.cur_joint_pose_R[0].x + interval, plp.cur_joint_pose_R[0].y + 5, plp.cur_joint_pose_R[0].z);
	//glVertex3f(plp.cur_joint_pose_R[0].x + interval, plp.cur_joint_pose_R[0].y, plp.cur_joint_pose_R[0].z);

	//glVertex3f(plp.cur_joint_pose_L[0].x + interval, plp.cur_joint_pose_L[0].y + 5, plp.cur_joint_pose_L[0].z);
	//glVertex3f(plp.cur_joint_pose_L[0].x + interval, plp.cur_joint_pose_L[0].y, plp.cur_joint_pose_L[0].z);
	for (int i = 0; i < 5; i++)
	{
		glVertex3f(plp.cur_joint_pose_R[0].x + interval, plp.cur_joint_pose_R[0].y, plp.cur_joint_pose_R[0].z);
		glVertex3f(plp.cur_joint_pose_R[i * 4 + 1].x + interval, plp.cur_joint_pose_R[i * 4 + 1].y, plp.cur_joint_pose_R[i * 4 + 1].z);

		glVertex3f(plp.cur_joint_pose_R[i * 4 + 1].x + interval, plp.cur_joint_pose_R[i * 4 + 1].y, plp.cur_joint_pose_R[i * 4 + 1].z);
		glVertex3f(plp.cur_joint_pose_R[i * 4 + 2].x + interval, plp.cur_joint_pose_R[i * 4 + 2].y, plp.cur_joint_pose_R[i * 4 + 2].z);

		glVertex3f(plp.cur_joint_pose_R[i * 4 + 2].x + interval, plp.cur_joint_pose_R[i * 4 + 2].y, plp.cur_joint_pose_R[i * 4 + 2].z);
		glVertex3f(plp.cur_joint_pose_R[i * 4 + 3].x + interval, plp.cur_joint_pose_R[i * 4 + 3].y, plp.cur_joint_pose_R[i * 4 + 3].z);

		glVertex3f(plp.cur_joint_pose_R[i * 4 + 3].x + interval, plp.cur_joint_pose_R[i * 4 + 3].y, plp.cur_joint_pose_R[i * 4 + 3].z);
		glVertex3f(plp.cur_joint_pose_R[i * 4 + 4].x + interval, plp.cur_joint_pose_R[i * 4 + 4].y, plp.cur_joint_pose_R[i * 4 + 4].z);


		glVertex3f(plp.cur_joint_pose_L[0].x + interval, plp.cur_joint_pose_L[0].y, plp.cur_joint_pose_L[0].z);
		glVertex3f(plp.cur_joint_pose_L[i * 4 + 1].x + interval, plp.cur_joint_pose_L[i * 4 + 1].y, plp.cur_joint_pose_L[i * 4 + 1].z);

		glVertex3f(plp.cur_joint_pose_L[i * 4 + 1].x + interval, plp.cur_joint_pose_L[i * 4 + 1].y, plp.cur_joint_pose_L[i * 4 + 1].z);
		glVertex3f(plp.cur_joint_pose_L[i * 4 + 2].x + interval, plp.cur_joint_pose_L[i * 4 + 2].y, plp.cur_joint_pose_L[i * 4 + 2].z);

		glVertex3f(plp.cur_joint_pose_L[i * 4 + 2].x + interval, plp.cur_joint_pose_L[i * 4 + 2].y, plp.cur_joint_pose_L[i * 4 + 2].z);
		glVertex3f(plp.cur_joint_pose_L[i * 4 + 3].x + interval, plp.cur_joint_pose_L[i * 4 + 3].y, plp.cur_joint_pose_L[i * 4 + 3].z);

		glVertex3f(plp.cur_joint_pose_L[i * 4 + 3].x + interval, plp.cur_joint_pose_L[i * 4 + 3].y, plp.cur_joint_pose_L[i * 4 + 3].z);
		glVertex3f(plp.cur_joint_pose_L[i * 4 + 4].x + interval, plp.cur_joint_pose_L[i * 4 + 4].y, plp.cur_joint_pose_L[i * 4 + 4].z);


	}


	glEnd();
	glPointSize(10.0);
	glBegin(GL_POINTS);

	//glVertex3f(plp.cur_joint_pose_R[0].x + interval, plp.cur_joint_pose_R[0].y + 5, plp.cur_joint_pose_R[0].z);
	//glVertex3f(plp.cur_joint_pose_L[0].x + interval, plp.cur_joint_pose_L[0].y + 5, plp.cur_joint_pose_L[0].z);

	for (int i = 0; i < plp.cur_joint_pose_R.size(); i++)
	{
		glVertex3f(plp.cur_joint_pose_R[i].x + interval, plp.cur_joint_pose_R[i].y, plp.cur_joint_pose_R[i].z);
		glVertex3f(plp.cur_joint_pose_L[i].x + interval, plp.cur_joint_pose_L[i].y, plp.cur_joint_pose_L[i].z);
	}

	glEnd();
#endif

}


void showInfo()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 511, 0, 511, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.6, 0.7, 0.9);

	char sz[128];
	int fontHeight = 12, line = 512;
	sprintf(sz, "vertices: %d", mom.m_vertices.size());
	print(2, line, sz);
	line -= (fontHeight + 2);

	sprintf(sz, "triangles: %d", mom.m_faces_index_normal.size());
	print(2, line, sz);
	line -= (fontHeight + 2);

	sprintf(sz, "hand_id: %d ", 1);
	print(2, line, sz);
	line -= (fontHeight + 2);

	sprintf(sz, "trans_XYZ: %.3f %.3f %.3f", x_trans, y_trans, z_trans);
	print(2, line, sz);
	line -= (fontHeight + 2);

	sprintf(sz, "joints: %d", 21);
	print(2, line, sz);
	line -= (fontHeight + 2);



	line = (fontHeight + 2);
	sprintf(sz, "Thumb:  %.3f %.3f %.3f %.3f %.3f!",dof_R[0], dof_R[1], dof_R[2], dof_R[3], dof_R[4]);
	print(2, line, sz);


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void print(int x, int y, const char *string)
{
	size_t len = (int)strlen(string);
	int fontHeight = 12;
	glDisable(GL_LIGHTING);
	glColor3f(0.6, 0.7, 0.9);
	for (size_t i = 0; i < len; i++)
	{
		glRasterPos2i(x, y - fontHeight);
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
		x += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
}



GLuint ImportTextures(const char* file_name)
{
	//modelLoader.Load(modelName.c_str());
	//modelLoader.SetupJoints();
	//modelLoader.SetFrame(0);
	//modelLoader.reAlignJoint();
	//const char* file_name = "./res/web/lj.bmp";


	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLint last_texture_ID;
	GLuint texture_ID = 0;



	//打开文件，如果失败，返回
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;



	//读取文件中图象的宽度和高度

	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);


	//计算每行像素所占字节数，并根据此数据计算总像素字节数
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}



	//根据总像素字节数分配内存
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}



	// 读取像素数据

	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}


	// 在旧版本的OpenGL中
	 //如果图象的宽度和高度不是的整数次方，则需要进行缩放
	 //这里并没有检查OpenGL版本，出于对版本兼容性的考虑，按旧版本处理
	 //另外，无论是旧版本还是新版本，
	 //当图象的宽度和高度超过当前OpenGL实现所支持的最大值时，也要进行缩放

	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		//printf("\n-%d-\n", max);
		if (false/*!power_of_two(width)|| !power_of_two(height)|| width > max|| height > max*/)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;
			// 计算每行需要的字节数和总字节数
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// 分配内存
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// 进行像素缩放
			gluScaleImage(GL_RGB,width, height, GL_UNSIGNED_BYTE, pixels, new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);
			// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}

	}



	//分配一个新的纹理编号

	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	//绑定新的纹理，载入纹理并设置纹理参数
	//在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	//printf("%d%d", width, height);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);
	// 之前为pixels分配的内存可在使用glTexImage2D以后释放

	 //因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）

	free(pixels);
	return texture_ID;
}




void grab(void)

{

	FILE*    pDummyFile;
	FILE*    pWritingFile;
	GLubyte* pPixelData;
	GLubyte  BMP_Header[BMP_Header_Length];
	GLint    i, j;
	GLint    PixelDataLength;

	//计算像素数据的实际长度
	i = screenWidth * 3;   // 得到每一行的像素数据长度
	while (i % 4 != 0)      // 补充数据，直到i是的倍数
		++i;              // 本来还有更快的算法，
	//但这里仅追求直观，对速度没有太高要求
	PixelDataLength = i * screenHeight;
	//分配内存和打开文件
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);
	pDummyFile = fopen("./res/lj/lj.bmp", "rb");
	if (pDummyFile == 0)
		exit(0);
	std::string img_id = std::to_string(save_img_id++);
	if (save_img_id > 888)
		save_img_id = 0;
	std::string pre = "./imgs/";
	std::string typ = ".bmp";
	img_id = pre + img_id+typ;
	
	pWritingFile = fopen(img_id.c_str(), "wb");
	if (pWritingFile == 0)
		exit(0);

	// 读取像素

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(0, 0, screenWidth, screenHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);


	// 把dummy.bmp的文件头复制为新文件的文件头
	fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET);
	i = screenWidth;
	j = screenHeight;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 写入像素数据
	fseek(pWritingFile, 0, SEEK_END);
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);
	//释放内存和关闭文件
	fclose(pDummyFile);
	fclose(pWritingFile);
	free(pPixelData);
}


void init(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	texhand=ImportTextures("./res/lj/lj.bmp");

	glViewport(0, 0, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth(10.0);

	glPushMatrix();
	glTranslated(x_trans, y_trans, z_trans);
	glScalef(scale, scale, scale);
	glRotated(x_angle, 1.0, 0.0, 0.0);
	glRotated(y_angle, 0.0, 1.0, 0.0);

	//glTranslated(-0.01, 0, 0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);


	//texhand = ImportTextures();
	glBindTexture(GL_TEXTURE_2D, texhand);
	render();
	//modelLoader.Render(false, false);
	glDisable(GL_TEXTURE_2D);
	//modelLoader.RenderJoint();
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
	showInfo();
	//renderWord();

	glFlush();
	glutSwapBuffers();
	grab();
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
		y_angle += dx * 0.2f;
		x_angle += dy * 0.2f;
	}
	display();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {


	case ' ': {
		//vector<float>df = DOF::randDOF();
		//sdof = df;
		//modelLoader.getRandomAngle(sdof);
		index_tex++;
		if (index_tex >= MAX_NUM_TEX)
			index_tex = index_tex % MAX_NUM_TEX;
		std::string s("./res/texs/");
		s = s + std::to_string(index_tex)+std::string(".bmp");
		texhand = ImportTextures(s.c_str());
		break;
	}

	case 'p': {
		//vector<float> df = DOF::randDOF();
		//RAND_MAX
		/*std::vector<float> */
		dof_R = plp.rand_dof();
		plp.random_pose_R(dof_R,mom.m_vertices);


		dof_L = plp.rand_dof();
		plp.random_pose_L(dof_L, mom.m_vertices);


		//for (int i = 0; i < plp.cur_vertexs.size(); i++)
		//{
		//	glm::vec3 t(plp.cur_vertexs[i]);
		//	mom.m_vertices[i] =t;// glm::vec3({ plp.cur_vertexs[0], plp.cur_vertexs[1], plp.cur_vertexs[2] });
		//}
		

		std::cout << "random pose" << std::endl;
		//sdof[0] += pi / 10;
		//sdof[1] += pi / 10;
		//sdof[2] += pi / 10;
		//sdof[3] += pi / 10;

		//modelLoader.getRandomAngle(sdof);
		break;


	}
	case 'r':
	{
		index_dof_R = 0;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 't':
	{
		index_dof_R = 1;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'y':
	{
		index_dof_R = 2;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'u':
	{
		index_dof_R = 3;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'i':
	{
		index_dof_R = 4;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '9':
	{
		plp.dis_LR -= 1;
		if (plp.dis_LR < 2.0)
			plp.dis_LR = 25.0;
		plp.random_pose_R(dof_R, mom.m_vertices);
		plp.random_pose_L(dof_L, mom.m_vertices);
	}

	case 'o':
	{
		//index_dof_R = 4;
		//step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		//if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
		//	dof_R[index_dof_R] += step;
		//else
		//	dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		/*plp.random_pose_R(dof_R, mom.m_vertices);*/
		step = PI / 20;

		global_R = plp.eulerAngles2matrix({step, 0, 0});

        for (int i = 0; i < mom.m_vertices.size()/2; i++)
	        mom.m_vertices[i] = glm::vec4(mom.m_vertices[i].x, mom.m_vertices[i].y, mom.m_vertices[i].z,1)*global_R;

		//global_R = plp.eulerAngles2matrix({ step, global_dof[1], global_dof[2] });
		for (int i = 0; i < plp.cur_joint_pose_R.size(); i++)
      	    plp.cur_joint_pose_R[i] =  plp.cur_joint_pose_R[i]*global_R;

		break;
	}

	case 'k':
	{
		step = PI / 20;

		global_R = plp.eulerAngles2matrix({ 0, step, 0 });
		for (int i = 0; i < mom.m_vertices.size()/2; i++)
			mom.m_vertices[i] = glm::vec4(mom.m_vertices[i].x, mom.m_vertices[i].y, mom.m_vertices[i].z, 1)*global_R;

		for (int i = 0; i < plp.cur_joint_pose_R.size(); i++)
			plp.cur_joint_pose_R[i] = plp.cur_joint_pose_R[i] * global_R;
		break;
	}
	case 'l':
	{
		step = PI / 20;

		global_R = plp.eulerAngles2matrix({ 0, 0, step });

		for (int i = 0; i < mom.m_vertices.size()/2; i++)
			mom.m_vertices[i] = glm::vec4(mom.m_vertices[i].x, mom.m_vertices[i].y, mom.m_vertices[i].z, 1)*global_R ;

		for (int i = 0; i < plp.cur_joint_pose_R.size(); i++)
			plp.cur_joint_pose_R[i] = plp.cur_joint_pose_R[i] * global_R;
		break;
	}



	case 'O':
	{
		//index_dof_R = 4;
		//step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		//if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
		//	dof_R[index_dof_R] += step;
		//else
		//	dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		/*plp.random_pose_R(dof_R, mom.m_vertices);*/
		step = PI / 20;

		global_L = plp.eulerAngles2matrix({ step, 0, 0 });

		for (int i = 0; i < mom.m_vertices.size() / 2; i++)
			mom.m_vertices[i+ mom.m_vertices.size() / 2] = glm::vec4(mom.m_vertices[i + mom.m_vertices.size() / 2].x, mom.m_vertices[i + mom.m_vertices.size() / 2].y, mom.m_vertices[i + mom.m_vertices.size() / 2].z, 1)*global_L;

		//global_R = plp.eulerAngles2matrix({ step, global_dof[1], global_dof[2] });
		for (int i = 0; i < plp.cur_joint_pose_L.size(); i++)
			plp.cur_joint_pose_L[i] = plp.cur_joint_pose_L[i] * global_L;

		break;
	}

	case 'K':
	{
		step = PI / 20;

		global_L = plp.eulerAngles2matrix({ 0, step, 0 });
		for (int i = 0; i < mom.m_vertices.size() / 2; i++)
			mom.m_vertices[i+ mom.m_vertices.size() / 2] = glm::vec4(mom.m_vertices[i + mom.m_vertices.size() / 2].x, mom.m_vertices[i + mom.m_vertices.size() / 2].y, mom.m_vertices[i + mom.m_vertices.size() / 2].z, 1)*global_L;

		for (int i = 0; i < plp.cur_joint_pose_L.size(); i++)
			plp.cur_joint_pose_L[i] = plp.cur_joint_pose_L[i] * global_L;
		break;
	}
	case 'L':
	{
		step = PI / 20;

		global_L = plp.eulerAngles2matrix({ 0, 0, step });

		for (int i = 0; i < mom.m_vertices.size() / 2; i++)
			mom.m_vertices[i + mom.m_vertices.size() / 2] = glm::vec4(mom.m_vertices[i + mom.m_vertices.size() / 2].x, mom.m_vertices[i + mom.m_vertices.size() / 2].y, mom.m_vertices[i + mom.m_vertices.size() / 2].z, 1)*global_L;

		for (int i = 0; i < plp.cur_joint_pose_L.size(); i++)
			plp.cur_joint_pose_L[i] = plp.cur_joint_pose_L[i] * global_L;
		break;
	}

	case 'f':
	{
		index_dof_R = 5;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'g':
	{
		index_dof_R = 7;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'h':
	{
		index_dof_R = 8;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'j':
	{
		index_dof_R = 9;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}




	case 'v':
	{
		index_dof_R = 10;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'b':
	{
		index_dof_R = 12;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'n':
	{
		index_dof_R = 13;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case 'm':
	{
		index_dof_R = 14;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}



	case '1':
	{
		index_dof_R = 15;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '2':
	{
		index_dof_R = 17;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '3':
	{
		index_dof_R = 18;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '4':
	{
		index_dof_R = 19;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}



	case '5':
	{
		index_dof_R = 20;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '6':
	{
		index_dof_R = 22;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '7':
	{
		index_dof_R = 23;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}
	case '8':
	{
		index_dof_R = 24;
		step = (plp.dof_max_limits[index_dof_R] - plp.dof_min_limits[index_dof_R]) / 20.0;
		if (dof_R[index_dof_R] < plp.dof_max_limits[index_dof_R])
			dof_R[index_dof_R] += step;
		else
			dof_R[index_dof_R] = plp.dof_min_limits[index_dof_R];
		plp.random_pose_R(dof_R, mom.m_vertices);
		break;
	}

	case 'w': {
		y_trans += 0.01;
		break;
	}

	case 'x': {
		y_trans -= 0.01;
		break;
	}
	case 'q': {
		x_trans -= 0.01;
		break;
	}
	case 'e': {
		x_trans += 0.01;
		break;

	}


	case 's': {
		char filename[128] = { 0 };
		for (int i = 0; i < 50; i++) {
			//vector<float>df = DOF::randDOF();
			//modelLoader.getRandomAngle(df);
			sprintf(filename, "./model/hand_model%d.obj", cnt);
			//modelLoader.saveOBJ(filename);
			sprintf(filename, "./model/hand_model%d.txt", cnt++);
			//modelLoader.saveJoint(filename);
		}
		break;
	}
	case 'a': {
		//modelLoader.setJointArea();
		z_trans += 0.01;
		break;
	}
	case 'd': {
		//modelLoader.reset();
		z_trans -= 0.01;
		break;
	}
	case 'z': {
		scale += 0.02f;
		break;
	}
	case 'c': {
		if (scale > 0.02f)
			scale -= 0.02f;
		break;
	}
	}
	display();
}