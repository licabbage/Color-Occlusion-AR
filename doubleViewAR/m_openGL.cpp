

#include "m_openGL.h"
#include"m_easyAR.h"

GLuint cameraTex = 0;	//ÉãÏñ»ú²¶»ñÍ¼ÏñÎÆÀí
int windowSizeX = 640, windowSizeY = 480;	//´°¿Ú³ß´ç

int leftwindow, rightwindow;
float modelview_matrix[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
float projection_matrix[16] = { 1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0 };
using namespace std;

std::shared_ptr<easyar::Frame> frame;

int mainWindow;




void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3ub(255, 255, 255);

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, cameraTex);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 1); glVertex2f(-1, -1);
	glTexCoord2f(1, 1); glVertex2f(1, -1);
	glTexCoord2f(1, 0); glVertex2f(1, 1);
	glTexCoord2f(0, 0); glVertex2f(-1, 1);
	glEnd();


	glEnable(GL_DEPTH_TEST);
	//glDrawPixels(windowSizeX, windowSizeY, GL_BGR_EXT, GL_UNSIGNED_BYTE,frameMat.ptr());
	
		
	glMatrixMode(GL_PROJECTION);
	getProjection(projection_matrix);
	glLoadMatrixf(projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(modelview_matrix);

	//glTranslatef(5, 0, 0);

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 100);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(100, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 100, 0);
	glEnd();


	glColor3f(1, 0, 0);
	glTranslatef(0.0, 1.0, 0.0);
	glRotated(90, 1, 0, 0);
	
	glutWireTeapot(0.4);
	
	//glFlush();
	

	Mat frameMat = Frame2Mat(frame);
	Mat skin = HSVBin(frameMat);
	auto validContours = getContours(skin);
	
	GLubyte* BUFFER = (GLubyte*)malloc(windowSizeX * windowSizeY * 3);
	
	glReadPixels(0,0,windowSizeX, windowSizeY,GL_BGR_EXT,GL_UNSIGNED_BYTE,BUFFER);

	Mat bgMat;
	bgMat.create(windowSizeY, windowSizeX,CV_8UC3);
	bgMat.data = BUFFER;
	Mat normalBgMat;
	flip(bgMat, normalBgMat, 0);
	

	for (int i = 0; i < skin.rows; i++)
	{
		for (int j = 0; j < skin.cols; j++)
		{
			if (skin.at<uchar>(i, j) != 0)
			{									
				normalBgMat.at<Vec3b>(i, j) = frameMat.at<Vec3b>(i, j);				
			}
		}
	}
	
	/*imshow("dsdsd", normalBgMat);
	imshow("nenen", skin);*/

	//for (int i = 0; i < skin.rows; i++)
	//{
	//	for (int j = 0; j < skin.cols; j++)
	//	{
	//		if (pointPolygonTest(validContours, Point(i, j), false) < 0)
	//		{
	//			/*normalBgMat.at<Vec3b>(i, j)[0] = 0;
	//			normalBgMat.at<Vec3b>(i, j)[1] = 0;
	//			normalBgMat.at<Vec3b>(i, j)[2] = 0;*/
	//		}
	//	}
	//}
	imshow("dsdsd", normalBgMat);
	free(BUFFER);

	glutSwapBuffers();
}

void onTime(int id)
{
	switch (id)
	{
	case 1:
	{
		frame = m_nextFrame(cameraTex, modelview_matrix);
		
		
		glutPostRedisplay();
		glutTimerFunc(33, onTime, 1);
	}
		break;
	default:
		break;
	}
}


void init()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	
	glShadeModel(GL_SMOOTH);

}



void initOpenGL(int argc, char *argv[])
{
	glutInit(&argc, __argv);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(windowSizeX, windowSizeY);
	glutInitDisplayMode(GL_RGB | GL_DOUBLE);
	
	glutTimerFunc(50, onTime, 1);

	mainWindow = glutCreateWindow("main window");
	glutDisplayFunc(&display);
	init();

	
}
