#include "stdafx.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <opencv2/opencv.hpp>

#include "GlHelper.h"

GlHelper* GlHelper::s_glh = NULL;

GlHelper::GlHelper(): fbo(0), depthbuffer(0), rendertarget(0)
{
}

GlHelper::~GlHelper()
{
}

// 初始化摄像机
void GlHelper::setupCamera(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);

	// 各种变换应该在GL_MODELVIEW模式下进行
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// 启用2D贴图
	glEnable(GL_TEXTURE_2D);
}

// 初始化几何形体
void GlHelper::setupResource(void)
{
	// 创建纹理
	glGenTextures(1, &rendertarget);
	glBindTexture(GL_TEXTURE_2D, rendertarget);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// 创建深度缓冲区
	glGenRenderbuffersEXT(1, &depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// 创建FBO对象
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rendertarget, 0);
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		std::cout << "Failed to create framebuffer ! " << std::endl;
	}
}

// 渲染到窗体
void GlHelper::render(void)
{
	// 绑定默认FBO（窗体帧缓冲区的ID是0）
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, rendertarget);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// 渲染
	glClearColor(0, 0, 1, 0);	// 蓝色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// --------------------------------------------------------
	glBegin(GL_POLYGON);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);	// 白色
						// 矩形
	glTexCoord2f(1, 1);
	glVertex3d(1, 1, 0);

	glTexCoord2f(0, 1);
	glVertex3d(-1, 1, 0);

	glTexCoord2f(0, 0);
	glVertex3d(-1, -1, 0);

	glTexCoord2f(1, 0);
	glVertex3d(1, -1, 0);
	glEnd();
	// --------------------------------------------------------

	// ☆
	if (1 == which_frame_buffer)
		saveCVImage("../image_1.bmp");	// 切记在glutSwapBuffer之前glReadPixels

}

// 渲染到纹理
void GlHelper::renderToTarget(void)
{
	glBindTexture(GL_TEXTURE_2D, 0); // 取消绑定，因为如果不取消，渲染到纹理的时候会使用纹理本身

									 // 绑定渲染目标
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);

	// 渲染
	glClearColor(1, 1, 0, 1);	// 黄色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// --------------------------------------------------------
	glBegin(GL_POLYGON);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 红色三角形
	glColor4f(1, 0, 0, 1);
	glVertex3d(0, 1, 0);
	glVertex3d(-1, -1, 0);
	glVertex3d(1, -1, 0);

	glEnd();
	// --------------------------------------------------------
	// ☆
	if(0 == which_frame_buffer)
		saveCVImage("../image_0.bmp");	// 切记在glutSwapBuffer之前glReadPixels

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

// 读取当前绑定的FrameBuffer，并保存图片
void GlHelper::saveCVImage(const char *file_name)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int width = viewport[2];
	int height = viewport[3];

	// 设置像素的对齐格式——Word对齐(4字节)
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	// 计算对齐后的真实宽度
	int nAlignWidth = (width * 24 + 31) / 32;
	// 分配缓冲区
	unsigned char *pdata = new unsigned char[nAlignWidth * height * 4];
	memset(pdata, 0, nAlignWidth * height * 4);

	// 从当前绑定的 frame buffer 读取 pixels  
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pdata);

	cv::Mat src = cv::Mat(height, width, CV_8UC3, pdata);
	cv::Mat bgr;
	cvtColor(src, bgr, CV_RGB2BGR);
	cv::Mat dst;
	flip(bgr, dst, 0);	// 0:x; 1:y, -u:x,y;
	imwrite(file_name, dst);
}

GlHelper* GlHelper::getSingleInstance()
{
	if (NULL == s_glh)
	{
		s_glh = new GlHelper();
	}
	else
		return s_glh;
}
