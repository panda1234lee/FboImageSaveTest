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
	void loop();
	void setGlHelper(GlHelper* glh);

	static void renderScene();

private:
	static GlHelper* s_glh;

};

