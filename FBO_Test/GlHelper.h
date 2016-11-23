#pragma once

class GlHelper
{
public:
	~GlHelper();
	
	void setupCamera(void);
	void setupResource(void);
	void render(void);
	void renderToTarget(void);
	void saveCVImage(const char *file_name);

	static GlHelper* getSingleInstance();

private:
	GlHelper();

public:

	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int TEXTURE_WIDTH = 512;
	const int TEXTURE_HEIGHT = 512;
	const double NEAR_PLANE = 1.0f;
	const double FAR_PLANE = 1000.0f;

	int which_frame_buffer; // 0: 自定义的framebuffer， 1：系统framebuffer，其他则不截图

private:

	unsigned int fbo ;							// FBO对象的句柄
	unsigned int depthbuffer ;
	unsigned int rendertarget ;			// 纹理对象的句柄

	static GlHelper* s_glh;
};

