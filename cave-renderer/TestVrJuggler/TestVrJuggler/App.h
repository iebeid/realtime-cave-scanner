#ifndef _APP_H
#define _APP_H

#include "Model.h"

using namespace vrj;

class MeshApp : public opengl::App
{

public:

	MeshApp()
	{
		;
	}

	virtual ~MeshApp(void) { ; }

	virtual void init()
	{
		std::cout << "---------- App:init() ---------------" << std::endl;
		mWand.init("VJWand");
		mHead.init("VJHead");
		mForwardButton.init("VJButton0");
		mRotateButton.init("VJButton1");
		mGrabButton.init("VJButton2");
	}

	virtual void apiInit()
	{
		;
	}

	virtual void bufferPreDraw();

	virtual void preFrame(){ ; }

	virtual void intraFrame()
	{
		;
	}

	virtual void postFrame(){ ; }

	virtual void contextInit()
	{
		initGLState();
	}

	virtual void draw();

	virtual void reset();

	virtual void contextClose(){ ; }

	void loadObject();

	void UpdateX();

	void UpdateTimer();

private:

	void initGLState();

protected:

	void updateGrabbing();

	void updateNavigation();

public:

	gadget::PositionInterface  mWand;
	gadget::PositionInterface  mHead;
	gadget::DigitalInterface  mForwardButton;
	gadget::DigitalInterface  mRotateButton;
	gadget::DigitalInterface  mGrabButton;
	gadget::DigitalInterface  mDumpStateButton;
	
	Model current_model;
	//std::vector<Model> v;
	unsigned mFramesToSleep;
	Shader* shader;
	Shader* post_shader;
	GLuint g_fb = 0;
	GLuint g_fb_tex = 0;
	GLuint g_ss_quad_vao = 0;
	int x_origin;
	int y_origin;
	int width;
	int height;
	bool v_updated;
	GLuint VAO, VBO, EBO;
	clock_t time0, time1;
	float timer010;
	bool bUp;
	GLfloat x_position;

};
#endif