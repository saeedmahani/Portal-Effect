
#include "common_defs.h"
#include "gl_helper.h"

extern float		window_width, window_height;
extern Vector3DF	cam_from;

// Inverse view matrix
Matrix4F inv_view;

// Shadow Light
float light_proj[16];
float light_x, light_y, light_z;
float light_tox, light_toy, light_toz;
float light_mfov;

// Mouse control

int		mouse_x, mouse_y;
float	mouse_sx, mouse_sy, mouse_sz, mouse_sw ;
int		mouse_lx, mouse_ly;
int		mouse_drag, mouse_mode;

// Fonts
void *font = GLUT_BITMAP_8_BY_13;
void *fonts[] = {GLUT_BITMAP_9_BY_15,
				 GLUT_BITMAP_TIMES_ROMAN_10,
				GLUT_BITMAP_TIMES_ROMAN_24};
// Timing
//mint::Time	tm_last;
int			tm_cnt;
float		tm_fps;


// Check if there have been any openGL problems
void checkOpenGL ()
{
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR) {
		const GLubyte* errString = gluErrorString(errCode);
		fprintf( stderr, "OpenGL error: %s\n", errString );
	}
}

void drawText ( int x, int y, char* msg)
{
  int len, i;
  glRasterPos2f(x, y);
  len = (int) strlen(msg);
  for (i = 0; i < len; i++) 
    glutBitmapCharacter(font, msg[i]);  
}

void drawGrid ()
{
	// Draw grid in X/Z plane (Y+ is up)
	glColor3f ( 0.3, 0.3, 0.3 );
	glBegin ( GL_LINES );
	for (float x=-40; x<=40.0; x+=10.0 ) {
		glVertex3f ( x, 0, -40.0 );
		glVertex3f ( x, 0, 40.0 );
	}
	for (float z=-40; z<=40.0; z+=10.0 ) {
		glVertex3f ( -40.0, 0, z );
		glVertex3f (  40.0, 0, z );
	}
	glEnd ();
}

void drawAxes ()
{
	glLineWidth ( 3 );
	glTranslatef ( 0, 0, .1);
	glBegin ( GL_LINES );	
	glColor3f(1,0,0); glVertex3f ( 0,0,0);	glVertex3f ( 10,0,0);
	glColor3f(0,1,0); glVertex3f ( 0,0,0);	glVertex3f ( 0,10,0);
	glColor3f(0,0,1); glVertex3f ( 0,0,0);	glVertex3f ( 0,0,10);
	glEnd ();
	glLineWidth ( 1 );	
}

void drawSphere ( float rad, int usec, int vsec )
{
	float vstep = 180.0 / vsec;
	float ustep = 360.0 / usec;
	float x, y, z, w;
	float xl, yl, zl, wl; 
	glBegin ( GL_TRIANGLE_STRIP );	
	xl = sin(-90 * DEGtoRAD); wl = cos(-90 * DEGtoRAD);
	for (float vang = -90.0; vang <= 90.0; vang += vstep) {
		x = sin(vang * DEGtoRAD); w = cos(vang * DEGtoRAD);
		for (float uang =0; uang <= 360.0; uang += ustep) {			
			yl = wl * cos(uang * DEGtoRAD); zl = wl * sin(uang * DEGtoRAD);
			y = w * cos(uang * DEGtoRAD); z = w * sin(uang * DEGtoRAD);			
			glTexCoord2f ( 0.5 + (vang-vstep)/180.0, uang/360.0f  ); glNormal3f ( xl, yl, zl ); glVertex3f ( xl*rad, yl*rad, zl*rad );
			glTexCoord2f ( 0.5 + vang/180.0, uang/360.0f ); glNormal3f ( x, y, z ); glVertex3f ( x*rad, y*rad, z*rad );
		}
		xl = x; wl = w;
	}
	glEnd ();
}

void drawCylinder ( float hgt, float rad )
{
	float y, z;
	glBegin ( GL_TRIANGLE_STRIP );	
	for (float ang =0; ang <= 360.0; ang += 10.0) {
		y = cos(ang*DEGtoRAD); z = sin(ang*DEGtoRAD);
		glTexCoord2f ( 0, ang/360.0f  ); glNormal3f ( 0, y, z ); glVertex3f ( 0, y*rad, z*rad );
		glTexCoord2f ( 1, ang/360.0f ); glNormal3f ( 0, y, z ); glVertex3f ( hgt, y*rad, z*rad );		
	}
	glEnd ();
}

void getViewRay ( float x, float y, Vector4DF& p0, Vector4DF& dir )
{
	p0.Set ( cam_from.x, cam_from.y, cam_from.z );	
	float fnear = 1.0;
	float fov = 40.0 * DEGtoRAD;
	float asp = float(window_width) / window_height;
	dir.x = fnear * tanf ( fov*0.5f) * (x / (window_width/2.0f) - 1.0f) * asp;
	dir.y = fnear * tanf ( fov*0.5f) * ( 1.0f - y / (window_height/2.0f) );
	dir.z = -fnear;
	dir *= inv_view;
	dir.Normalize ();
}

/*
void measureFPS ()
{
	// Measure FPS
	mint::Time tm_elaps;	
	if ( ++tm_cnt > 5 ) {		
		tm_elaps.SetSystemTime ( ACC_NSEC );			// get current sytem time - accurate to 1 ns
		tm_elaps = tm_elaps - tm_last;					// get elapsed time from 5 frames ago
		tm_fps = 5.0 * 1000.0 / tm_elaps.GetMSec ();	// compute fps
		tm_cnt = 0;										// reset frame counter
		tm_last.SetSystemTime ( ACC_NSEC );
	}
}

 */
void checkFrameBuffers ()
{                                                            
	GLenum status;                                             
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);  
	switch(status) {                                          
	case GL_FRAMEBUFFER_COMPLETE_EXT: printf ( "FBO complete\n" ); break;                                                
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT: printf ( "FBO format unsupported\n"); break;                                                
	default:  printf ( "Unknown FBO error\n");
	}
}

void disableShadows ()
	{
		glDisable ( GL_TEXTURE_2D );		
		
		glActiveTextureARB( GL_TEXTURE1_ARB );
		glBindTexture ( GL_TEXTURE_2D, 0 );
		glDisable ( GL_TEXTURE_GEN_S );
		glDisable ( GL_TEXTURE_GEN_T );
		glDisable ( GL_TEXTURE_GEN_R );
		glDisable ( GL_TEXTURE_GEN_Q );	
		
		glActiveTextureARB( GL_TEXTURE2_ARB );
		glBindTexture ( GL_TEXTURE_2D, 0 );		
		glDisable ( GL_TEXTURE_GEN_S );
		glDisable ( GL_TEXTURE_GEN_T );
		glDisable ( GL_TEXTURE_GEN_R );
		glDisable ( GL_TEXTURE_GEN_Q );	
	}

#ifdef USE_SHADOWS
	// Materials & Textures
	GLuint shadow1_id = 0;			// display buffer shadows
	GLuint shadow2_id = 0;			// display buffer shadows

	// Frame buffer
	GLuint frameBufferObject = 0;	// frame buffer shadows

	void createFrameBuffer () 
	{
		//Generate the frame buffer object
		glGenFramebuffersEXT (1, &frameBufferObject);  
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, frameBufferObject);		// Turn on frame buffer object
		glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadow1_id, 0);
		glDrawBuffer (GL_NONE);							// Set Draw & ReadBuffer to none since we're rendering depth only
		glReadBuffer (GL_NONE);
		checkFrameBuffers ();					// Check completeness of frame buffer object (no need for stencil and depth attachement)
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);	// Turn off frame buffer object
	}

	void createShadowTextures ()
	{
		// Create depth texture maps
		glActiveTextureARB( GL_TEXTURE1_ARB );
		glGenTextures( 1, &shadow1_id );	
		glBindTexture ( GL_TEXTURE_2D, shadow1_id );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		
		//-- sets region outside shadow to 0
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB  );
		
		//-- sets region outside shadow to 1 (border edge color)
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );				

		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_ARB, TEX_SIZE, TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

		glActiveTextureARB( GL_TEXTURE2_ARB );
		glGenTextures( 1, &shadow2_id );
		glBindTexture ( GL_TEXTURE_2D, shadow2_id );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB );
		//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB  );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE  );
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D ( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_ARB, TEX_SIZE, TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	}

	void computeLightMatrix ( int n, int tx, int ty )
	{
		int lnum = n;
		// Construct projective texturing matrix

		// S - light bias matrix
		glMatrixMode ( GL_MODELVIEW );
		glLoadIdentity ();
		glTranslatef ( 0.5, 0.5, 0.5 );
		glScalef ( 0.5, 0.5, 0.5 );
		// Plight - light projection matrix
		gluPerspective ( light_mfov*2.0, float(tx) / ty, LIGHT_NEAR, LIGHT_FAR );
		// L^-1 - light view inverse matrix
		gluLookAt ( light_x, light_y, light_z, light_tox, light_toy, light_toz, 0, 0, 1);
		glPushMatrix ();
		glGetFloatv ( GL_MODELVIEW_MATRIX, light_proj );
		glPopMatrix ();

	}
	void renderDepthMap_FrameBuffer ( int n, float wx, float wy )
	{
		float vmat[16];

		computeLightMatrix ( n, TEX_SIZE, TEX_SIZE );	

		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, frameBufferObject);
		
		if ( n == 0 )	{	
			glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadow1_id, 0);
		} else {		
			glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadow2_id, 0);
		}
		
		if ( n == 0 )	glActiveTextureARB( GL_TEXTURE1_ARB );			// TEXTURE1 = shadow map stage	
		else			glActiveTextureARB( GL_TEXTURE2_ARB );			// TEXTURE2 = shadow map stage				
		
		glViewport (1, 1, TEX_SIZE-2, TEX_SIZE-2);		// Note: Avoid artifact cause by drawing into border pixels
		glClear ( GL_DEPTH_BUFFER_BIT );	
		glLoadIdentity();

		// Plight - projection matrix of light
		glMatrixMode ( GL_PROJECTION );			// Setup projection for depth-map rendering
		glLoadIdentity ();
		gluPerspective ( light_mfov*2.0, float(TEX_SIZE) / TEX_SIZE, LIGHT_NEAR, LIGHT_FAR );

		// L^-1 - light view matrix (gluLookAt computes inverse)
		glMatrixMode ( GL_MODELVIEW);			// Setup view for depth-map rendering
		glLoadIdentity ();
		gluLookAt ( light_x, light_y, light_z, light_tox, light_toy, light_toz, 0, 0, 1);
		glPushMatrix ();						// Save view matrix for later
		glGetFloatv ( GL_MODELVIEW_MATRIX, vmat );
		glPopMatrix ();

		glDisable ( GL_LIGHTING );
		glColor4f ( 1, 1, 1, 1 );
		glShadeModel (GL_FLAT);					// No shading (faster)
		
		glEnable ( GL_CULL_FACE );
		glCullFace ( GL_FRONT );	
		
		glEnable ( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset ( 50.0, 0.1 );			// Depth bias
		
		drawScene ( &vmat[0] );				// Draw scene. 

		glDisable ( GL_POLYGON_OFFSET_FILL );	
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
		glViewport ( 0, 0, (GLsizei) wx, (GLsizei) wy );

		//glCullFace (GL_BACK);					// Restore render states
		//glBindTexture ( GL_TEXTURE_2D, 0);
	}

	void renderShadowStage ( int n, float* vmat )
	{
		GLfloat pos[4];
		GLfloat row[4];

		computeLightMatrix ( n, TEX_SIZE, TEX_SIZE );
		if ( n == 0 ) {
			glActiveTextureARB( GL_TEXTURE1_ARB );			// TEXTURE1 = shadow map stage #1
		} else {
			glActiveTextureARB( GL_TEXTURE2_ARB );			// TEXTURE2 = shadow map stage #2
		}
		glEnable ( GL_TEXTURE_2D );   
		if ( n == 0 )	glBindTexture ( GL_TEXTURE_2D, shadow1_id );
		else			glBindTexture ( GL_TEXTURE_2D, shadow2_id );	

		glMatrixMode( GL_MODELVIEW );	
		glLoadMatrixf ( vmat );

		row[0] = light_proj[0]; row[1] = light_proj[4]; row[2] = light_proj[8]; row[3] = light_proj[12];
		glTexGenfv(GL_S, GL_EYE_PLANE, &row[0] );
		row[0] = light_proj[1]; row[1] = light_proj[5]; row[2] = light_proj[9]; row[3] = light_proj[13];
		glTexGenfv(GL_T, GL_EYE_PLANE, &row[0] );
		row[0] = light_proj[2]; row[1] = light_proj[6]; row[2] = light_proj[10]; row[3] = light_proj[14];
		glTexGenfv(GL_R, GL_EYE_PLANE, &row[0] );
		row[0] = light_proj[3]; row[1] = light_proj[7]; row[2] = light_proj[11]; row[3] = light_proj[15];
		glTexGenfv(GL_Q, GL_EYE_PLANE, &row[0] );
		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glEnable(GL_TEXTURE_GEN_S);    
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_GEN_Q);	

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
		
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE ) ;
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE ) ;
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS ) ;
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT ) ;
			
		pos[0] = 0.20;
		pos[1] = 0.20;
		pos[2] = 0.20;
		pos[3] = 0.20;
		glTexEnvfv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &pos[0] );
	}

	void renderShadows ( float* vmat )
	{
		GLfloat pos[4];
		
		renderShadowStage ( 0, vmat );
	//	renderShadowStage ( 1, vmat );		

		glActiveTextureARB( GL_TEXTURE0_ARB );			// Render Tex 0 - Base render
		glDisable ( GL_TEXTURE_GEN_S );
		glDisable ( GL_TEXTURE_GEN_T );
		glDisable ( GL_TEXTURE_GEN_R );
		glDisable ( GL_TEXTURE_GEN_Q );	
		glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glEnable ( GL_LIGHTING );
		glLightModeli (GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
		
		glEnable ( GL_LIGHT0 );	
		pos[0] = light_x; pos[1] = light_y; pos[2] = light_z; pos[3] = 1.0;	
		glLightfv ( GL_LIGHT0, GL_POSITION, &pos[0] );
	
		/* glEnable ( GL_LIGHT1 );
		pos[0] = light[1].x; pos[1] = light[1].y; pos[2] = light[1].z; pos[3] = 1.0;	
		glLightfv ( GL_LIGHT1, GL_POSITION, &pos[0] );*/			
	}

	

	void setShadowLight ( float fx, float fy, float fz, float tx, float ty, float tz, float fov )
	{
		light_x = fx;
		light_y = fy;
		light_z = fz;
		light_tox = tx;
		light_toy = ty;
		light_toz = tz;
		light_mfov = fov;
	}

	void setShadowLightColor ( float dr, float dg, float db, float sr, float sg, float sb )
	{
		GLfloat amb[4] = {0.0,0.0,0.0,1};
		GLfloat dif[4];
		GLfloat spec[4];
		GLfloat pos[4] = {0.0,0.0,0.0, 100.0};

		glEnable(GL_LIGHT0);    
		dif[0] = dr; dif[1] = dg; dif[2] = db; dif[3] = 1;	
		spec[0] = sr; spec[1] = sg; spec[2] = sb; spec[3] = 1;
		glLightfv(GL_LIGHT0, GL_AMBIENT, &amb[0] );
		glLightfv(GL_LIGHT0, GL_DIFFUSE, &dif[0] );
		glLightfv(GL_LIGHT0, GL_SPECULAR, &spec[0] );
	}

#endif


#ifdef USE_SHADERS

	CGcontext cgContext = 0;
	CGprofile vert_profile = CG_PROFILE_VP30;		// Profiles are latest supported by NVIDIA FX 5200
	CGprofile frag_profile = CG_PROFILE_FP30;
	CGprogram vert_prog[10];							// Handles for shader program objects
	CGprogram frag_prog[10];
	//CGparameter uModelViewProj, uModel;
	//CGparameter uLightPos, uViewPos, uTexture, uCube;

	void cgErrorCallback ()
	{
		CGerror error = cgGetError();
		if( error ) {
			const char *listing = cgGetLastListing( cgContext );
			fprintf( stderr, "---------------------------------------------------------\n" );
			fprintf( stderr, "ERROR: Cg error\n" );
			fprintf( stderr, "%s\n\n", cgGetErrorString( error ) );
			fprintf( stderr, "%s\n", listing );
			fprintf( stderr, "---------------------------------------------------------\n" );
			fgetc( stdin );
			exit( 1 );
		}
	}

	void loadShader ( int n, char* vname, char* vfunc, char* fname, char* ffunc)
	{
		if ( cgContext == 0 ) {
			cgSetErrorCallback( cgErrorCallback );
			cgContext = cgCreateContext();
		}

		// Select profiles
		vert_profile = cgGLGetLatestProfile( CG_GL_VERTEX );
		cgGLSetOptimalOptions( vert_profile );
		frag_profile = cgGLGetLatestProfile( CG_GL_FRAGMENT );
		cgGLSetOptimalOptions( frag_profile );

		vert_prog[n] = cgCreateProgramFromFile( cgContext, CG_SOURCE, vname, vert_profile, vfunc, NULL );
		frag_prog[n] = cgCreateProgramFromFile( cgContext, CG_SOURCE, fname, frag_profile, ffunc, NULL );
		cgGLLoadProgram( vert_prog[n] );
		cgGLLoadProgram( frag_prog[n] );
	}

	void enableShader ( int n )
	{
		cgGLEnableProfile( vert_profile );
		cgGLEnableProfile( frag_profile );
		cgGLBindProgram( vert_prog[n] );		
		cgGLBindProgram( frag_prog[n] );
	}

	void disableShaders ()
	{
		cgGLDisableProfile( vert_profile );
		cgGLDisableProfile( frag_profile );
	}

#endif
