// Graphics Prg 5 for Aaron Nguyen

/*===========================================================
source.cpp

The program creates a scene of a sphere, spindle, and a spinning UAH emblem.The animation of the 
emblem is achieved by utilizing a timer event handler, timer_func, to update the rotation variable, 
rotate, every 25 milisecons and then invokes the display handler, display_func, 
to draw the frame-by-frame animation.The rotations offset is 4.5 degrees which allows for
full rotations to take 2 seconds to complete. 

The display handler utilizes double buffering for frame-by-frame animation of the spinning 
spinning. The animation will be started when main registers the first timer_func event handler. 
The frame will be drawn with a perspective projection defined in function,my_setup(). 
main will also define a viewing transformation for the perspective projection using
gluLookAt() in function init_setup(). The perspectice projection is positioned at the 
world origin (0,0,0) with a reference point of (0,0,-1). 

Functions make_U(), make_A(), make_H(), make_sphere(), and make_spindle() utilized 
illumination effects functions called enable_lightSolid() solid objects and enable_light()
for wireframe objects. enable_lightSolid() enables ambient, diffuse, and specular lighting
using light source 1. enable_light() enables ambiend and diffuse lighting only. Both functions
position the light source at the world origin (0,0,0) and enable shininess material to objects.
Both functions also enable smooth shading to objects. 

EXTRA
The keyboard event handler, keyInput_func, is used to apply positional movements to the UAH emblem. 
Upon pressing 'H' and 'J' will move the emblem left and right respectfully by 25 units.

Upon pressing 'K' and 'L' will increase and decrease the light source intensity respectfully for 
ambient, diffuse, and specular lighting. Each key press event of 'K' and 'L' will change the 
light intensity by 10% offsets. 

===========================================================*/


#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup-aug24.h"	/* Include configurations settings for OpenGL and GLUT*/
#include <cmath>            		/* Needed to perform trigonometric calculations */
#include <string>					/* Needed to perform string operations to print bitmap characters */
#include <iostream>

float rotate;						/* rotation offset for UAH */
float intensity;					/* light source intensity */
float x_offset;						/* x offset for letter and spindle movement */

/*===========================================================
enable_lightSource

Enable lighting source and position
===========================================================*/
void enable_lightSource()
{
	float lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};	// light position above center of top of tank
	
	glEnable(GL_LIGHTING);
	
	glEnable(GL_LIGHT1); 

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
}

/*===========================================================
enable_light

Enable lighting and material properties for solid objects

INPUT
float r : red color value 
float g : green color value 
float b : blue color value 
float intensity : light source intensity
===========================================================*/
void enable_lightSolid(float r, float g, float b, float intensity)
{
	enable_lightSource();

	float lightAmb[] = {0.1f * intensity, 0.1f * intensity, 0.1f * intensity, 1.0f};
	float lightDif[] = {0.7f * intensity, 0.7f * intensity, 0.7f * intensity, 1.0f};
	float lightSpc[] = {intensity, intensity, intensity, 1.0f};

	float matAmbAndDif[] = { r, g, b, 1.0 };
	float matSpec[] = { 0.9f, 0.9f, 0.9f, 1.0 };
	float matShine[] = { 50.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); 

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);

	glShadeModel(GL_SMOOTH);
}

/*===========================================================
enable_light

Enable lighting and material properties for other objects 
===========================================================*/
void enable_light(float r, float g, float b, float intensity)
{
	enable_lightSource();

	float lightAmb[] = {0.1f * intensity, 0.1f * intensity, 0.1f * intensity, 1.0f};
	float lightDif[] = {0.7f * intensity, 0.7f * intensity, 0.7f * intensity, 1.0f};

	float matAmbAndDif[] = { r, g, b, 1.0 };
	float matShine[] = { 50.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); 

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);

	glShadeModel(GL_SMOOTH);
}

/*===========================================================
disable_lightAmb

Disable lighting and material properties
===========================================================*/
void disable_light()
{
	float lightAmb[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float lightDif[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float lightSpc[] = {0.0f, 0.0f, 0.0f, 1.0f};

	float matAmb[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float matDif[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float matSpc[] = {0.0f, 0.0f, 0.0f, 1.0f};

	glDisable(GL_LIGHTING);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpc);

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpc);
}

/*===========================================================
make_U

Draws and enables illumination effects for letter U
===========================================================*/
void make_U()
{

	glPushMatrix();
	glLoadIdentity();
	enable_lightSolid(0.0f, 0.529f, 0.889f, intensity);
	glPopMatrix();

	for (int x = -25; x <= 25; x += 50)
	{
		glPushMatrix();
		glTranslatef(x,0.0f,0.0f);
		glutSolidCube(50.0f);
		glPopMatrix();
	}

	for (int x = -75; x <= 75; x += 150)
	{
		for (int y = 150; y >= 0; y -= 50)
		{
			glPushMatrix();
			glTranslatef(x,y,0.0f);
			glutSolidCube(50.0f);
			glPopMatrix();
		}
	}	

	glPushMatrix();
	glLoadIdentity();
	disable_light();
	glPopMatrix();

}

/*===========================================================
make_A

Draws and enables illumination effects for letter A
===========================================================*/
void make_A()
{
	glPushMatrix();
	glLoadIdentity();
	enable_light(0.753f, 0.753f, 0.753f, intensity);
	glPopMatrix();

	int space = 25;
	for (int y = 150; y >= 0; y -= 50)
	{
		for (int x = -space; x <= space; x += space*2)
		{
			glPushMatrix();
			glTranslatef(x,y,0.0f);
			glutWireCube(50.0f);
			glPopMatrix();
		}
		space += 25;
	}

	for (int x = -25; x <= 25; x += 50)
	{
		glPushMatrix();
		glTranslatef(x,50.0f,0.0f);
		glutWireCube(50.0f);
		glPopMatrix();
	}

	glPushMatrix();
	glLoadIdentity();
	disable_light();
	glPopMatrix();

}

/*===========================================================
make_H

Draws and enables illumination effects for letter H
===========================================================*/
void make_H()
{
	glPushMatrix();
	glLoadIdentity();
	enable_lightSolid(0.0f, 0.529f, 0.889f, intensity);
	glPopMatrix();

	for (int x = -25; x <= 25; x += 50)
	{
		glPushMatrix();
		glTranslatef(x,50.0f,0.0f);
		glutSolidCube(50.0f);
		glPopMatrix();
	}
	
	for (int x = -75; x <= 75; x += 150)
	{
		for (int y = 150; y >= 0; y -= 50)
		{
			glPushMatrix();
			glTranslatef(x,y,0.0f);
			glutSolidCube(50.0f);
			glPopMatrix();
		}
	}

	glPushMatrix();
	glLoadIdentity();
	disable_light();
	glPopMatrix();

}

/*===========================================================
make_UAH

Draws UAH letters 
===========================================================*/
void make_UAH()
{

	glPushMatrix();
	glTranslatef(-275.0f, 0.0f, 0.0f);
	make_U();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	make_A();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(275.0f, 0.0f, 0.0f);
	make_H();
	glPopMatrix();
	
}

/*===========================================================
make_sphere

Draws and enables illumination effects for sphere
===========================================================*/
void make_sphere()
{
	glPushMatrix();
	glLoadIdentity();
	enable_lightSolid(1.0f, 0.031f, 0.0f, intensity);
	glPopMatrix();

	glPushMatrix();
	glutSolidSphere(25.0f,200,200);
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	disable_light();
	glPopMatrix();

}

/*===========================================================
make_spindle

Draws and enables illumination effects for spindle
===========================================================*/
void make_spindle()
{
	glPushMatrix();
	glLoadIdentity();
	enable_light(0.502f, 0.502f, 0.502f, intensity);
	glPopMatrix();

	glPushMatrix();
	glBegin(GL_TRIANGLES);
		glVertex3f(0.0f,175.0f,0.0f);
		glVertex3f(-25.0f,250.0f,0.0f);
		glVertex3f(25.0f,250.0f,0.0f);
	glEnd();
	glPopMatrix();


	glPushMatrix();
	glLoadIdentity();
	disable_light();
	glPopMatrix();
}

/*===========================================================
keyInput_func

Keyboard event handler to handle letter and spindle movement

INPUT
unsigned char key: store character that key was pressed
int x: store x position of cursor at time of key press
int y: store y position of cursor at time of key press

===========================================================*/
void keyInput_func(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'h':
		x_offset -= 25.0f;							/* move left 25 units */
	break;	
	case 'j':
		x_offset += 25.0f;							/* move right 25 units */
	break;

	case 'k':
		if (intensity >= 1.0f) break;				/* rail intensity value */
		intensity += 0.1f;
		glutPostRedisplay();						/* redraw canvas */
	break;

	case 'l':
		if (intensity <= 0.0f) break;				/* rail intensity value */
		intensity -= 0.1f;
		glutPostRedisplay();						/* redraw canvas */
	break;	
	
	default:	
	
	break;
	
	}

}

/*===========================================================
timer_func

Timer event handler to perform animation

INPUT
int val: ID of timer event
===========================================================*/
void timer_func(int val)
{
	// Animation Timer Event Handler
	if (val == 1)
	{
		rotate += 4.5;								/* rotate UAH letters */
		glutPostRedisplay();						/* redraw canvas */
		glutTimerFunc(25,timer_func,1);				/* Call timer_func */
	}
}

/*===========================================================
display_func

Display handler to draw UAH letters, sphere, and spindle, 
and draw rotation 
===========================================================*/
void display_func()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers including the depth buffer.
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// black background	  

	glPushMatrix();
	glTranslatef(10.0f + x_offset, -75.0f, -400.0f);
	make_sphere();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10.0f + x_offset, -175.0f, -400.0f);
	make_spindle();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10.0f + x_offset, -175.0f, -400.0f);
	glRotatef(rotate,0.0f,1.0f,0.0f);
	make_UAH();
	glPopMatrix();   


	glutSwapBuffers();
}


/*===========================================================
init_setup

Initialize all global variables, camera, and depth buffer
===========================================================*/
void init_setup()
{
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST); // Enable depth buffer 

	rotate = 0.0f;

	intensity = 1.0f;

	x_offset = 0.0f;

}

#define canvas_Width 800 			/* width of canvas */
#define canvas_Height 800 			/* height of canvas */
char canvas_Name[]= "Program 5"; 	/* name in window top */

/*===========================================================
main 

Main set initial values of global variables and the setup of 
the OpenGL canvas. Main also registers event handlers. Main
also set up display list for objects.  
===========================================================*/
int main(int argc, char ** argv)
{

	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);
	init_setup();

	glutDisplayFunc(display_func); 			/* registers the display callback */
	glutKeyboardFunc(keyInput_func);		/* registers the keyboard callback */

	glutTimerFunc(25,timer_func,1);			/* Call timer_func */

	glutMainLoop(); 						/* execute until killed */
	return 0;
}