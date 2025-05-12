// Graphics Prg 3 for Aaron Nguyen

/*===========================================================
source.cpp

The program creates a game of a falling spaceship. The animation of the spaceship 
is achieved by utilizint a timer event handler, timer_func. Within timer_func, the 
y position of the spaceship is calcualted by the kinematic equation y = 0.5*g*t^2,
where g equals the gravity constant and t equals time. timer_func invokes the display
handler, display_func, every 66.667 ms to achieve 15 frames per second animation. 

display_func draws the current frame of the animation. The keyboard event handler,
keyInput_func, can start the animation with key press of 'I' or 'V', which invokes
timer event handlers to start animation of the spaceship. display_func and keyInput_func
are registered first inside main. 

An animation cycle is determined by 2 timer event handlers to achieve simulation of
falling object using different gravity constants. A timer event handler with an ID of 1
determines the position and collision of the falling spaceship and if collided, stop movement
of the spaceship dependent on collision of spaceship. A timer event handler with an ID of 2
is used to track the "time of fall" of the spaceship every 1 ms which is used by the first
timer event handler for gravity-based calclations using equation y = 0.5*g*t^2. 

keyInput_func also simulates movement of the falling spaceship. Upon pressing 'H' will
invoke keyInput_func to move the spaceship towards the left of the canvas by 4 units. 
Upon pressing 'J'  move the spaceship towards the right of the canvas by 4 units.
Upon pressing 'U'  move the spaceship towards the up of the canvas by 5 units.
All movement will not effect the gravity-based calclations and will continue to fall 
without affecting the velocity or acceleration of the spaceship.

Each callback of the timer event handler with ID of 1 determines the collision of the spaceship. 
The collision detection is determined by 2 void functions, checkCollision_X and checkCollision_Y. 
These two functions handles the collision between the spaceship and landing zone. To similate 
collision detection of "diamond" shape object, checkCollision_X used positional data of the left 
and right side of the spaceship and landing zone by taking the difference between the left side 
of the spaceship and the right side of the zone if the spaceship entered the zone from the right 
side along the x axis, or the difference between the right side of the spaceship and the left side 
of the zone if the spaceship entered from the left side of the zone along the x-axis. This difference 
was used to resize the bounding box of the spaceship to determine and set the bounding box to the 
closest point of contact with respect to the edges of the zone. checkCollision_Y determines the 
collision detection of spaceship and zone along the y-axis. 

Should the bounding box of both the spaceship and landing zone intersect, timer_func will set 
animation state variable to stop movement and gravity-based calculations by stoping the register 
of the timer event handler with ID of 2 and translate the position of the spaceship to the final 
position on the langind zone. Should the bounding box of the spaceship intersect with the red line, 
timer_func will also stop movement and gravity-based calcualtions by stoping the register of the 
timer event handler and lock the movement of the spaceship at the current position.  
display_func will draw the spaceship at the final position on the landing zone and text using 
bitmap representation but will continue to register the timer event handler with ID of 1. 

EXTRA
Upon each callback of the timer_func with ID of 1, a rotation offset is calcualted to rotate the 
spaceship on the y-axis throughout the animation of the falling spaceship. The spaceship will continue
to rotate even after the collision of the landing zone or the red line. Upon key press of 'U' will also 
draw the Alabama Flag on the and set a timer_fuc with ID of 4 for 250 ms to disable the flag after 0.25
seconds elapses. Upon key press of 'Space' will lock the y positon of the spaceship to simulate "stop gravity",
where the spaceship is free to move along the x-axis without falling. A timer_func with an ID of 3 is registered
for 1000 ms to disable the lock and re-register the timer_func with ID of 2 to calculate time. 

===========================================================*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup-aug24.h"	/* Include configurations settings for OpenGL and GLUT*/
#include <cmath>            		/* Needed to perform trigonometric calculations */
#include <string>					/* Needed to perform string operations to print bitmap characters */

bool start; 			/* start animation state */
bool end;				/* end animation state */
bool stop;				/* EXTRA: stop gravity state */
bool flag_draw;			/* EXTRA: draw alabama flag state */

bool x_Collision;		/* x collision state */
bool y_Collision;		/* y collision state */
int diff;				/* diff between edge of spaceship and landing zone */

float grav;				/* value of gravity constant */
float t_timer;			/* value of timer for kinematic equation calcualtion */

int fuel;				/* value of score */
std::string fuel_str;	/* string to store value of score */

std::string status;		/* status of game ending */

float x_pos;			/* init position of spaceship on x axis */
float y_pos;			/* init position of spaceship on y axis */

float rotate_Offset;	/* EXTRA: value of rotation offset */
float y_Offset; 		/* value of y positoin offset */

// spaceship bounding box definition
struct shipBoundingBox {
	int left = 400;	
	int right = 401;	
	int top = 568;
	int bottom = 532;
} shipBox;

// landing zone bounding box definition 
struct zoneBoundingBox {
	int left = 40;	
	int right = 80;	
	int top = 17;
	int bottom = 7;
} zoneBox;

/*===========================================================
init_var()

Update initial global variables 
===========================================================*/
void init_var()
{
	start = false;
	end = false;
	stop = false;
	flag_draw = false;

	x_Collision = false;
	y_Collision = false;
	diff = 0;

	grav = 0;

	status = "";

	fuel = 200;
	fuel_str = "Fuel: " + std::to_string(fuel);

	rotate_Offset = 0;
	y_Offset = 0;

	x_pos = 400.0f;
	y_pos = 565.0f;	

	t_timer = 0;

}

/*===========================================================
bitmap_Output()

Set up bitmap to output text

INPUT
string str: message to process and draw onto canvas
===========================================================*/
void bitmap_Output(std::string str)
{
	int len = str.length();
	for (int i = 0;i < len; i++) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);	
}

/*===========================================================
make_zoneBox()

Helper function to output bounding box of landing zone onto canvas
for testing collision
===========================================================*/
void make_zoneBox()
{
	glBegin(GL_LINE_LOOP);
		glVertex3i(zoneBox.left,zoneBox.top,-50);
		glVertex3i(zoneBox.left,zoneBox.bottom,-50);
		glVertex3i(zoneBox.right,zoneBox.bottom,-50);
		glVertex3i(zoneBox.right,zoneBox.top,-50);
	glEnd();
}

/*===========================================================
make_shipBox()

Helper function to output bounding box of spaceship onto canvas
for testing collision
===========================================================*/
void make_shipBox()
{
	glBegin(GL_LINE_LOOP);
		glVertex3i(shipBox.left,shipBox.top,0);
		glVertex3i(shipBox.left,shipBox.bottom,0);
		glVertex3i(shipBox.right,shipBox.bottom,0);
		glVertex3i(shipBox.right,shipBox.top,0);
	glEnd();
}

/*===========================================================
make_Square()

Helper function to create square for spaceship. An octahedron
is simply 3 squares, where 1 square is flat on x,y,z plane 
===========================================================*/
void make_Square()
{
	glBegin(GL_LINE_LOOP);
		glVertex3f(-12.5f,-12.5f,0.0f);
		glVertex3f(-12.5f,12.5f,0.0f);
		glVertex3f(12.5f,12.5f,0.0f);
		glVertex3f(12.5f,-12.5f,0.0f);
	glEnd();
}

/*===========================================================
make_SpaceShip()

Helper function to create spaceship using display list. Utilize
rotation to make each square flat on each place
===========================================================*/
static unsigned int ship;                /* display list to draw sapceship*/
void make_SpaceShip()
{
	ship = glGenLists(1);
	glNewList(ship,GL_COMPILE);
	
	glColor3f(0.0f,0.4667f,0.7843f);	/* set color to UAH blue */
	
	// diamond flat on x,y place 
	glPushMatrix();
	glRotatef(45.0f,0.0f,0.0f,1.0f);
	make_Square();
	glPopMatrix();

	// diamond flat on x,z place 
	glPushMatrix();
	glRotatef(90.0f,1.0f,0.0f,0.0f);
	glRotatef(45.0f,0.0f,0.0f,1.0f);
	make_Square();
	glPopMatrix();

	// diamond flat on y,z place 
	glPushMatrix();
	glRotatef(90.0f,0.0f,1.0f,0.0f);
	glRotatef(45.0f,0.0f,0.0f,1.0f);
	make_Square();
	glPopMatrix();

	glEndList();
}

/*===========================================================
make_LandZone()

Helper function to create landing zone using display list.
Landing zone is in shape of inverted chevron.
===========================================================*/
static unsigned int zone;                /* display list to draw landing zone */
void make_LandZone()
{
	zone = glGenLists(1);
	glNewList(zone,GL_COMPILE);
	
	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_LINE_LOOP);
		glVertex3f(40.0f,7.0f,-50.0f);
		glVertex3f(40.0f,17.0f,-50.0f);
		glVertex3f(60.0f,7.0f,-50.0f);
		glVertex3f(80.0f,17.0f,-50.0f);
		glVertex3f(80.0f,7.0f,-50.0f);
	glEnd();

	glEndList();
}

/*===========================================================
make_RedLine()

Helper function to create red line using display list
===========================================================*/
static unsigned int line;                /* Display list to draw red line */
void make_RedLine()
{
	line = glGenLists(1);
	glNewList(line,GL_COMPILE);

	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,7.0f,-50.0f);
		glVertex3f(800.0f,7.0f,-50.0f);
	glEnd();
	glEndList();
}

/*===========================================================
EXTRA
make_Flag()

Helper function to create flag using display list
===========================================================*/
static unsigned int flag;                /* display list to draw flab */
void make_Flag()
{
	flag = glGenLists(1);
	glNewList(flag,GL_COMPILE);

	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,0.0f,-50.0f);
		glVertex3f(15.0f,10.0f,-50.0f);

		glVertex3f(0.0f,10.0f,-50.0f);
		glVertex3f(15.0f,0.0f,-50.0f);
	glEnd();


	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_LINE_LOOP);
		glVertex3f(0.0f,0.0f,-50.0f);
		glVertex3f(0.0f,10.0f,-50.0f);
		glVertex3f(15.0f,10.0f,-50.0f);
		glVertex3f(15.0f,0.0f,-50.0f);
	glEnd();
	
	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0f,10.0f,-50.0f);
		glVertex3f(0.0f,-10.0f,-50.0f);
	glEnd();

	glEndList();
}

/*===========================================================
update_shipBox_X()

Helper function to update left and right bound box of spaceship
===========================================================*/
void update_shipBox_X(shipBoundingBox& ptr, int left, int right)
{
	ptr.left = left;
	ptr.right = right;
}

/*===========================================================
update_shipBox_X()

Helper function to update top and bottom bound box of spaceship
===========================================================*/
void update_shipBox_Y(shipBoundingBox& ptr, int top, int bottom)
{
	ptr.top = top;
	ptr.bottom = bottom;
}

/*===========================================================
checkCollision_X()

Check collision state of spaceship and landing zone along x-axis.
If x-axis collision is true, resize bound box of spaceship and set
x collision state to true. 
===========================================================*/
void checkCollision_X()
{
	if (x_pos - 18 <= zoneBox.right && x_pos + 18 >= zoneBox.left)
	{
		x_Collision = true;

		if (x_pos - 18 <= zoneBox.right && x_pos > zoneBox.right) 
		{
			diff = abs(x_pos - 18 - zoneBox.right);
			update_shipBox_X(shipBox,zoneBox.right, x_pos + 18 - diff);
			update_shipBox_Y(shipBox,y_pos + y_Offset + diff, y_pos + y_Offset - diff);
		}
		else if (x_pos + 18 >= zoneBox.left && x_pos < zoneBox.left)
		{
			diff = abs(x_pos + 18 - zoneBox.left);
			update_shipBox_X(shipBox,x_pos - 18 + diff, zoneBox.left);
			update_shipBox_Y(shipBox,y_pos + y_Offset - diff, y_pos + y_Offset + diff);
		}
		else
		{
			update_shipBox_X(shipBox, x_pos, x_pos + 1);
			update_shipBox_Y(shipBox,y_pos + y_Offset - 18, y_pos + y_Offset + 18);
		}
	}
	else
	{
		x_Collision = false;
		update_shipBox_X(shipBox, x_pos, x_pos + 1);
		update_shipBox_Y(shipBox,y_pos + y_Offset + 18, y_pos + y_Offset - 18);
	}
}

/*===========================================================
checkCollision_Y()

Check collision state of spaceship and landing zone along y-axis.
If y-axis collision is true, set y collision state to true. 
===========================================================*/
void checkCollision_Y()
{
	if (y_pos + y_Offset - 18 <= zoneBox.top && y_pos + y_Offset + 18 >= zoneBox.bottom) y_Collision = true;
	else y_Collision = false;
}

/*===========================================================
timer_func

Timer event handler to perform falling spaceship animation, perform
gravity-based calculations, and determine collision state of spaceship and
landing zone. 

INPUT
int val: ID of timer event
===========================================================*/
void timer_func(int val)
{
	// animation and collision timer event handler 
	if (val == 1)
	{
		rotate_Offset += 9.667;		/* EXTRA: rotation offset of spaceship */

		if (start)
		{
			y_Offset = 0.5*grav*pow(t_timer,2.0);		/* y Offset based from gravity */

			checkCollision_X();		/* determine x collision state */
			checkCollision_Y();		/* determine y collision state */
			
			if (x_Collision && y_Collision)		/* if spaceship collide with landing zone */
			{
				status = "YOU WIN";
				start = false;
				end = true;			/* stop animation */
				y_Offset = 0;		/* stop movement based on gravity */
				grav = 0;			/* ensure movement stop based on gravity */
				x_pos = 60.0f;		/* set x final position of spaceship */
				y_pos = 24.677f;	/* set y final position of spaceship */
			}
			else if (y_pos + y_Offset - 18 <= 7 && !(x_Collision && y_Collision))	/* if spaceship collide with red line*/
			{
				status = "YOU LOSE";
				start = false;
				end = true;			/* stop animation */
				y_Offset = 0;		/* stop movement based on gravity */
				grav = 0;			/* ensure movement stop based on gravity */
				x_pos = x_pos;		/* set x final position of spaceship */
				y_pos = 24.677f;	/* set y final position of spaceship */
			}
			
			glutPostRedisplay();						/* redraw canvas */
			glutTimerFunc(66.667,timer_func,1);			/* register timer_func to draw animation */


		}
		else if ((x_Collision && y_Collision) || end) 	/* if animation ended */
		{
			glutPostRedisplay();						/* redraw canvas */
			glutTimerFunc(66.667,timer_func,1);			/* register timer_func to draw animation to continue rotation*/
		}
		
	}

	// time calcualtions timer event
	if (val == 2 && start)
	{
		if (!stop || end)	/* EXTRA: stop counting time if space was pressed or end of animation */
		{
			t_timer += 0.001;				/* increment by 1 ms, 1000ms = 1 sec */
			glutTimerFunc(1,timer_func,2);	/* register timer_func to count time */
		}
	}

	// EXTRA: stop gravity timer event handler 
	if (val == 3 && start)
	{
		stop = false;					/* after 1 sec, set stop state to false */
		glutTimerFunc(1,timer_func,2);	/* re-register timer to start time calcualtions for gravity */
	}

	// EXTRA: flag draw timer event handler
	if (val == 4 && start)
	{
		flag_draw = false;		/* after 0.25 sec, set flag draw to false to stop drawing flag */
	}

}

/*===========================================================
keyInput_func

Keyboard event handler to register the timer event handler when 
the animation was not started and to perform movement controls
for the spaceship.

INPUT
unsigned char key: store character that key was pressed
int x: store x position of cursor at time of key press
int y: store y position of cursor at time of key press
===========================================================*/
void keyInput_func(unsigned char key, int x, int y)
{
	switch(key)
	{
		// start animation with Venus' gravity  
		case 'v':
			if (!start && !end) 
			{
				grav = -29;								/* gravity constant of Venus */
				start = true;							/* start animation */
				glutTimerFunc(66.667,timer_func,1);		/* register timer_func to start animation */
				glutTimerFunc(1,timer_func,2);			/* register timer_func to start timer calcualtions */
			}		
		break;

		// start animation with Jovian moon Io's gravity 
		case 'i':
			if (!start && !end) 
			{
				grav = -5.9;							/* gravity constant of Jovian moon Io */
				start = true;							/* start animation */
				glutTimerFunc(66.667,timer_func,1);		/* register timer_func to start animation */
				glutTimerFunc(1,timer_func,2);			/* register timer_func to start timer calcualtions */

			}
		break;
		
		// move spaceship to left side of canvas
		case 'h':
			if (end) break;
			if (x_pos <= 16) x_pos -= 0;		/* Stop sapceship movement at end of left side of canvas */
			else 
			{
				x_pos -= 4;			/* Update x offset of sapceship */

				// update boud box of spaceship
				if (!x_Collision) update_shipBox_X(shipBox, shipBox.left - 4, shipBox.right - 4);
			}

		break;

		// move spaceship to right side of canvas
		case 'j':
			if (end) break;
			if (x_pos >= 784) x_pos += 0;		/* stop sapceship movement at end of right side of canvas */
			else 
			{
				x_pos += 4;						/* update x offset of sapceship */

				// update bound box of spaceship 
				if (!x_Collision) update_shipBox_X(shipBox, shipBox.left + 4, shipBox.right + 4);
			}
		break;

		// move spaceship to up side of canvas 
		case 'u':
			if (end) break; 
			flag_draw = true;					/* EXTRA: set flag draw state to true */
			glutTimerFunc(250,timer_func,4);	/* EXTRA: set timer_func to stop draw at end of 250 sec */
			if (y_pos + y_Offset >= 580) 		/* stop sapceship movement at end of top side of canvas */
			{
				y_pos += 0;						/* update y offset of sapceship */
				if (fuel > 0) fuel -= 5;		/* update score of game, cannot go below 0 */			
			}
			else 
			{
				y_pos += 5;						/* update x offset of sapceship */
				if (fuel > 0) fuel -= 5;		/* update score of game, cannot go below 0 */

				// update bound box of spaceship 
				if (!y_Collision) update_shipBox_Y(shipBox,shipBox.top + 5, shipBox.bottom + 5);
			}
		break;
		
		// EXTRA: stop gravity 
		case 32:	/* space bar */
			if (start) 
			{
				stop = true;						/* set stop state to true to stop gravity */
				glutTimerFunc(1000,timer_func,3);	/* register timer_func to end stop state to continue gravity animation */
			}
			
		break;

		// reset game, used for testing 
		case 'r':	
		
			if (end)
			{
				init_var();				/* re-initialize global var */
				glutPostRedisplay();	/* redraw canvas */
			}
			
		break;

		default:
		break;
	}

}

/*===========================================================
display_func

Display handler to draw string messages, spaceship, landing zone,
and draw the animation cycle frame-by-frame. 
===========================================================*/
void display_func()
{
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	/* Set background to dark blue */
	glClear(GL_COLOR_BUFFER_BIT);

	// draw controls before animation start
	if (!start && !end)
	{
		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(10.0f, 575.0f, -50.0f);                
		bitmap_Output("Press H to move left");

		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(10.0f, 560.0f, -50.0f);                
		bitmap_Output("Press J to move right");

		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(10.0f, 545.0f, -50.0f);                
		bitmap_Output("Press U to move up");

		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(10.0f, 530.0f, -50.0f);                
		bitmap_Output("Press Space to stop gravity");

		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(10.0f, 515.0f, -50.0f);                
		bitmap_Output("Press I for Jovian moon Io's gravity ");

		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(10.0f, 500.0f, -50.0f);                
		bitmap_Output("Press V for Venus' gravity");
	}
	else if (end)	/* end of animation */
	{
		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(377.0f, 300.0f, -50.0f);                
		bitmap_Output(status);

		glColor3f(0.0f,0.0f,0.0f);
		glRasterPos3f(335.0f, 285.0f, -50.0f);                
		bitmap_Output("Press R to restart");
	}

	// draw red line and landing zone 
	glCallList(line);
	glCallList(zone);
	// glColor3f(0.0f,0.0f,0.0f);
	// make_zoneBox();

	// draw score 
	glColor3f(0.0f,0.0f,0.0f);
	glRasterPos3f(720.0f, 575.0f, -50.0f);  
	fuel_str = "Fuel: " + std::to_string(fuel);              
	bitmap_Output(fuel_str);

	// draw spaceship 
	glPushMatrix();					
	glTranslatef(x_pos,y_pos + y_Offset,-50.0f);
	glRotatef(15.0, 1.0, 0.0, 0.0);  				/* init x roatation to show 12 line segments*/
	glRotatef(15 + rotate_Offset, 0.0, 1.0, 0.0);  	/* EXTRA: rotate based on offset */
	glCallList(ship);								
	glPopMatrix();

	// glColor3f(0.0f,0.0f,0.0f);
	// make_shipBox();

	// draw flag
	if (flag_draw)
	{
		glPushMatrix();
		glTranslatef(x_pos,y_pos + y_Offset + 28,-50.0f);
		glCallList(flag);
		glPopMatrix();
	}

	glFlush();
}


#define canvas_Width 800 			/* width of canvas */
#define canvas_Height 600 			/* height of canvas */
char canvas_Name[]= "Program 3"; 	/* name in window top */


/*===========================================================
main 

Main set initial values of global variables and the setup of 
the OpenGL canvas. Main also registers the display handler,
display_func, and the keyboard hander, key_input_func. Main
also set up display list for objects.  
===========================================================*/
int main(int argc, char ** argv)
{

	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);

	init_var();

	make_SpaceShip();
	make_LandZone();
	make_RedLine();
	make_Flag();


	glutDisplayFunc(display_func); 			/* registers the display callback */
	glutKeyboardFunc(keyInput_func);		/* registers the keyboard callback */
	glutMainLoop(); 						/* execute until killed */
	return 0;
}