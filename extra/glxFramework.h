#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <string>

class GlxFramework
{
public:
	typedef GLXContext (*GLXCREATECONTEXTATTRIBSARBPROC)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

public:
	enum Buttons {
		kLeftButton = 0,
		kMidButton = 1,
		kRightButton = 2
	};
	enum ModifierKeys {
		kNoModifiers = 0,
		kShift = 1,
		kAlt   = 2,
		kCtrl  = 4
	};
	enum CameraMode
	{
		kTumble,
		kTrack,
		kZoom,
		kPassive
	} ;
	enum ViewMode
	{
		kPerspective,
		kFront,
		kLeft,
		kRight,
		kTop
	} ;

private:
	//-------------------------------------------------------- glxContext
	bool        running_;
	Display*    display_;
	Window      window_;
	int         width_;
	int         height_;
	GLXContext  ctx_;
	
	//-------------------------------------------------------- camera configuration
	CameraMode state_;
	ViewMode   viewMode_;
	
	double eye_[3];			// longitude / latitude / distance or x / y / z
	double target_[3];
	double up_[3];
	
	double track_x_;
	double track_y_;
	
	double	x0_;
	double	y0_;
	
	//-------------------------------------------------------- camera perspective parameters
	double fovy_;
	double ratio_;
	double znear_;
	double zfar_;
	
	//-------------------------------------------------------- camera orthogonal parameters
	double left_;
	double right_;
	double bottom_;
	double top_;
	
	//-------------------------------------------------------- viewport configuration (0x0001 : , 0x0002 : , 0x0004 : )
	size_t      displayMode_;
	
	bool		displayAxis_ = true;
	bool		displayGrid_;
	bool		active_;
	//-------------------------------------------------------- backgorund color
	float background_color;
	
	//-------------------------------------------------------- light configuration
	
	//-------------------------------------------------------- transformation matricies
	//float projection_[16];
	//float modelview_[16];
	
	int 	viewport_[4];
	float 	projectionMtx_[16];
	float 	modelViewMtx_[16];
	
	double  fps_;
	double  shutterAngle_;
	double  minFrame_;
	double  maxFrame_;
	double  frame_;

public:
	GlxFramework()
	{
		//-------------------------------------------------------- window title
		//-------------------------------------------------------- window position
		//this->window_xpos_ = 0;
		//this->window_ypos_ = 115;
		
		//-------------------------------------------------------- window size
		//const int kWindow_xpos	= 1700;
		//const int kWindow_ypos	= 40;
		//this->window_width_ = 770 ;
		//this->window_height_ = 500 ;
		//this->window_width_ = 1000;
		//this->window_height_ = 800;
		
		//-------------------------------------------------------- camera configuration
		state_    = kPassive;
		viewMode_ = kPerspective;
		//-------------------------------------------------------- camera perspective configuration
		eye_[0] = 90.0;
		eye_[1] = 0.0;
		eye_[2] = 25.0;
		target_[0] = 0.0;
		target_[1] = 0.0;
		target_[2] = 0.0;
		up_[0] = 0.0;
		up_[1] = 1.0;
		up_[2] = 0.0;
		
		track_x_ = 0.0;
		track_y_ = 0.0;
		
		x0_ = 640 / 2.0;
		y0_ = 480 / 2.0;
		
		fovy_ = 45.0;
		ratio_ = 1.0;
		znear_ = 0.01;
		zfar_ = 10000.0;
		
		//-------------------------------------------------------- camera orthogonal configuration
		right_= -15.0;
		left_ = 15;
		bottom_ = -15.0;
		top_ = 15.0;
		
		
		//-------------------------------------------------------- viewport configuration
		displayMode_ = 0xffff;
		displayAxis_ = true;
		displayGrid_ = true;
		active_ = true;
		//-------------------------------------------------------- background color
		background_color = 0.15f;
		
		//-------------------------------------------------------- light configuration
		//this->light_ambient_[0] = 0.3f;
		//this->light_ambient_[1] = 0.3f;
		//this->light_ambient_[2] = 0.3f;
		//this->light_ambient_[3] = 1.0f;
		//this->light_color_[0] = 0.3f;
		//this->light_color_[1] = 0.3f;
		//this->light_color_[2] = 0.3f;
		//this->light_color_[3] = 1.0f;
		//this->light_position_[0] = 0.0f;
		//this->light_position_[1] = 0.0f;
		//this->light_position_[2] = 15.0f;
		//this->light_position_[3] = 1.3f;
		
		fps_          = 24.0;
		shutterAngle_ = 360.0;
		minFrame_     = 1.0;
		maxFrame_     = 24.0;
		frame_        = 1.0;
	}

public:
	virtual bool init(const std::string& title = "test",
	          const int majorVersion = 4,
	          const int minorVersion = 3,
	          const bool coreProfile = true,
	          const bool directRendering = true,
	          const int width  = 640,
	          const int height = 480,
	          int nSamples = 1
	)
	{
		int attrib[] = {
				GLX_RENDER_TYPE,    GLX_RGBA_BIT,
				GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
				GLX_DOUBLEBUFFER,   True,
				GLX_RED_SIZE,       8,
				GLX_GREEN_SIZE,     8,
				GLX_BLUE_SIZE,      8,
				GLX_DEPTH_SIZE,     24,
				GLX_SAMPLE_BUFFERS, (nSamples > 1 ? 1 : 0),
				GLX_SAMPLES,        nSamples,
				None
		};
		
		display_    = XOpenDisplay(NULL);
		
		int     screen  = DefaultScreen(display_);
		Window  root    = RootWindow(display_, screen);
		
		// X window
		int fbcount;
		GLXFBConfig *fbc = glXChooseFBConfig(display_, screen, attrib, &fbcount);
		if (!fbc)
		{
			printf("Failed to get glXChooseFBConfig");
			return false;
		}
		
		XVisualInfo *visinfo = glXGetVisualFromFBConfig(display_, fbc[0]);
		if (!visinfo)
		{
			printf("Failed to get glXGetVisualFromFBConfig");
			return false;
		}
		
		XSetWindowAttributes attr;
		attr.background_pixel = 0;
		attr.border_pixel     = 0;
		attr.colormap         = XCreateColormap(display_, root, visinfo->visual, AllocNone);
		attr.event_mask = StructureNotifyMask |
		                  ExposureMask        |
		                  KeyPressMask        |
		                  KeyReleaseMask      |
		                  PointerMotionMask   |
		                  ButtonPressMask     |
		                  ButtonReleaseMask;
		
		window_ = XCreateWindow(display_,
		                        root,
		                        0,
		                        0,
		                        width,
		                        height,
		                        0,
		                        visinfo->depth,
		                        InputOutput,
		                        visinfo->visual,
		                        CWBackPixel|CWBorderPixel|CWColormap|CWEventMask,
		                        &attr
		);
		XStoreName(display_, window_, title.c_str());
		
		// GL context
		ctx_ = glXCreateContext(display_, visinfo, nullptr, directRendering);
		glXMakeCurrent(display_, window_, ctx_);
		
		//TODO::not working
		// set GL context
		//int attribs[] = { 0 };
		//const int profile = coreProfile ? GLX_CONTEXT_CORE_PROFILE_BIT_ARB : GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
		//int attribs[] = {
		//		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		//		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		//		GLX_CONTEXT_PROFILE_MASK_ARB,  profile,
		//		GLX_CONTEXT_FLAGS_ARB,
		//		//GLX_CONTEXT_DEBUG_BIT_ARB,
		//		0,
		//};
		//
		//GLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (GLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress( (const GLubyte*)"glXCreateContextAttribsARB");
		//if(glXCreateContextAttribsARB)
		//{
		//	ctx_ = glXCreateContextAttribsARB(display_,
		//			                          fbc[0],
		//			                          0,
		//			                          directRendering,
		//			                          attribs
		//	);
		//	glXMakeCurrent(display_, window_, ctx_);
		//}
		//else
		//{
		//	STDOUT_ERROR("Failed to create Context Attribs");
		//	return false;
		//}
		onInitializeGL_();
		
		return true;
	}
	void run()
	{
		if (!display_)
			return;
		
		XMapWindow(display_, window_);
		
		running_ = true;
		XEvent event;
		while (running_ )
		{
			while (XPending(display_))
			{
				XNextEvent(display_, &event);
				
				switch(event.type)
				{
					case Expose:
						break;
					case ConfigureNotify:
						onResize_(event.xconfigure.width,
						          event.xconfigure.height
						);
						break;
					case ButtonPress:
						onMousePress_(getButton_(event.xbutton.button),
						              event.xbutton.x,
						              event.xbutton.y,
						              getModifierKeys_(event.xbutton.state)
						);
						break;
					case ButtonRelease:
						onMouseRelease_(getButton_(event.xbutton.button),
						                event.xbutton.x,
						                event.xbutton.y,
						                getModifierKeys_(event.xbutton.state)
						);
						break;
					case MotionNotify:
						onMouseMove_(event.xmotion.x,
						             event.xmotion.y,
						             getModifierKeys_(event.xbutton.state)
						);
						break;
					case KeyRelease:
					{
						char key;
						XLookupString(&event.xkey,
						              &key,
						              1,
						              nullptr,
						              nullptr
						);
						onKeyRelease_(key);
						break;
					}
				}
			}
			
			glXMakeCurrent(display_, window_, ctx_);
			
			onIdle_();
			onPaintGL_();
			
			glFinish();
			glXSwapBuffers(display_, window_);
		}
		
		onUninitializeGL_();
		
		glXMakeCurrent(display_, 0, 0);
		
		glXDestroyContext(display_, ctx_);
		
		XDestroyWindow(display_, window_);
		XCloseDisplay(display_);
	}
	void exit()
	{
		running_ = false;
	}

protected:
	virtual void onInitializeGL()
	{
	}
	virtual void onUninitializeGL()
	{
	}
	virtual void onResize(int w, int h)
	{
	}
	virtual void onIdle()
	{
	}
	virtual void onPaintGL()
	{
	}
	virtual void onKeyRelease(int key)
	{
	}
	virtual void onMousePress(int button, int x, int y, int modKeys)
	{
	}
	virtual void onMouseRelease(int button, int x, int y, int modKeys)
	{
	}
	virtual void onMouseMove(int x, int y, int modKeys)
	{
	}

private:
	void onInitializeGL_()
	{
		printf("-------------------------------------------------------------\n");
		printf("GL Vendor       : %s\n", glGetString(GL_RENDERER));
		printf("GL Renderer     : %s\n", glGetString(GL_VENDOR));
		printf("GL Version      : %s\n", glGetString(GL_VERSION));
		printf("GLSL Version    : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		printf("Context Profile : %s\n", glGetString(GL_CONTEXT_PROFILE_MASK));
		printf("-------------------------------------------------------------\n");
		
		onInitializeGL();
	}
	void onUninitializeGL_()
	{
		onUninitializeGL();
	}
	void onResize_(int w, int h)
	{
		width_ = w;
		height_ = h;
		ratio_ = (GLdouble)width_ / (GLdouble)height_;
		
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//if (view_mode_ == ePerspective )
		//	gluPerspective(this->fovy_, this->ratio_, this->znear_, this->zfar_);
		//else
		//	glOrtho(this->right_, this->left_, this->bottom_, this->top_, this->znear_, this->zfar_);
		
		//glGetIntegerv(GL_VIEWPORT, this->viewport_);
		//glGetDoublev(GL_PROJECTION_MATRIX, this->projection_);
		
		//this->onReshape(_width, _height);
		onResize(w, h);
	}
	void onIdle_()
	{
		onIdle();
		//STDOUT_WARRNING("onIdle is not impl");
	}
	void onPaintGL_()
	{
		XWindowAttributes gwa;
		XGetWindowAttributes(display_, window_, &gwa);
		
		glViewport(0, 0, gwa.width, gwa.height);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (viewMode_ == kPerspective )
			gluPerspective(fovy_, ratio_, znear_, zfar_);
		else
			glOrtho(right_ * ratio_, left_ * ratio_, bottom_, top_, znear_, zfar_);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (viewMode_ == kPerspective )
		{
			/* camera track */
			glTranslated(track_x_, 0.0, 0.0);
			glTranslated(0.0, track_y_, 0.0);
			
			/* camera tumbble */
			double eye[3];
			
			sphToEucl_(eye_, eye);
			for ( int i = 0; i < 3; ++i )
				eye[i] += target_[i];
			
			
			gluLookAt(eye[0], eye[1], eye[2], target_[0], target_[1], target_[2], up_[0], up_[1], up_[2]);
			
			//glGetDoublev(GL_MODELVIEW_MATRIX, this->modelview_);
			
			/* positioned lights */
			//glLightfv(GL_LIGHT0, GL_POSITION, this->light_position_);
			
			/* draw grid*/
			draw_axis_grid_();
			
		}
		else
		{
			/* camera track */
			glTranslated(this->track_x_, 0.0, 0.0);
			glTranslated(0.0, this->track_y_, 0.0);
			
			/* camera tumbble */
			switch ( viewMode_ )
			{
				case kFront:
					eye_[0] = 90.0;
					eye_[1] = 0.0;
					eye_[2] = 10.0;
					break;
				case kLeft:
					eye_[0] = 180.0;
					eye_[1] = 0.0;
					eye_[2] = 10.0;
					break;
				case kRight:
					eye_[0] = 0.0;
					eye_[1] = 0.0;
					eye_[2] = 10.0;
					break;
				case kTop:
					eye_[0] = 270.0;
					eye_[1] = 270.0;
					eye_[2] = 10.0;
					break;
			}
			double eye[3];
			sphToEucl_(eye_, eye);
			for ( int i = 0; i < 3; ++i )
				eye[i] += target_[i];
			
			gluLookAt(eye[0], eye[1], eye[2], target_[0], target_[1], target_[2], up_[0], up_[1], up_[2]);
			
			//glGetDoublev(GL_MODELVIEW_MATRIX, this->modelview_);
			
			/* positioned lights */
			//glLightfv(GL_LIGHT0, GL_POSITION, this->light_position_);
			
			drawAxis_();
			switch (viewMode_)
			{
				case kFront:
					draw_xy_grid_();
					break;
				case kLeft:
					draw_yz_grid_();
					break;
				case kRight:
					draw_yz_grid_();
					break;
				case kTop:
					draw_xz_grid_();
					break;
				default: break;
			}
		}
		
		/*Rendering */
		glMatrixMode(GL_PROJECTION);
		glGetIntegerv(GL_VIEWPORT, viewport_);
		glGetFloatv(GL_PROJECTION_MATRIX, projectionMtx_);
		
		glMatrixMode(GL_MODELVIEW);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMtx_);
		
		//onRender();
		onPaintGL();
		
		/*Update Time */
		frame_ += stepSize();
		if(frame_ > maxFrame_)
			frame_ = minFrame_;
		
		glXSwapBuffers(display_, window_);
	}
	void onKeyRelease_(int key)
	{
		static const double kSpeed = 0.5;
		switch (key)
		{
			case 27:    //escape key
				running_ = false;
				break;
			case 'g':
			case 'G':    //G or g key
				displayGrid_ = (displayGrid_ ? false : true);
			case 'a':
			case 'A':
				displayAxis_ = (displayAxis_ ? false : true);
				break;
			case '1':
				viewMode_ = kPerspective;
				break;
			case '2':
				viewMode_ = kFront;
				break;
			case '3':
				viewMode_ = kLeft;
				break;
			case '4':
				viewMode_ = kRight;
				break;
			case '5':
				viewMode_ = kTop;
				break;
			case 32:
				this->active_ = (this->active_ ? false : true);
				break;
			case '+':
				if (viewMode_ == kPerspective)
				{
					eye_[2] -= kSpeed;
				}
				else
				{
					right_ += kSpeed;
					left_ -= kSpeed;
					bottom_ += kSpeed;
					top_ -= kSpeed;
				}
				break;
			case '-':
				if (viewMode_ == kPerspective)
				{
					eye_[2] += kSpeed;
				}
				else
				{
					right_ -= kSpeed;
					left_ += kSpeed;
					bottom_ -= kSpeed;
					top_ += kSpeed;
				}
				break;
		}
		onKeyRelease(key);
	}
	void onMousePress_(int button, int x, int y, int modKeys)
	{
		x0_ = x, y0_ = y;
		if (button == kLeftButton)// && modKeys == GLUT_DOWN)
		{
			state_ = kTumble;
		}
		if (button == kMidButton)// && _state == GLUT_DOWN)
		{
			state_ = kTrack;
		}
		if (button == kRightButton)// && _state == GLUT_DOWN)
		{
			state_ = kZoom;
		}
		onMousePress(button, x, y, modKeys);
	}
	void onMouseRelease_(int button, int x, int y, int modKeys)
	{
		if (button == kLeftButton)// && _state == GLUT_UP)
		{
			state_ = kPassive;
		}
		if (button == kMidButton)// && _state == GLUT_UP)
		{
			state_ = kPassive;
		}
		if (button == kRightButton)// && _state == GLUT_UP)
		{
			state_ = kPassive;
		}
		onMouseRelease(button, x, y, modKeys);
	}
	void onMouseMove_(int x, int y, int modKeys)
	{
		GLdouble dx = ((GLdouble)x - x0_) / (GLdouble)width_;
		GLdouble dy = (y0_ - (GLdouble)y) / (GLdouble)height_;
		
		//if (this->state_ == tumble && glutGetModifiers() == GLUT_ACTIVE_ALT)
		//{
		//	this->eye_[0] += (150 * dx);
		//	this->eye_[1] += (150 * dy);
		//}
		//if (this->state_ == track && glutGetModifiers() == GLUT_ACTIVE_ALT)
		//{
		//	this->track_x_ += 10.0 * dx;
		//	this->track_y_ += 10.0 * dy;
		//}
		if (state_ == kTumble)
		{
			eye_[0] += (150 * dx);
			eye_[1] += (150 * dy);
		}
		if (state_ == kTrack)
		{
			track_x_ += 10.0 * dx;
			track_y_ += 10.0 * dy;
		}
		if (state_ == kZoom)
		{
			double speed = 0.1;
			int direction = 0;
			if(dx < 0.0 || dy < 0.0)
				direction = -1;
			else
				direction = 1;
			if ( viewMode_ == kPerspective )
			{
				eye_[2] -= direction * speed;
			}
			else
			{
				right_	-= -direction * speed;
				left_	+= -direction * speed;
				bottom_	-= -direction * speed;
				top_	+= -direction * speed;
				
			}
		}
		x0_ = x;
		y0_ = y;
		
		onMouseMove(x, y, modKeys);
		
	}
private:
	int getButton_(unsigned int button)
	{
		if (button == Button1)
		{
			return kLeftButton;
		}
		else if (button == Button2)
		{
			return kRightButton;
		}
		else if (button == Button3)
		{
			return kMidButton;
		}
		return 0;
	}
	int getModifierKeys_(int state)
	{
		int keys = 0;
		if (state & ShiftMask)   keys |= kShift;
		if (state & ControlMask) keys |= kCtrl;
		if (state & Mod1Mask)    keys |= kAlt;
		return keys;
	}
	
	void drawAxis_() const
	{
		if ( displayAxis_ )
		{
			glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
			glLineWidth(3.0);
			glBegin(GL_LINES);
			glColor3f(0.5f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 0.0f, 0.0f);
			
			glColor3f(0.0f, 0.5f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			
			glColor3f(0.0f, 0.0f, .5f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 1.0f);
			
			glColor3f(1.0, 1.0, 1.0);
			glEnd();
			glPopAttrib();
		}
		
	}
	void draw_xy_grid_() const
	{
		if ( this->displayGrid_ )
		{
			int line_num = 20;
			int sep = 5;
			glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for ( int i = -line_num; i <= line_num; ++i )
			{
				if ( i % sep == 0)
					glColor3f(0.8f, 0.8f, 0.8f);
				else
					glColor3f(0.3f, 0.3f, 0.3f);
				glVertex3f(static_cast<GLfloat>(i), static_cast<GLfloat>(-line_num), static_cast<GLfloat>(0));
				glVertex3f(static_cast<GLfloat>(i), static_cast<GLfloat>(line_num), static_cast<GLfloat>(0));
				
				glVertex3f(static_cast<GLfloat>(line_num), static_cast<GLfloat>(i), static_cast<GLfloat>(0));
				glVertex3f(static_cast<GLfloat>(-line_num), static_cast<GLfloat>(i), static_cast<GLfloat>(0));
			}
			glColor3f(1.0, 1.0, 1.0);
			glEnd();
			glPopAttrib();
		}
	}
	void draw_yz_grid_() const
	{
		if ( displayGrid_ )
		{
			int line_num = 20;
			int sep = 5;
			glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for ( int i = -line_num; i <= line_num; ++i )
			{
				if ( i % sep == 0 )
					glColor3f(0.8f, 0.8f, 0.8f);
				else
					glColor3f(0.3f, 0.3f, 0.3f);
				glVertex3f(static_cast<GLfloat>(0), static_cast<GLfloat>(-line_num),	 static_cast<GLfloat>(i));
				glVertex3f(static_cast<GLfloat>(0), static_cast<GLfloat>(line_num),	 static_cast<GLfloat>(i));
				
				glVertex3f(static_cast<GLfloat>(0), static_cast<GLfloat>(i), static_cast<GLfloat>(line_num));
				glVertex3f(static_cast<GLfloat>(0), static_cast<GLfloat>(i), static_cast<GLfloat>(-line_num));
			}
			glColor3f(1.0, 1.0, 1.0);
			glEnd();
			glPopAttrib();
		}
	}
	void draw_xz_grid_() const
	{
		if ( displayGrid_ )
		{
			int line_num = 20;
			int sep = 5;
			glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for ( int i = -line_num; i <= line_num; ++i )
			{
				if ( i % sep == 0 )
					glColor3f(0.8f, 0.8f, 0.8f);
				else
					glColor3f(0.3f, 0.3f, 0.3f);
				glVertex3f(static_cast<GLfloat>(i), static_cast<GLfloat>(0), static_cast<GLfloat>(-line_num));
				glVertex3f(static_cast<GLfloat>(i), static_cast<GLfloat>(0), static_cast<GLfloat>(line_num));
				
				glVertex3f(static_cast<GLfloat>(line_num) ,static_cast<GLfloat>(0), static_cast<GLfloat>(i));
				glVertex3f(static_cast<GLfloat>(-line_num),static_cast<GLfloat>(0), static_cast<GLfloat>(i));
			}
			glColor3f(1.0, 1.0, 1.0);
			glEnd();
			glPopAttrib();
		}
	}
	void draw_axis_grid_() const
	{
		float tmp, tmp_color[4];
		glGetFloatv(GL_LINE_WIDTH, &tmp);
		glGetFloatv(GL_CURRENT_COLOR, tmp_color);
		
		glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
		
		/* draw axis */
		//glDisable(GL_LIGHTING);
		if (displayAxis_)
		{
			glLineWidth(3.0);
			glBegin(GL_LINES);
			glColor3f(0.5f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 0.0f, 0.0f);
			
			glColor3f(0.0f, 0.5f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			
			glColor3f(0.0f, 0.0f, .5f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 1.0f);
			glEnd();
		}
		/* draw grid */
		if (displayGrid_)
		{
			int line_num = 10;
			int sep = 5;
			
			/* x-z grid */
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for ( int i = -line_num; i <= line_num; ++i )
			{
				if ( i % sep == 0 )
					glColor3f(0.8f, 0.8f, 0.8f);
				else
					glColor3f(0.3f, 0.3f, 0.3f);
				
				glVertex3f((GLfloat)i, (GLfloat)0, (GLfloat)-10);
				glVertex3f((GLfloat)i, (GLfloat)0, (GLfloat)10);
				
				glVertex3f((GLfloat)10, (GLfloat)0, (GLfloat)i);
				glVertex3f((GLfloat)-10, (GLfloat)0, (GLfloat)i);
			}
			glEnd();
			/* x-y grid*/
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for ( int i = -line_num; i <= line_num; ++i )
			{
				if ( i % sep == 0 )
					glColor3f(0.8f, 0.8f, 0.8f);
				else
					glColor3f(0.3f, 0.3f, 0.3f);
				
				glVertex3f((GLfloat)i, (GLfloat)-10 + 10, (GLfloat)-10);
				glVertex3f((GLfloat)i, (GLfloat)10 + 10, (GLfloat)-10);
				
				glVertex3f((GLfloat)10, (GLfloat)i + 10, (GLfloat)-10);
				glVertex3f((GLfloat)-10, (GLfloat)i + 10, (GLfloat)-10);
			}
			glEnd();
			/* y-z grid*/
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for ( int i = -line_num; i <= line_num; ++i )
			{
				if ( i % sep == 0 )
					glColor3f(0.8f, 0.8f, 0.8f);
				else
					glColor3f(0.3f, 0.3f, 0.3f);
				
				glVertex3f((GLfloat)-10, (GLfloat)i + 10, (GLfloat)-10);
				glVertex3f((GLfloat)-10, (GLfloat)i + 10, (GLfloat)10);
				
				glVertex3f((GLfloat)-10, (GLfloat)10 + 10, (GLfloat)i);
				glVertex3f((GLfloat)-10, (GLfloat)-10 + 10, (GLfloat)i);
			}
			glEnd();
		}
		glLineWidth(tmp);
		glColor3fv(tmp_color);
		//glEnable(GL_LIGHTING);
		glPopAttrib();
	}
	
	void sphToEucl_(double const sph[3], double eucl[3]) const
	{
		static double pi = 3.14159265358979323846;
		
		double phi      = (pi * 2.0 / 360.0) * sph[0];
		double theta    = (pi * 2.0 / 360.0) * (sph[1] + 90.0);
		double r        = sph[2];
		
		
		
		double sinTheta = sin(theta);
		double cosTheta = cos(theta);
		double cosPhi   = cos(phi);
		double sinPhi   = sin(phi);
		
		//return vec3d(r * sinTheta * cosPhi, r * cosTheta, r * sinTheta * sinPhi);
		eucl[0] = r * sinTheta * cosPhi;
		eucl[1] = r * cosTheta;
		eucl[2] = r * sinTheta * sinPhi;
	}
	
	
	//int window_width() const			{ return window_width_;                                     }
	//int window_height() const			{ return this->window_height_;                                    }
	
	double frame() const                { return frame_;                            }
	double sec() const                  { return frame_ / fps_;                     }
	double dt() const                   { return (shutterAngle_ / 360.0) / fps_;    }
	double stepSize() const             { return dt() * fps_;                    	}
	
};
