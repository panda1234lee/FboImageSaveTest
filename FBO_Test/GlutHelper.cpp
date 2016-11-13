#include "stdafx.h"

#include <GL/glew.h>
#include<GL/freeglut.h>

#include "GlutHelper.h"

GlHelper* GlutHelper::s_glh = NULL;

GlutHelper::GlutHelper()
{
}

GlutHelper::~GlutHelper()
{
}

void GlutHelper::setupWindow(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Hello Framebuffer !");

	
}

void GlutHelper::setupOthers()
{
	GLenum err = glewInit(); // GLEW的初始化必须在OpenGL上下文被创建之后调用

	s_glh->setupCamera();
	s_glh->setupResource();
}

void GlutHelper::display()
{
	glutDisplayFunc(renderScene);
}

void GlutHelper::loop()
{
	glutMainLoop();
}

void GlutHelper::setGlHelper(GlHelper* glh)
{
	s_glh = glh;
}

void GlutHelper::renderScene()
{
	if (NULL == s_glh)
		return;
	s_glh->renderToTarget();
	s_glh->render();
	glutSwapBuffers();
}
