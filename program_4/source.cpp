// Graphics Prg 4 for Aaron Nguyen

/*===========================================================
source.cpp

The program creates a scene of a fish tank with two fish and spinning fan blades.
The animation of the fan blades is achieved by utilizing a timer event handler, 
timer_func, to update the rotation variable, rotate, every 25 milisecons and then invokes
the display handler, display_func, to draw the frame-by-frame animation.

The display handler utilizes double buffering for frame-by-frame animation of the spinning 
fan blades and the positional updates for each fish. The animation will be started 
when main registers the first timer_func event handler. A second timer_func event handler
is used every 1000 miliseconds to simulate a timer and stops the animation of the spinning
fan blades and movement of the fish when 30 seconds has elapsed. The timer is drawn using
bitmap characters. The frame will be drawn with a perspective projection defined in function,
my_setup(). main will also define a viewing transformation for the perspective projection using
function, camera_angle().

The display_func also enables ambient lighting for the spinning fan blades. The function,
enable_lightAmb(), enables ambient lighting with light source 1. The position of the ambient 
light is above the center of the top of the fish tank. The light intensity utilizes an RGBA
floating point values with a main foucus on red intensity. The material property of the fan blades
are enabled with front face triangle updates and an ambient reflectance with a main focus on red 
reflectance on the material. 

The keyboard event handler, keyInput_func, is used to apply positional movements to the fish
in the fish tank. Upon pressing 'H' and 'J' will move the bright orange big fish left and right respectfully. 
Upon pressing 'U' and 'N' will move the gray small fish up and down respectfully. All movements move
the respective fish by 10 units. The keyboard_func also handles object collision between the both fish 
and tank walls and between fish object collision using the function checkCollision(). The function, 
checkCollision(), will check collision between both fish and if there is collition, set a collision 
check key, collide_key, to the last key that caused the collision. keyInput_func will prevent the fish 
from moving if the same key is pressed again. If there still is a collision, the key that was last casued 
the collision will still prevent the movement until there is no collision detected. 

EXTRA
Each time the fish collides with the tank boundaries defined in keyInput_func, it will perform a 
rotation of 180 degrees with respect to the y-axis to simuilate the fish reversing its orintation. 
A button drawn on the top right corner of the front face of the fish tank that serves as a toggle 
switch to stop animations and the timer. A mouse input event handler, mouseInput_func is used to 
detect if the button has been pushed by checking collisions between mouse press position and the 
bounding box of the button. If there is a collision, the button will toggle boolean variable, stop, 
and stop all animations and timer. 

===========================================================*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup-aug24.h"	/* Include configurations settings for OpenGL and GLUT*/
#include <cmath>            		/* Needed to perform trigonometric calculations */
#include <string>					/* Needed to perform string operations to print bitmap characters */

// Global Variables
float rotate;						/* rotation offset for fan blades */
float flip_bigFish;					/* EXTRA: rotation offset for bright orange big fish */
float flip_smallFish;				/* EXTRA: rotation offset for gray small fish */

bool stop;							/* boolean to stop all animation and movement */

float x_offset;						/* x position offset for bright orange big fish */
float y_offset;						/* y position offset for gray small fish*/

unsigned char collide_key;			/* last collision check key */
float still_hit;					/* boolean if collision occurs in next frame */

int timer_num;						/* timer count */
std::string timer;					/* string for timer count */

// Bright Orange Big Fish Bounding Box
struct bigFishBoundBox {
	float left = -37.5f;	
	float right = 52.5f;	
	float top = 15.0f;
	float bottom = -15.0f;
	float front = -162.5f;
	float back = -187.5f;
} bigFishBox;

// Gray Small Fish Bounding Box
struct smallFishBoundBox {
	float left = -25.0f;	
	float right = 37.0f;	
	float top = -22.5f;
	float bottom = -47.5f;
	float front = -165.0f;
	float back = -185.0f;
} smallFishBox;

// Fish Tank Bounding Box
struct tankBoundBox {
	float left = -125.0f;	
	float right = 125.0f;	
	float top = 125.0f;
	float bottom = -125.0f;
	float near = -50.0f;
	float far = 200.0f;
} tankBox;

// EXTRA: Toggle Button Bounding Box
struct buttonBoundBox {
	float left = 552.0f;	
	float right = 570.0f;	
	float top = 158.0f;
	float bottom = 142.0f;
} buttonBox;

/*===========================================================
update_bigFish

Update left and right of big fish bounding box

INPUT
bigFishBoundBox ptr: reference to bounding box definition to update 
===========================================================*/
void update_bigFish(bigFishBoundBox& ptr)
{
	ptr.left = -37.5 + x_offset;
	ptr.right = 52.5f + x_offset;
}

/*===========================================================
update_smallFish

Update top and bottom of small fish bounding box

INPUT
update_smallFish ptr: reference to bounding box definition to update 
===========================================================*/
void update_smallFish(smallFishBoundBox& ptr)
{
	ptr.top = -22.5f + y_offset;
	ptr.bottom = -47.5f + y_offset;
}

/*===========================================================
make_bigFishBox

Helper function to draw bounding box for big fish
===========================================================*/
void make_bigFishBox()
{
	glColor3f(1.0f,1.0f,1.0f);

	glBegin(GL_LINES);
		// top
		glVertex3f(bigFishBox.left,bigFishBox.top,bigFishBox.front);
		glVertex3f(bigFishBox.left,bigFishBox.top,bigFishBox.back);
				
		glVertex3f(bigFishBox.left,bigFishBox.top,bigFishBox.back);
		glVertex3f(bigFishBox.right,bigFishBox.top,bigFishBox.back);

		glVertex3f(bigFishBox.right,bigFishBox.top,bigFishBox.back);
		glVertex3f(bigFishBox.right,bigFishBox.top,bigFishBox.front);

		glVertex3f(bigFishBox.right,bigFishBox.top,bigFishBox.front);
		glVertex3f(bigFishBox.left,bigFishBox.top,bigFishBox.front);

		// bottom
		glVertex3f(bigFishBox.left,bigFishBox.bottom,bigFishBox.front);
		glVertex3f(bigFishBox.left,bigFishBox.bottom,bigFishBox.back);
				
		glVertex3f(bigFishBox.left,bigFishBox.bottom,bigFishBox.back);
		glVertex3f(bigFishBox.right,bigFishBox.bottom,bigFishBox.back);

		glVertex3f(bigFishBox.right,bigFishBox.bottom,bigFishBox.back);
		glVertex3f(bigFishBox.right,bigFishBox.bottom,bigFishBox.front);

		glVertex3f(bigFishBox.right,bigFishBox.bottom,bigFishBox.front);
		glVertex3f(bigFishBox.left,bigFishBox.bottom,bigFishBox.front);

		// left
		glVertex3f(bigFishBox.left,bigFishBox.top,bigFishBox.front);
		glVertex3f(bigFishBox.left,bigFishBox.bottom,bigFishBox.front);

		glVertex3f(bigFishBox.left,bigFishBox.top,bigFishBox.back);
		glVertex3f(bigFishBox.left,bigFishBox.bottom,bigFishBox.back);

		// right
		glVertex3f(bigFishBox.right,bigFishBox.top,bigFishBox.front);
		glVertex3f(bigFishBox.right,bigFishBox.bottom,bigFishBox.front);

		glVertex3f(bigFishBox.right,bigFishBox.top,bigFishBox.back);
		glVertex3f(bigFishBox.right,bigFishBox.bottom,bigFishBox.back);

	glEnd();
}

/*===========================================================
make_smallFishBox

Helper function to draw bounding box for small fish
===========================================================*/
void make_smallFishBox()
{
	glColor3f(1.0f,1.0f,1.0f);

	glBegin(GL_LINES);
		// top
		glVertex3f(smallFishBox.left,smallFishBox.top,smallFishBox.front);
		glVertex3f(smallFishBox.left,smallFishBox.top,smallFishBox.back);
				
		glVertex3f(smallFishBox.left,smallFishBox.top,smallFishBox.back);
		glVertex3f(smallFishBox.right,smallFishBox.top,smallFishBox.back);

		glVertex3f(smallFishBox.right,smallFishBox.top,smallFishBox.back);
		glVertex3f(smallFishBox.right,smallFishBox.top,smallFishBox.front);

		glVertex3f(smallFishBox.right,smallFishBox.top,smallFishBox.front);
		glVertex3f(smallFishBox.left,smallFishBox.top,smallFishBox.front);

		// bottom
		glVertex3f(smallFishBox.left,smallFishBox.bottom,smallFishBox.front);
		glVertex3f(smallFishBox.left,smallFishBox.bottom,smallFishBox.back);
				
		glVertex3f(smallFishBox.left,smallFishBox.bottom,smallFishBox.back);
		glVertex3f(smallFishBox.right,smallFishBox.bottom,smallFishBox.back);

		glVertex3f(smallFishBox.right,smallFishBox.bottom,smallFishBox.back);
		glVertex3f(smallFishBox.right,smallFishBox.bottom,smallFishBox.front);

		glVertex3f(smallFishBox.right,smallFishBox.bottom,smallFishBox.front);
		glVertex3f(smallFishBox.left,smallFishBox.bottom,smallFishBox.front);

		// left
		glVertex3f(smallFishBox.left,smallFishBox.top,smallFishBox.front);
		glVertex3f(smallFishBox.left,smallFishBox.bottom,smallFishBox.front);

		glVertex3f(smallFishBox.left,smallFishBox.top,smallFishBox.back);
		glVertex3f(smallFishBox.left,smallFishBox.bottom,smallFishBox.back);

		// right
		glVertex3f(smallFishBox.right,smallFishBox.top,smallFishBox.front);
		glVertex3f(smallFishBox.right,smallFishBox.bottom,smallFishBox.front);

		glVertex3f(smallFishBox.right,smallFishBox.top,smallFishBox.back);
		glVertex3f(smallFishBox.right,smallFishBox.bottom,smallFishBox.back);

	glEnd();
}

/*===========================================================
make_bigFish

Set up display list to draw big fish at center of fish tank
===========================================================*/
static unsigned int bigFish;    
void make_bigFish()
{
	bigFish = glGenLists(1);
	glNewList(bigFish,GL_COMPILE);
	
	glColor3f(1.0f,0.65f,0.0f);		// Bright Orange 
	glPushMatrix();
	glScalef(37.5f,15.0f,12.5f);	
	glutWireOctahedron();
	glPopMatrix();

	glPushMatrix();
	glBegin(GL_LINES);
		glVertex3f(37.5f,0.0f,0.0f);
		glVertex3f(52.5f,7.5f,0.0f);

		glVertex3f(52.5f,7.5f,0.0f);
		glVertex3f(52.5f,-7.5f,0.0f);

		glVertex3f(52.5f,-7.5f,0.0f);
		glVertex3f(37.5f,0.0f,0.0f);
		
		// Yellow stripe
		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f(42.5f,0.0f,0.0f);
		glVertex3f(52.5f,0.0f,0.0f);
	glEnd();
	glPopMatrix();
	glEndList();
}

/*===========================================================
make_smallFish

Set up display list to draw small fish directly below the big fish
===========================================================*/
static unsigned int smallFish;            
void make_smallFish()
{
	smallFish = glGenLists(1);
	glNewList(smallFish,GL_COMPILE);
	
	glColor3f(0.576f,0.592f,0.6f);		// gray
	glPushMatrix();
	glScalef(25.0f,12.5f,10.0f);
	glutWireOctahedron();
	glPopMatrix();

	glPushMatrix();
	glBegin(GL_LINES);
		glVertex3f(25.0f,0.0f,0.0f);
		glVertex3f(37.0f,6.0f,0.0f);

		glVertex3f(37.0f,6.0f,0.0f);
		glVertex3f(37.0f,-6.0f,0.0f);

		glVertex3f(37.0f,-6.0f,0.0f);
		glVertex3f(25.0f,0.0f,0.0f);
	glEnd();
	glPopMatrix();
	glEndList();
}

/*===========================================================
make_fishTank

Set up display list to draw fish tank with size 250
===========================================================*/
static unsigned int fishTank;            
void make_fishTank()
{
	fishTank = glGenLists(1);
	glNewList(fishTank,GL_COMPILE);
	
	glColor3f(0.2f,1.0f,1.0f);	// aqua blue

	glPushMatrix();
	glutWireCube(250.0f);
	glPopMatrix();
	glEndList();
}

/*===========================================================
make_tankFan

Set up display list to draw fan blades
===========================================================*/
static unsigned int tankFan;            
void make_tankFan()
{
	tankFan = glGenLists(1);
	glNewList(tankFan,GL_COMPILE);
	
	for (int deg = 0; deg <= 360; deg += 60)
	{
		glPushMatrix();
		glRotatef(deg,0,0,1);
		glBegin(GL_TRIANGLES);
			glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(-13.5f,50.0f,0.0f);
			glVertex3f(13.5f,50.0f,0.0f);
		glEnd();
		glPopMatrix();
	}

	glEndList();
}

/*===========================================================
EXTRA : make_toggleSwitch

Draw toggle button at top right corner of front face of fish tank 
===========================================================*/
void make_toggleButton()
{
	float splices = 40.0f;
	float radius = 8.0f;
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(110.0f, 110.0f, -50.0f);
	for (int i = 0; i <= splices; ++i)
	{
		float t = 2 * M_PI * i / splices;
		glVertex3f(110.0f + cos(t) * radius, 110.0f + sin(t) * radius, -50.0f);
	}
	glEnd();
}

/*===========================================================
bitmapStringOut

Draw string onto canvas 

INPUT
string str: message to process and draw onto canvas
===========================================================*/
void bitmapStringOut(std::string str)
{
	int len = str.length();
	for (int i = 0;i < len; i++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
}

/*===========================================================
enable_lightAmb

Enable ambient lighting and material properties
===========================================================*/
void enable_lightAmb()
{
	float lightPos[] = {0.0f, 150.0f, -175.0f, 1.0f};	// light position above center of top of tank
	float lightAmb[] = {0.8f, 0.0f, 0.0f, 1.0f};
	float matAmb[] = {1.0f, 0.0f, 0.0f, 1.0f};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1); // Enable light source 0

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); 

	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
}

/*===========================================================
disable_lightAmb

Disable ambient lighting and material properties
===========================================================*/
void disable_lightAmb()
{
	float noAmb[] = {0.0f, 0.0f, 0.0f, 1.0f};  // Zero intensity for ambient light
	float noMatAmb[] = {0.0f, 0.0f, 0.0f, 1.0f};

	glDisable(GL_LIGHTING);
	glLightfv(GL_LIGHT1, GL_AMBIENT, noAmb);
	glMaterialfv(GL_FRONT, GL_AMBIENT, noMatAmb);

}

/*===========================================================
checkCollision

Check collision between the big and small fish. If there is a 
collision, last key press will be disabled for movement. If there
continues to have collisions, then return and save last key. 

INPUT
unsigned char key : key press that will be used to save as last key
if collision occurs
===========================================================*/
void checkCollision(unsigned char key)
{
	// check left and right interections
	if (bigFishBox.right >= smallFishBox.left && bigFishBox.left <= smallFishBox.right) 
	{	
		// check top and bottom intersections
		if (bigFishBox.bottom <= smallFishBox.top && bigFishBox.top >= smallFishBox.bottom)
		{	
			// if collision still occurs from last frame, return and save last key
			if (still_hit)	
			{
				return;
			}
			// new collision check key 
			else
			{
				collide_key = key;
				still_hit = true;
			}
		}
		// no collision
		else
		{
			collide_key = '\0';
			still_hit = false;
		}
	}
	// no collision
	else 
	{
		collide_key = '\0';
		still_hit = false;
	}
}

/*===========================================================
keyInput_func

Keyboard event handler to handle fish movement and collision detection

INPUT
unsigned char key: store character that key was pressed
int x: store x position of cursor at time of key press
int y: store y position of cursor at time of key press

===========================================================*/
void keyInput_func(unsigned char key, int x, int y)
{
	// Do not update fish position when animation stop 
	if (stop) return;

	switch (key)
	{
	// move big fish left and right
	case 'h':
		// check tank boundary collision
		if (bigFishBox.left <= tankBox.left)
		{
			x_offset = x_offset;		/* stop movement */
			flip_bigFish += 180;		/* EXTRA : rotate big fish when hit tank boundary*/
		}
		// check collision key (last key that caused collision)
		else if (collide_key == 'h')
		{
			x_offset = x_offset;		/* stop movement */
		}
		// no collison, update position and bound box
		else
		{
			x_offset -= 10;
			update_bigFish(bigFishBox);
		}
	break;
	case 'j':
		// check tank boundary collision
		if (bigFishBox.right >= tankBox.right)
		{
			x_offset = x_offset;		/* stop movement */
			flip_bigFish += 180;		/* EXTRA : rotate big fish when hit tank boundary*/
		}
		// check collision key (last key that caused collision)
		else if (collide_key == 'j')
		{
			x_offset = x_offset;		/* stop movement */
		}
		// no collision, update position and boud box
		else 
		{
			x_offset += 10;
			update_bigFish(bigFishBox);
		}
	break;

	// move small fish up and down 
	case 'u':
		// check tank boundary collision
		if (smallFishBox.top >= tankBox.top)
		{
			y_offset = y_offset;		/* stop movement */
			flip_smallFish += 180;		/* EXTRA : rotate small fish when hit tank boundary */
		}
		// check collision key (last key that caused collision)
		else if (collide_key == 'u')
		{
			y_offset = y_offset;		/* stop movement*/
		}
		// no collision, update position and boud box
		else
		{
			y_offset += 10;
			update_smallFish(smallFishBox);
		}
	break;
	case 'n':
		// check tank boundary 
		if (smallFishBox.bottom <= tankBox.bottom)
		{
			y_offset = y_offset;		/* stop movement */
			flip_smallFish += 180;		/* EXTRA : rotate small fish when hit tank boundary */
		}
		// check collision key (last key that caused collision)
		else if (collide_key == 'n')
		{
			y_offset = y_offset;		/* stop mocement */
		}
		// no collision, update position and boud box
		else
		{
			y_offset -= 10;
			update_smallFish(smallFishBox);
		}
	break;	
	
	default:	
	
	break;
	
	}

	checkCollision(key);	/* perform collision detection */
}

/*===========================================================
timer_func

Timer event handler to perform animation and timer count 

INPUT
int val: ID of timer event
===========================================================*/
void timer_func(int val)
{
	// Animation Timer Event Handler
	if (val == 1 && !stop)
	{
		rotate += 4.5;								/* roate fan blades */
		glutPostRedisplay();						/* redraw canvas */
		glutTimerFunc(25,timer_func,1);				/* Call timer_func */
	}

	// Time count Timer Event Handler
	if (val == 2 && !stop)
	{
		timer_num -= 1;							
		timer = std::to_string(timer_num);		/* convert timer count to string */
		glutTimerFunc(1000,timer_func,2);		/* Call timer_func */
		if (timer_num == 0) 
		{
			stop = true;
			glutPostRedisplay();				/* send last frame before stop animation */	
		}
	}
}

/*===========================================================
mouseInput_func

Mouse Input event handler to handle toggle button press 

INPUT
int button: mouse press used to press toggle button
int state: state of mouse press when pressing toggle button
int x: x position of mouse press
int y: y position of mouse press
===========================================================*/
void mouseInput_func(int button, int state, int x, int y)
{
	// left click onto toggle button 
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// check if toggle button was pressed 
		if (x >= buttonBox.left && x <= buttonBox.right)
		{
			if (y >= buttonBox.bottom && y <= buttonBox.top)
			{
				stop = !stop;
				glutPostRedisplay();						/* send last frame before stop animation */	

				if (!stop)
				{
					glutTimerFunc(25,timer_func,1);			/* Call timer_func to draw frame */
					glutTimerFunc(1000,timer_func,2);		/* Call timer_func to continue trimer */	
				}
			}
		}
	}
}

/*===========================================================
display_func

Display handler to draw timer count, fish tank, fish, spinning 
fan blades, and draw animations. 

===========================================================*/
void display_func()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers including the depth buffer.
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// black background	

	// draw timer
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(275.0, 275.0, -300.0);        
	bitmapStringOut(timer);        


	// draw fish tank
	glPushMatrix();
	glTranslatef(0.0f,0.0f,-175.0f);
	glCallList(fishTank);	
	glPopMatrix();

	if (!stop) glColor3f(1.0f, 1.0f, 1.0f);
	else glColor3f(1.0f, 0.0f, 0.0f);
	make_toggleButton();
	
	// draw big fish
	glPushMatrix();					
	glTranslatef(x_offset,0.0f,-175.0f);
	glRotatef(flip_bigFish,0,1,0);
	glCallList(bigFish);								
	glPopMatrix();

	// glPushMatrix();					
	// make_bigFishBox();
	// glPopMatrix();

	// draw small fish 
	glPushMatrix();					
	glTranslatef(0.0f,y_offset-35.0f,-175.0f);
	glRotatef(flip_smallFish,0,1,0);
	glCallList(smallFish);								
	glPopMatrix();

	// glPushMatrix();					
	// make_smallFishBox();
	// glPopMatrix();

	// enable lighting for tank fan
	glPushMatrix();
	glLoadIdentity();
	enable_lightAmb();
	glPopMatrix();

	// draw tank fan
	glPushMatrix();					
	glTranslatef(0.0f,0.0f,-300.0f);
	glRotatef(rotate,0,0,1);
	glCallList(tankFan);								
	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();
	disable_lightAmb();
	glPopMatrix();

	glutSwapBuffers();
}

/*===========================================================
camera_angle

Define camera position and orientation for perspective projection
Default is defined location at (200,0,500) with center at origin 

INPUT
unisghed char angle: used to define different camera angles 
===========================================================*/
void camera_angle(unsigned char angle)
{
	switch (angle)
	{
	
	case 'r': // right view
		gluLookAt(500.0, 0.0, -175.0, 0.0, 0.0, -175.0, 0.0, 1.0, 0.0);
	break;
	case 'l': // left view
		gluLookAt(-500.0, 0.0, -175.0, 0.0, 0.0, -175.0, 0.0, 1.0, 0.0);
	break;
	case 'f': // front view
		gluLookAt(0.0, 0.0, 300.0, 0.0, 0.0, -175.0, 0.0, 1.0, 0.0);
	break;
	case 'k': // back view
		gluLookAt(0.0, 0.0, -600.0, 0.0, 0.0, -175.0, 0.0, 1.0, 0.0);
	break;
	case 't': // top view
		gluLookAt(0.0, 500.0, -175.0, 0.0, 0.0, -175.0, 0.0, 0.0, -1.0);
	break;
	case 'b': // bottom view
		gluLookAt(0.0, 500.0, -175.0, 0.0, 0.0, -175.0, 0.0, 0.0, 1.0);
	break;
	default: // default
		gluLookAt(200.0, 0.0, 500.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	break;
	}
}

/*===========================================================
init_setup

Initialize all global variables, camera, and depth buffer
===========================================================*/
void init_setup()
{
	camera_angle('0');		// Define camera angle

	glEnable(GL_DEPTH_TEST); // Enable depth buffer 
	
	rotate = 0;
	flip_bigFish = 0;
	flip_smallFish = 0;

	timer_num = 30;
	timer = std::to_string(timer_num);

	stop = false;
	still_hit = false;

	x_offset = 0.0f;
	y_offset = 0.0f;

	collide_key = '\0';
}


#define canvas_Width 800 			/* width of canvas */
#define canvas_Height 600 			/* height of canvas */
char canvas_Name[]= "Program 4"; 	/* name in window top */


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

	make_bigFish();							/* display list for big fish */
	make_smallFish();						/* display list for small fish */
	make_fishTank();						/* display list for fish tank */
	make_tankFan();							/* display list for tank fan */

	glutDisplayFunc(display_func); 			/* registers the display callback */
	glutKeyboardFunc(keyInput_func);		/* registers the keyboard callback */
	glutMouseFunc(mouseInput_func);			/* registers the mouse callback */

	glutTimerFunc(25,timer_func,1);			/* Call timer_func */
	glutTimerFunc(1000,timer_func,2);		/* Call timer_func */

	glutMainLoop(); 						/* execute until killed */
	return 0;
}