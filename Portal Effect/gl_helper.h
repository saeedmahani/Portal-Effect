

#ifndef GL_HELPER
	#define GL_HELPER

	#include "common_defs.h"

	#ifdef _MSC_VER						// Windows
		#include <gl/glee.h>
		#include <gl/glext.h>			
		#include <gl/glut.h>
	#else								// Linux
		//#include "GLee.h"
		//#include <OpenGL/glext.h>
		#include <GLUT/glut.h>
	#endif
	#ifdef USE_SHADERS
		#include <Cg/cg.h>
		#include <Cg/cgGL.h>
	#endif


	#include "matrix.h"
	#include "vector.h"
	#include "image.h"
	//#include "mtime.h"

	extern void checkOpenGL ();
	extern void drawText ( int x, int y, char* msg);
	extern void drawGrid ();	
	extern void drawAxes ();
	extern void drawSphere ( float rad, int usec, int vsec );
	extern void drawCylinder ( float hgt, float rad );
	extern void measureFPS ();
	extern void getViewRay ( float x, float y, Vector4DF& p0, Vector4DF& dir );

	//extern mint::Time	tm_last;
	extern int			tm_cnt;
	extern float		tm_fps;

	#define DRAG_OFF		0				// mouse states
	#define DRAG_LEFT		1
	#define DRAG_RIGHT		2


	extern int			mouse_x, mouse_y;
	extern float		mouse_sx, mouse_sy, mouse_sz, mouse_sw;
	extern int			mouse_lx, mouse_ly;
	extern int			mouse_drag, mouse_mode;

	extern Matrix4F		inv_view;

	extern void disableShadows ();
	extern void checkFrameBuffers ();

	#ifdef USE_SHADOWS
		extern void setShadowLight ( float fx, float fy, float fz, float tx, float ty, float tz, float fov );
		extern void setShadowLightColor ( float dr, float dg, float db, float sr, float sg, float sb );
		
		extern void createFrameBuffer ();
		extern void createShadowTextures ();
		extern void computeLightMatrix ( int n, int tx, int ty );
		extern void renderDepthMap_FrameBuffer ( int n, float wx, float wy );
		extern void renderShadowStage ( int n, float* vmat );
		extern void renderShadows ( float* vmat );
		extern void drawScene ( float* view_mat );		// provided by user

		extern float light_proj[16];
		extern float light_x, light_y, light_z;
		extern float light_tox, light_toy, light_toz;
		extern float light_mfov;

		extern GLuint		shadow1_id;
		extern GLuint		shadow2_id;
	#endif

	#ifdef USE_SHADERS
		void cgErrorCallback ();
		void loadShader ( int n, char* vname, char* vfunc, char* fname, char* ffunc );
		void enableShader ( int n );
		void disableShaders ();

		extern CGprogram vert_prog[10];					// Handles for shader program objects
		extern CGprogram frag_prog[10];
	#endif

	

#endif
