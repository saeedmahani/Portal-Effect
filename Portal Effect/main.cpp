
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#include "gl_helper.h"

// Globals
float window_width  = 1024;
float window_height = 768;
int iSelect;
float cubeAngle = 0;
float spinAngle = 0.0;
float treoY = 10.0;
int height1 = 0;
bool bouncyPortal = false;

Vector3DF bouncy;

static GLuint texName;

#define other(x) (x) == (0) ? (1) : (0)


#define radToDeg 57.295779513082320876798154814114
#define degToRad 0.0174532925
#define MAX_STRAFE_SPEED 3

// Camera info
Vector3DF	cam_from, cam_angs, cam_to;
Vector3DF	obj_from, obj_angs, obj_dang;
Vector3DF	light_from, light_to;
Vector3DF	portal_view_from[2], portal_view_to[2];
Vector3DF	portal_cam_from[2], portal_cam_to[2];
Vector3DF	player_pos, player_angle, player_view;
float		light_fov;
float		cam_dolly;

bool STRAFE_LEFT;
bool STRAFE_RIGHT;
bool STRAFE_FORWARD;
bool STRAFE_BACKWARD;

#define	SIZE 256 //texture size
#define size 20 //panel size/2
#define sizeb 10 //backpanel size/2

// View matricies
float view_matrix[16];					// View matrix (V)
float model_matrix[16];					// Model matrix (M)
float proj_matrix[16];					// Projective matrix

// Image texture
Image	img;
Image	tileimg;
Image	portalringimg[18]; // ring animation
GLuint	image1;

float runtime;
float dt;
float portalframe[2];
float kradius;
float telespherez = 0.0;
bool PORTAL_OPEN[2];


static unsigned char texture[3 * SIZE * SIZE];
static unsigned int texture_id;


void drawPanel_floor() 
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tileimg.getID());
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glBegin(GL_QUADS);
	glNormal3f (0, 1, 0);
	glTexCoord2f(0.0, 0.0); glVertex3f (-20, 0, 20);
	glTexCoord2f(8.0, 0.0); glVertex3f (20, 0, 20);
	glTexCoord2f(8.0, 8.0); glVertex3f (20, 0, -20);
	glTexCoord2f(0.0, 8.0); glVertex3f (-20, 0, -20);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawPanel_back()
{
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glNormal3f (0, 1, 0);
	glTexCoord2f(0.0, 0.0); glVertex3f (-20, 0, -30);
	glTexCoord2f(8.0, 0.0); glVertex3f (20, 0, -30);
	glTexCoord2f(8.0, 8.0); glVertex3f (20, 40, -30);
	glTexCoord2f(0.0, 8.0); glVertex3f (-20, 40, -30);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawCircle(float radius)
{
   glBegin(GL_LINE_LOOP);
 
   for (int i=0; i <= 360; i++)
   {
      float degInRad = i*degToRad;
      glVertex2f(cos(degInRad)*radius,sin(degInRad)*radius);
   }
 
   glEnd();
}



void drawPanel_portal(int i)
{
	glPushMatrix();
	// calculate vector rotation
	Vector3DF portal_view_normal = (portal_view_to[i] - portal_view_from[i]).Normalize();
	Vector3DF pos_z_axis; pos_z_axis.Set(0, 0, 1);
	float portal_angle = acos(portal_view_normal.Dot(pos_z_axis)) * radToDeg;
	Vector3DF portal_cross = pos_z_axis.Cross(portal_view_normal);
	
	// move to position
	glTranslatef (portal_view_from[i].x, portal_view_from[i].y, portal_view_from[i].z);
	glRotatef(portal_angle, 0, portal_cross.y, 0);

	GLUquadric * quadric = gluNewQuadric();

	float radius = portalframe[i] * 0.5 + 1;

	switch((int)portalframe[i]) 
	{
		case -1: break; // do not draw portal disk
		default: gluDisk(quadric, 0, radius, 50, 2); break;
	}
	
	glPopMatrix();
}

void drawPanel_portal_sprite(int i)
{
	glPushMatrix();
	// calculate vector rotation
	Vector3DF portal_view_normal = (portal_view_to[i] - portal_view_from[i]).Normalize();
	Vector3DF pos_z_axis; pos_z_axis.Set(0, 0, 1);
	float portal_angle = acos(portal_view_normal.Dot(pos_z_axis)) * radToDeg;
	Vector3DF portal_cross = pos_z_axis.Cross(portal_view_normal);
	
	// move to position
	glTranslatef (portal_view_from[i].x, 0, portal_view_from[i].z);
	glRotatef(portal_angle, 0, portal_cross.y, 0);

	if(portalframe[i] >= 0) {
	glBindTexture(GL_TEXTURE_2D, portalringimg[(int)portalframe[i]].getID());

	glBegin(GL_QUADS);
	glNormal3f (0, 0, 1);
	glTexCoord2f(0.0, 0.0); glVertex3f (-10, 0, 0);
	glTexCoord2f(1.0, 0.0); glVertex3f (10, 0, 0);
	glTexCoord2f(1.0, 1.0); glVertex3f (10, 20, 0);
	glTexCoord2f(0.0, 1.0); glVertex3f (-10, 20, 0);
	glEnd();
	}
	glPopMatrix();
	
}

void drawPortal() 
{
	glPushMatrix();
	glScalef(1.05, 1.05, 1);
	glTranslatef (0, sizeb, -size-0.5);
	glRotatef(90, 1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f (0,0,1);
	glVertex3f (-sizeb, 0, sizeb);
	glVertex3f (sizeb, 0, sizeb);
	glVertex3f (sizeb, 0, -sizeb);
	glVertex3f (-sizeb, 0, -sizeb);
	glEnd();
	glPopMatrix();
}
int bob3 = 0;
void drawPortalCam(int i) {
		// Draw portal cam
	glPushMatrix();
	

	Vector3DF portal_cam_normal = (portal_cam_to[i] - portal_cam_from[i]).Normalize();
	//if(bob1 == 0)
	//	printf("portal_view_normal: %f, %f, %f\n", portal_view_normal.x, portal_view_normal.y, portal_view_normal.z);
	Vector3DF neg_z_axis; neg_z_axis.Set(0, 0, -1);
	float portal_angle = acos(portal_cam_normal.Dot(neg_z_axis)) * radToDeg;
	if(bob3 < 2)
		printf("cam angle for %d: %f", i, portal_angle);
	bob3++;
	Vector3DF portal_cross = neg_z_axis.Cross(portal_cam_normal);
	
	// move to position
	glTranslatef (portal_cam_from[i].x, 0, portal_cam_from[i].z);
	glRotatef(portal_angle, 0, portal_cross.y, 0);
	glTranslatef(0, 8, 5);


	//glTranslatef(portal_cam_from[i].x, portal_cam_from[i].y, portal_cam_from[i].z);
	//glRotatef(-90, 0, 1, 0);

	glColor3f ( 0, 1, 0);
	glPushMatrix();
	glScalef(1, 1, 1.5);
	glutSolidCube(2);
	glPopMatrix();
	glTranslatef(0,0,-3);
	glutSolidCone(1.5, 2.1, 10, 10);
	glPopMatrix();
}

void draw3D ()
{
	// -- Lighting OFF --
	glDisable ( GL_LIGHTING );

	// Draw axes & grid
	drawAxes ();					
	drawGrid ();	

	// Draw light itself
	glPushMatrix();
	glTranslatef ( light_from.x, light_from.y, light_from.z );
	glColor3f ( 1, 1, 1 );
	glutSolidSphere ( 1, 8, 8 );
	glPopMatrix();

	//drawPortal();

	// -- Lighting ON --
	glEnable ( GL_LIGHTING );
	
	// Draw sphere
	glPushMatrix();
	glTranslatef(0, treoY, 0);
	glRotatef(spinAngle + 120, 0, 0, 1);	
	glTranslatef ( 0, 5, 0 );			
		 
	glutSolidSphere ( 2.2, 20, 20 );
	glPopMatrix();
	// Draw cube
	
	glPushMatrix();
	glTranslatef(0, treoY, 0);
	glRotatef(spinAngle, 0, 0, 1);
	glTranslatef(0, 5, 0);
	glRotatef(spinAngle, 1, 1, 0);
	glutSolidCube(4);
	glPopMatrix();
	
	// draw torus
	//glColor3f(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslatef(0, treoY, 0);
	glRotatef(spinAngle + 240, 0, 0, 1);
	glTranslatef(0, 5, 0);
	glRotatef(spinAngle, 1, 1, 0);
	glutSolidTorus(0.8, 2.4, 50, 20 );
	glPopMatrix();
	
	//draw bouncy
	glPushMatrix();
	glTranslatef(bouncy.x, bouncy.y + 2.3, bouncy.z);
	glutSolidSphere(2.2, 20, 20);
	glPopMatrix();
	

	// Draw room

	// floor
	drawPanel_floor();

	// back
	//drawPanel_back();


}

void draw2D ()
{
	// Setup 2D projection matrix
	glDisable ( GL_LIGHTING );
	glDisable (GL_DEPTH_TEST);  
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();  
	glScalef ( 2.0/window_width, -2.0/window_height, 1 );		// Setup view (0,0) to (800,600)
	glTranslatef ( -window_width/2.0, -window_height/2, 0.0);

	// Setup 2D viewing matrix
	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ();

	// Write text on screen
	char disp[200];
	glDisable ( GL_TEXTURE_2D );
	glColor4f ( 1.0, 1.0, 1.0, 1.0 );	
	
	sprintf ( disp, "Camera:  X: %3.2f, Y: %3.2f, Z: %3.2f, dolly: %3.2f", cam_angs.x, cam_angs.y , cam_angs.z, cam_dolly );
	drawText (10, 25, disp);
	sprintf(disp, "FPS: %.0f runtime: %3.2f", tm_fps, runtime);
	drawText ( 10, 15, disp );	
}

void generatePortalStencil(int i) {
	// cull face back to not draw back of portal
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glClearStencil(0x0); //?
	//glClear(GL_STENCIL_BUFFER_BIT);
	if(i == 0) // portal 0
		glStencilFunc (GL_ALWAYS, 0x1, 0x1);
	else // portal 1
		glStencilFunc(GL_ALWAYS, 0x10, 0x10);
	glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glPushMatrix();
	drawPanel_portal(i);
	glPopMatrix();
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
}

void renderToTexture()
{
	/* Render to texture method

	//gluLookAt ( 0, 5, 40, 0, 5, 0, 0, 1, 0 );
	gluLookAt(cv.x, cv.y, cv.z , portal_view.x, portal_view.y, portal_view.z + height1, 0, 1, 0);
	//glViewport(0, 0, SIZE, SIZE);

	//glEnable(GL_TEXTURE_2D);
	//draw3D();
	//glDisable(GL_TEXTURE_2D);
	//glFlush();

	// Copy buffer to texture 
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 5, 5, 0, 0, SIZE - 10, SIZE - 10);

	//restore viewport
	glViewport( 0, 0, window_width, window_height );

	*/
}

int bob = 0;
void drawPortalToStencil(int i) 
{	
	glPushMatrix();
	// start drawing portal image over stencil area
	glEnable(GL_STENCIL_TEST);

	if(i == 0) // portal 0
		glStencilFunc(GL_EQUAL, 0x1, 0x1);
	else // portal 1
		glStencilFunc (GL_EQUAL, 0x10, 0x10);

	// do this only when portal created?
	Vector3DF portal_view_anormal = (portal_view_from[i] - portal_view_to[i]).Normalize();
	//Vector3DF portal_cam_normal = (portal_cam_to[i] - portal_cam_from[i]).Normalize();
	Vector3DF portal_cam_normal = (portal_view_to[other(i)] - portal_view_from[other(i)]).Normalize();
	float portal_angle = acos(portal_view_anormal.Dot(portal_cam_normal)) * radToDeg;
	if(bob == 0)
		printf("angle between view and cam: %f\n", portal_angle);
	bob++;
	Vector3DF portal_cross = portal_cam_normal.Cross(portal_view_anormal);
	
	glTranslatef(portal_view_from[i].x, 0, portal_view_from[i].z);
	
	
	glRotatef(portal_angle, portal_cross.x, portal_cross.y, portal_cross.z);

	// go to portal cam location
	//glTranslatef(-portal_cam_from[i].x, 0, -portal_cam_from[i].z);
	glTranslatef(-portal_view_from[other(i)].x, 0, -portal_view_from[other(i)].z);

	//render portal scene
	draw3D();

	// render portal sprites here too?
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	
	drawPanel_portal_sprite(i);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	

	// stop drawing portal image over stencil area
	glStencilFunc (GL_ALWAYS, 0x1, 0x1);	
	glDisable(GL_STENCIL_TEST);
	glPopMatrix();
}
void computeFromPositions ()
{
	// Compute camera pos: Cfrom = Cto + Ry Rx {0,0,1}	
	// Rx = tilt above ground
	// Ry = side-to-side rotation
	cam_from.x = cam_to.x + cos ( cam_angs.x * DEGtoRAD ) * sin ( cam_angs.y * DEGtoRAD ) * cam_dolly;
	cam_from.y = cam_to.y + sin ( cam_angs.x * DEGtoRAD ) * cam_dolly;
	cam_from.z = cam_to.z + cos ( cam_angs.x * DEGtoRAD ) * cos ( cam_angs.y * DEGtoRAD ) * cam_dolly;
}

void computeProjection ()
{
	// ---- Create projection matrix for eye-coordinate transformations 
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective ( 30, window_width / ( float ) window_height, 10.0, 800.0 );
}

void computeView ()
{
	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ();
	glRotatef ( cam_angs.z, 0, 0, 1 );
	gluLookAt ( cam_from.x, cam_from.y, cam_from.z, cam_to.x, cam_to.y, cam_to.z, 0, 1, 0 );
}	

void computePlayerView() 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(player_angle.y, 1, 0, 0);
	glRotatef(player_angle.x, 0, 1, 0);
	glTranslatef(-player_pos.x, -player_pos.y, -player_pos.z);
}
float frame = 0;

void update() {
	if((int)runtime == 0) {
		PORTAL_OPEN[0] = false; PORTAL_OPEN[1] = false;
		portalframe[0] = -1; portalframe[1] = -1;
		portal_view_from[0].Set(0, 10, -20); portal_view_to[0].Set(0, 10, 0);
		portal_view_from[1].Set(20, 10, 0); portal_view_to[1].Set(0, 10, 0);
		bouncy.Set(-100, 100, 0);
		bouncyPortal = false;
		cam_angs.x = 20;		cam_angs.y = 0;		cam_angs.z = 0;
		cam_dolly = 100.0;
		treoY = 10;
	}
	
	// start 3 object spin
	if(runtime > 0) {
		spinAngle += 1;
		
	}
	if(runtime < 10) {
		cam_angs.y = -runtime/10*(45);
		cam_dolly = 100 - runtime/10*(25);
	}
	if(runtime > 12 && runtime < 14) {
		cam_dolly += 0.05;
	}
	if(runtime > 9 && runtime < 15) {
		PORTAL_OPEN[0] = true;
		portalframe[0] += 0.2; if(portalframe[0] > 16.0) portalframe[0] = 16.0;
	}
	if(runtime > 10 && runtime < 20) {
		cam_angs.y -= 0.4*((runtime-10)/10);
	}
	if(runtime > 20 && runtime < 29) {
		cam_angs.y -= 0.4*(1- ((runtime-20)/9));
	}
	
	if(runtime > 30 && runtime < 35) {
		PORTAL_OPEN[1] = true;
		portalframe[1] += 0.2; if(portalframe[1] > 16.0) portalframe[1] = 16.0;
		cam_dolly -= 0.02;
	}
	
	//peak left
	if(runtime > 31 && runtime < 34) {
		cam_angs.y += 0.2*((runtime-31)/3);
	}
	if(runtime > 34 && runtime < 37) {
		cam_angs.y += 0.2*(1- ((runtime-34)/3));
	}
	
	//peak right
	if(runtime > 37 && runtime < 40) {
		cam_angs.y -= 0.2*((runtime-37)/3);
	}
	if(runtime > 40 && runtime < 43) {
		cam_angs.y -= 0.25*(1- ((runtime-40)/3));
	}
	
	//bye bye treo
	if(runtime > 43 && runtime < 46) {
		treoY += 1;
	}
	
	//setup bouncy #1
	if((int)runtime == 45) {
		bouncy.Set(-35, 0, 0);
	}
/* -------------------- 1st bouncy -------------------*/
	// bouncy - bounce!
	if(runtime > 46 && runtime < 57) {
		if(((int)runtime) % 2 == 0)
			bouncy.y += 0.2*(1 - (runtime - ((int)runtime)));
		else
			bouncy.y -= 0.2*(runtime - ((int)runtime) );
	}
	
	// bouncy - move (and collide)
	if(runtime > 46 && runtime < 57) {
		if(!bouncyPortal) {
			bouncy.x += .1;
			if(bouncy.x > 20) {
				bouncyPortal = !bouncyPortal;
				bouncy.x = 0; bouncy.z = -20;
			}
		}
		else 
			bouncy.z += .1;
	}
	
	if(runtime > 43 && runtime < 50) {
		cam_angs.x += 0.03;
		cam_dolly += 0.03;
	}
/* ----------------------- 2nd bouncy -----------------*/
	//setup bouncy #2
	if((int)runtime == 57) {
		bouncy.Set(0, 0, 45);
		bouncyPortal = false;
	}
	
	// bouncy - bounce!
	if(runtime > 58 && runtime < 67) {
		if(((int)runtime) % 2 == 0)
			bouncy.y += 0.2*(1 - (runtime - ((int)runtime)));
		else
			bouncy.y -= 0.2*(runtime - ((int)runtime) );
	}
	
	// bouncy - move (and collide)
	if(runtime > 58 && runtime < 67) {
		if(!bouncyPortal) {
			bouncy.z -= .1;
			if(bouncy.z < -20) {
				bouncyPortal = !bouncyPortal;
				bouncy.x = 20; bouncy.z = 0;
			}
		}
		else 
			bouncy.x -= .1;
	}
	
	if(runtime > 59 && runtime < 61) {
		cam_angs.x -= 0.15*((runtime-59)/2);
		cam_angs.y += 0.05*((runtime-59)/2);
	}
	if(runtime > 61 && runtime < 63) {
		cam_angs.x -= 0.15*(1-((runtime-61)/2));
		cam_angs.y += 0.05*(1-((runtime-61)/2));
	}
	
	if(runtime > 63 && runtime < 68) {
		cam_angs.y += 0.2*((runtime-63)/5);
	}
	if(runtime > 68 && runtime < 71) {
		cam_angs.y += 0.08*(1-((runtime-68)/3));
	}
	
/*-------------- portal swap --------------------------*/

	// close portal
	if(runtime > 65.5 && runtime < 67) {
		portalframe[0] -= 0.3; 
		if(portalframe[0] < -1.0) {
			portalframe[0] = -1.0;
			PORTAL_OPEN[0] = false;
		}
	}
	// reopen portal
	if(runtime > 67 && runtime < 69) {
		if((int)runtime == 67) {
			portal_view_from[0].Set(-20, 10, 0);
			portal_view_to[0].Set(0, 10, 0); 
		}
		PORTAL_OPEN[0] = true;
		portalframe[0] += 0.2; if(portalframe[0] > 16.0) portalframe[0] = 16.0;
	}
	
	if(runtime >= 67 && runtime < 85) {
		//bounce 
		if(((int)runtime) % 2 == 0)
			bouncy.y += 0.2*(1 - (runtime - ((int)runtime)));
		else
			bouncy.y -= 0.2*(runtime - ((int)runtime) );
		
		//move
		bouncy.x -= .1;
		
	}
	//check for collision
	if(runtime >= 67 && runtime < 77) {
		if(bouncy.x < -20) {
			bouncy.x = 20;
		}
	}
	
/* ------------------------ close portals --------------*/
	if(runtime >= 77 && runtime < 82) {
		portalframe[1] -= 0.3; 
		if(portalframe[1] < -1.0) {
			portalframe[1] = -1.0;
			PORTAL_OPEN[1] = true;
		}
		if(bouncy.x < -20) {
			bouncy.x = 100;
			portal_view_from[1].Set(100, 10, 0);
			portal_view_to[1].Set(0, 10, 0);
		}
	}
	
	if(runtime >= 82 && runtime < 86) {
		portalframe[0] -= 0.3; 
		if(portalframe[0] < -1.0) {
			portalframe[0] = -1.0;
			//PORTAL_OPEN[0] = false;
		}
	}
	
	
/* ---------------------- End (loop) --------------------*/
	if(runtime > 89)
		runtime = 0.0;

	runtime += dt;
}
void display () 
{
	update();

	// Set background color and clear
	glClearColor ( 0.1, 0.1, 0.1, 0 );			// r, g, b, a
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );		
	glEnable (GL_DEPTH_TEST);  
	

	// Compute camera view
	computeFromPositions ();
	computeProjection ();
	computeView ();	
	//computePlayerView();
	
	// prepare stencil buffer
	glClearStencil(0x0); //?
	glClear(GL_STENCIL_BUFFER_BIT);

	// draw portal recs to stencil buffer
	generatePortalStencil(0);
	generatePortalStencil(1);

	//glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	draw3D ();
	glPopMatrix();
	glPushMatrix();
	//drawPanel_portal(0);
	glDisable(GL_LIGHTING);
	//drawPortalCam(0);
	glPopMatrix();

	
	
	//glClear(GL_DEPTH_BUFFER_BIT);
	if(PORTAL_OPEN[0] && PORTAL_OPEN[1]) {
	drawPortalToStencil(0);
	drawPortalToStencil(1);
	}
	

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	//glBindTexture(GL_TEXTURE_2D, portalringimg[(int)frame].getID());
	drawPanel_portal_sprite(0);
	drawPanel_portal_sprite(1);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
		
	draw2D ();

	//measureFPS();

	frame += 0.1;
	if (frame > 16) frame = 16.0;

	// Swap buffers
	glutSwapBuffers();  
	glutPostRedisplay();
}

// Reshape - set window size if user resizes window
void reshape ( int width, int height ) 
{
	printf("reshape called\n");
	window_width  = (float) width;
	window_height = (float) height;
	glViewport( 0, 0, width, height );  
}

// Keyboard - accept keyboard input
void keyboard_func ( unsigned char key, int x, int y )
{
	switch( key ) {  
		case 'w': 
			STRAFE_FORWARD = true;
			break;
		case 's':
			STRAFE_BACKWARD = true;
			break;
		case 'a':
			STRAFE_LEFT = true;
			break;
		case 'd':
			STRAFE_RIGHT = true;
			break;
		case 'o': portalframe[1] -= 1.0; break;
		case 'p': portalframe[1] += 1.0; break;
		case 'u': portalframe[0] -= 1.0; break;
		case 'i': portalframe[0] += 1.0; break;
		case 'k': telespherez += 1.0; break;
		case 'l': telespherez -= 1.0; break;
		case 'r': runtime = 0.0; break;
		
		case 27: exit( 0 ); break;
	};
}

void keyboard_up_func(unsigned char key, int x, int y)
{
	switch(key) {
			case 'w': 
			STRAFE_FORWARD = false;
			break;
		case 's':
			STRAFE_BACKWARD = false;
			break;
		case 'a':
			STRAFE_LEFT = false;
			break;
		case 'd':
			STRAFE_RIGHT = false;
			break;
	};
}

// Mouse - accept mouse input
void mouse_click_func ( int button, int state, int x, int y )
{
	if( state == GLUT_DOWN ) {
		if ( button == GLUT_LEFT_BUTTON )		mouse_drag = DRAG_LEFT;
		else if ( button == GLUT_RIGHT_BUTTON ) mouse_drag = DRAG_RIGHT;	
		mouse_lx = x; mouse_ly = y;	
		switch ( iSelect ) {
		case 0:	
			mouse_sx = cam_angs.x;
			mouse_sy = cam_angs.y;
			mouse_sz = cam_dolly;
			break;
		case 1:		// Light
			mouse_sx = light_from.x;
			mouse_sy = light_from.y;
			mouse_sz = light_from.z;			
			break;
		}
	} else {
		mouse_drag = DRAG_OFF;
	}
}


void mouse_move_func ( int x, int y )
{
	
	mouse_x = x;
	mouse_y = y;

	
}

void mouseMoveFunc(int x, int y) 
{	
	float sensitivity = 0.3;

	mouse_lx = mouse_x;
	mouse_ly = mouse_y;
	mouse_x = x;
	mouse_y = y;

	//glutWarpPointer(window_width/2,window_height/2);

	int difx = mouse_x - mouse_lx;
	int dify = mouse_y - mouse_ly;

	player_angle.x = player_angle.x + difx*sensitivity;
	player_angle.y = player_angle.y + dify*sensitivity;
	if(player_angle.y > 90) player_angle.y = 45;
	else if(player_angle.y < -90) player_angle.y = -45;
}

void mouse_drag_func ( int x, int y )
{	
	if ( mouse_drag == DRAG_LEFT ) {
		switch ( iSelect ) {
		case 0:		// Camera 
			cam_angs.x = mouse_sx + (y-mouse_ly)*90/window_height;	// +X-axis rotation = tilt ground forward (== raise camera off ground)
			cam_angs.y = mouse_sy + (x-mouse_lx)*360/window_width;	// +Y-axis rotation = rotate ground
			break;
		case 1:		// Light
			light_from.x = mouse_sx + (x-mouse_lx)*100/window_width;
			light_from.z = mouse_sz + (y-mouse_ly)*100/window_height;
			break;
		}
	} else if ( mouse_drag == DRAG_RIGHT ) {
		switch ( iSelect ) {
		case 0:		// Camera 
			cam_dolly = mouse_sz + (y-mouse_ly)*250/window_height;	// +X-axis rotation = tilt ground forward (== raise camera off ground)			
			break;
		case 1:		// Light			
			light_from.y = mouse_sy - (y-mouse_ly)*100/window_height;
			break;
		}
	}	
}

void idle_func ()
{
	/*
	float step = 0.5;
	//cubeAngle += 0.2;
	//spinAngle += 0.3;
	
	if(STRAFE_LEFT) player_pos.x -= step;
	if(STRAFE_RIGHT) player_pos.x += step;
	if(STRAFE_FORWARD) player_pos.z -= step;
	if(STRAFE_BACKWARD) player_pos.z += step;
	
	*/
		
	
}

void init ()
{
	/*
	// Setup texture
	glEnable (GL_TEXTURE_2D);
	glGenTextures (1, &texture_id);
	glBindTexture (GL_TEXTURE_2D, texture_id);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB,
	GL_UNSIGNED_BYTE, texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	*/

	// setup textures
	tileimg.read("textures/tile.bmp");
	tileimg.refresh();

	portalringimg[0].read("textures/ring1.bmp", "textures/ring1a.bmp");
	portalringimg[0].refresh();
	portalringimg[1].read("textures/ring2.bmp", "textures/ring2a.bmp");
	portalringimg[1].refresh();
	portalringimg[2].read("textures/ring3.bmp", "textures/ring3a.bmp");
	portalringimg[2].refresh();
	portalringimg[3].read("textures/ring4.bmp", "textures/ring4a.bmp");
	portalringimg[3].refresh();
	portalringimg[4].read("textures/ring5.bmp", "textures/ring5a.bmp");
	portalringimg[4].refresh();
	portalringimg[5].read("textures/ring6.bmp", "textures/ring6a.bmp");
	portalringimg[5].refresh();
	portalringimg[6].read("textures/ring7.bmp", "textures/ring7a.bmp");
	portalringimg[6].refresh();
	portalringimg[7].read("textures/ring8.bmp", "textures/ring8a.bmp");
	portalringimg[7].refresh();
	portalringimg[8].read("textures/ring9.bmp", "textures/ring9a.bmp");
	portalringimg[8].refresh();
	portalringimg[9].read("textures/ring10.bmp", "textures/ring10a.bmp");
	portalringimg[9].refresh();
	portalringimg[10].read("textures/ring11.bmp", "textures/ring11a.bmp");
	portalringimg[10].refresh();
	portalringimg[11].read("textures/ring12.bmp", "textures/ring12a.bmp");
	portalringimg[11].refresh();
	portalringimg[12].read("textures/ring13.bmp", "textures/ring13a.bmp");
	portalringimg[12].refresh();
	portalringimg[13].read("textures/ring14.bmp", "textures/ring14a.bmp");
	portalringimg[13].refresh();
	portalringimg[14].read("textures/ring15.bmp", "textures/ring15a.bmp");
	portalringimg[14].refresh();
	portalringimg[15].read("textures/ring16.bmp", "textures/ring16a.bmp");
	portalringimg[15].refresh();
	portalringimg[16].read("textures/ring17.bmp", "textures/ring17a.bmp");
	portalringimg[16].refresh();



	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// Setup lighting
	glEnable ( GL_LIGHT0 );
	float ambient[] =  {0.6f, 0.6f, 0.6f, 1.0f};
	float diffuse[] =  {1.0f, 1.0f, 1.0f, 1.0f};
	float specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
	float position[] = {10.0f, 10.0f, 10.0f, 1.0f};    
	float shininess = 2.0;
	position[0] = light_from.x; position[1] = light_from.y; position[2] = light_from.z;

	glLightfv ( GL_LIGHT0, GL_AMBIENT, ambient );			
	glLightfv ( GL_LIGHT0, GL_DIFFUSE, diffuse );	
	glLightfv ( GL_LIGHT0, GL_POSITION, position ); 	
	glMaterialfv ( GL_FRONT, GL_SPECULAR, specular );
	glMaterialf ( GL_FRONT, GL_SHININESS, shininess);

	glClearStencil(0x0);
	//glEnable(GL_STENCIL_TEST);
}

// Main program
int main ( int argc, char **argv )
{
	printf ( "Hello world. (This should appear in console window)\n");

	// Set up the GLUI window
	glutInit( &argc, &argv[0] ); 
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition( 100, 100 );
	glutInitWindowSize( (int) window_width, (int) window_height );
	glutCreateWindow ( "1scene" );
	init();

	// GLUT callbacks
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyboard_func );
	glutKeyboardUpFunc(keyboard_up_func);
	glutMouseFunc( mouse_click_func );  
	glutMotionFunc( mouse_drag_func );
	glutPassiveMotionFunc( mouse_move_func );
	//glutPassiveMotionFunc(mouseMoveFunc);
	glutIdleFunc( idle_func );
	//glutSetCursor ( GLUT_CURSOR_NONE );

	// Basic opengl setup
	glShadeModel( GL_SMOOTH );	
	
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	// Setup camera and light positions
	cam_angs.x = 20;		cam_angs.y = 0;		cam_angs.z = 0;
	cam_to.x = 0;		cam_to.y = 8;		cam_to.z = 0;
	cam_dolly = 100.0;

	light_from.x = 0;	light_from.y = 50.0;	light_from.z = 0;
	light_to.x = 0;	light_to.y = 0;	light_to.z = 0;	
	light_fov = 45;	

	portal_view_from[0].Set(0, 10, -20);		portal_view_from[1].Set(0, 10, 20);
	portal_view_to[0].Set(0, 10, 0);		portal_view_to[1].Set(0, 10, 0);

	// not needed. other portal is 'cam'
	portal_cam_from[0].Set(20, 5, 0);		portal_cam_from[1].Set(0, 5, -20);
	portal_cam_to[0].Set(0, 5, 0);			portal_cam_to[1].Set(0, 5, 0);

	//player
	player_pos.x = 5; player_pos.y = 10; player_pos.z = 40;
	player_angle.x = 0;	player_angle.y = 0; player_angle.z = 0;

	portalframe[0] = 0.0; portalframe[1] = 0.0; kradius = 0;
	runtime = 0.0; dt = 0.01;

	mouse_x = window_width/2; mouse_y = window_height/2;
	// put pointer in center of screen
	glutWarpPointer(window_width / 2, window_height / 2);
	
	//runtime = 44;
	PORTAL_OPEN[0] = true; PORTAL_OPEN[1] = true;

	// Run program..
	glutMainLoop();

	return 0;	// Done.
}
