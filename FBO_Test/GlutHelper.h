#pragma once
#include "GlHelper.h"

class GlutHelper
{
public:
	GlutHelper();
	~GlutHelper();
	void setupWindow(int argc, char *argv[]);
	void setupOthers();
	void display();
	void keyboardCallback();
	void loop();
	void setGlHelper(GlHelper* glh);

	static void processNormalKeys(unsigned char key, int x, int y);
	static void renderScene();

private:
	static GlHelper* s_glh;

};

