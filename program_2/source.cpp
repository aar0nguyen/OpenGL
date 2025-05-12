// Graphics Prg 2 for Aaron Nguyen

/*===========================================================
source.cpp

The program creates a scene of a snowman and a falling snowflake. 
The animation of the snowflake is achieved by utilizing a timer
event handler, timer_func, to update the offset y position variable
of the snowflake and then invokes the display handler, display_func, 
every 27 miliseconds to perform frame-by-frame animation. 

The display handler, display_func, will draw the current frame of the 
animation, which is started by the keyboard event handler, keyInput_func. 
The animation can be started with any key press event which then registers
the timer event handler, timer_func, to start the animation of the falling
snowflake.

An animation cycle is determined by the timer event handler, timer_func, to
determine if the snowflake reached the bottom on the canvas and if so, reset
the position of the snowflake to the top of the canvas and redraw 
the snowflake on the next callback of the display handler, display_func. 

The keyboard event handler, keyInput_func, also simulates the movements of
the snowman. Upon pressing key 'k' will invoke the keyboard event handler 
to move the snowman towards the left side of the canvas. Upon pressing
key 'l' will move the snowman towards the right side of the canvas. 

Upon each callback to the display handler, the display handler determines
the collision between the snowflake and the snowman. This collision detection
is determined by the funciton checkCollision() to determine if the bounding
box of the snowflake is intersecting with the bounding box of the snowman. 
Should the intersection happen, the function will register a timer event for 
1200 miliseconds and set the collision state variable 'hit' to true. 
For the duration of this timer event, the program will draw the text "OUCH, COLD!" 
in color red upon every callback of the display handler. The timer event handler, 
timer_func, will remove the drawn text after the 1200 miliseconds have elapsed. 

EXTRA
Upon each reset of the animation cycle, the color of the snowflake will
alternate between colors states Chartreuse Yellow and Chartreuse Green.
In addition, a score is recorded depending on if the collision happened 
between the snowman and the snowflake. When collision happens between
the snowflake and the snowman, the collision state variable 'wasHit' will
be set to true to indicate that collision has happened during the animation 
cycle. Before the animation cycle is reset, the score will be updated by 10 
points only if the snowman did not collide with the snowflake. Should the snowman 
collide with the snowflake during the timer event for 1200 miliseconds or during 
the animation cycle, the score will not be updated. 

===========================================================*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "OpenGL445Setup-aug24.h"	/* Include configurations settings for OpenGL and GLUT*/
#include <cmath>            		/* Needed to perform trigonometric calculations */
#include <string>					/* Needed to perform string operations to print bitmap characters */

// Global Vars
int x_pos;							/* x value to offset snowman */
int y_pos;							/* y value to offset snowflake */
bool start;							/* bool to determine start of animation */
bool flip;							/* EXTRA: bool to determine snowflake color*/

bool hit;							/* Collision state when snowman is hit by snowflake */
bool wasHit;						/* EXTRA: Collision staten when snowman is hit before animation cycle is reset */

static unsigned int startTxt;		/* Display List to print animation start */
static unsigned int ctrlTxt;		/* Display List to print controls for snowman */
static unsigned int hitTxt;			/* Display List to print collition detection */

int score;							/* EXTRA: integer to store score */
std::string scoreStr;				/* EXTRA: string to form text to print using string operations*/
static unsigned int scoreTxt;		/* EXTRA: Display List to print score */

static unsigned int snowman;                /* Display List to draw snowman*/
static unsigned int list[3] = {0, 1, 2};    /* Array of list offsets for snowman display list*/

// Snowman Bounding Box Definition
struct snowmanboundbox {
	int left = 210;	
	int right = 330;	
	int top = 301;
	int bottom = 1;
} manbox;

// Snowflake Bounding Box Definition
struct snowflakeboundbox {
	int left = 75;
	int right = 125;
	int top = 600;
	int bottom = 550;
} flakebox;

/*===========================================================
updateflakePt

Update top and bottom of snowflake bounding box

INPUT
snowflakeboundbox ptr: reference to bounding box definition 
to update 

===========================================================*/
void updateflakePt(snowflakeboundbox& ptr)
{
	ptr.top = 600 + y_pos;
	ptr.bottom = 550 + y_pos;
}

/*===========================================================
bitmapOutputSetup

Set up bitmap display list to output text

INPUT
string str: message to process and draw onto canvas

OUTPUT
int text: stores the base value of display list to be called

===========================================================*/
int bitmapOutputSetup(std::string str)
{
	unsigned int text = glGenLists(1);
	glNewList(text,GL_COMPILE);
		int len = str.length();
		for (int i = 0;i < len; i++) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
	glEndList();

	return text;
}

/*===========================================================
EXTRA: flipColor

Set glColor state to alternate between 2 colors on every
animation cycle

INPUT
bool state: determine what glColor to set

===========================================================*/
void flipColor(bool state)
{
	if (state) glColor3f(127.0/255.0,1.0,0.0); 		/* Chartreuse Green */
	else glColor3f(233.0/255.0,1.0,0.0); 			/* Chartreuse Yellow */
}

/*===========================================================
EXTRA: updateScore

Update score when snowman was not hit by snowflake or was not 
hiy during the animation cycle

INPUT
bool wasHit: state of collision of snowman 

===========================================================*/
void updateScore(bool wasHit)
{
	if (wasHit) return;		/* Do not update when snowman was hit */
	 
	score += 10;
	scoreStr = "score = " + std::to_string(score);	/* Set score string message to be printed onto canvas*/
}


/*===========================================================
drawSnowmanBoxSetup

Set up display list to draw snowman with center position
of bottom edge at (270, 1, 0)

===========================================================*/
void drawSnowmanBoxSetup()
{
	snowman = glGenLists(3);

	// Bottom Box
	glNewList(snowman, GL_COMPILE);
        glBegin(GL_LINE_LOOP);
            glVertex3f(210.0, 1.0, 0.0);
            glVertex3f(330.0, 1.0, 0.0);
            glVertex3f(330.0, 121.0, 0.0);
            glVertex3f(210.0, 121.0, 0.0);
        glEnd();
	glEndList();

	// Middle Box
	glNewList(snowman + 1, GL_COMPILE);
        glBegin(GL_LINE_LOOP);
            glVertex3f(220.0, 121.0, 0.0);
            glVertex3f(320.0, 121.0, 0.0);
            glVertex3f(320.0, 221.0, 0.0);
            glVertex3f(220.0, 221.0, 0.0);
        glEnd();
	glEndList();

	// Top Box
	glNewList(snowman + 2, GL_COMPILE);
        glBegin(GL_LINE_LOOP);
            glVertex3f(230.0, 221.0, 0.0);
            glVertex3f(310.0, 221.0, 0.0);
            glVertex3f(310.0, 301.0, 0.0);
            glVertex3f(230.0, 301.0, 0.0);
        glEnd();
	glEndList();
}

/*===========================================================
drawSnowflake

Draw snowflake onto canvas with center position at (100,575,0)

===========================================================*/
void drawSnowflake()
{	
    glBegin(GL_LINES);
        glVertex3i(75.0, 575.0 + y_pos, 0.0);
        glVertex3i(125.0, 575.0 + y_pos, 0.0);
		
		glVertex3i(100.0, 600.0 + y_pos, 0.0);
        glVertex3i(100.0, 550.0 + y_pos, 0.0);

		glVertex3i(100 - 25*sqrt(2)/2, 575 + 25*sqrt(2)/2 + y_pos, 0.0);
        glVertex3i(100 + 25*sqrt(2)/2, 575 - 25*sqrt(2)/2 + y_pos, 0.0);

		glVertex3i(100 + 25*sqrt(2)/2, 575 + 25*sqrt(2)/2 + y_pos, 0.0);
        glVertex3i(100 - 25*sqrt(2)/2, 575 - 25*sqrt(2)/2 + y_pos, 0.0);
    glEnd();
}


/*===========================================================
timer_func

Timer event handler to perform snowflake animation and reset 
collision state when snowman is hit by snowflake

INPUT
int val: ID of timer event

===========================================================*/
void timer_func(int val)
{
	// Animation Timer Event Handler
	if (val == 1)
	{
		y_pos -= 4;							/* Offset snowflake position */
		updateflakePt(flakebox);			/* Update bound box of snowflake */
		glutPostRedisplay();				/* Redraw canvas */
		glutTimerFunc(27,timer_func,1);		/* Continue animation */

		if ((600 - 50 + y_pos) <= 4) 		/* Check if snowflake drawn at end of canvas */
		{
			y_pos = 0;						/* Reset y offset */
			updateflakePt(flakebox);		/* Reset bound box for snowflake*/
			flip = !flip;					/* EXTRA: Change snowflake color*/
			updateScore(hit || wasHit);		/* EXTRA: Update score only if snowman was not hit durring collision or was hit during animation cycle */
			wasHit = false;					/* EXTRA: Reset collision state upon reset of animation cycle */
		}	
	}

	// Collision Timer Event Handler
	if (val == 2)
	{
		hit = false;						/* Reset collision state */
		glutPostRedisplay();				/* Redraw canvas */
	}

}

/*===========================================================
checkCollision

Check collision state of snowman and snowflake upon the 
current frame of the animation. If snowman was hit, the 
timer event handler will trigger for 1.2 seconds. 

===========================================================*/
void checkCollision()
{
	if (wasHit) return;		/* Do not trigger if snowman is already hit */

	// check if snowflake and snowman right or left side intersect
	if (flakebox.left >= manbox.right || flakebox.right >= manbox.left)
	{
		// check if snowflake and snowman top or bottom side intrsect
		if (flakebox.bottom <= manbox.top || flakebox.top <= manbox.bottom)
		{
			glutTimerFunc(1200,timer_func,2);	/* Call timer event handler to handle collision detection */
			hit = true;							/* Set collision state to true */
			wasHit = true;						/* Set collision state to true for animation cycle */
		}
	}
}


/*===========================================================
keyInput_func

Keyboard event handler to register the timer event handler when 
the animation was not started and to perform movement controls
for the snowman.

INPUT
unsigned char key: store character that key was pressed
int x: store x position of cursor at time of key press
int y: store y position of cursor at time of key press

===========================================================*/
void keyInput_func(unsigned char key, int x, int y)
{
	// Animation Start 
	if (!start) 
	{
		start = true;
		glutTimerFunc(27,timer_func,1);		/* Register timer_func to start animation cycle */
	}

	// Snowman Movement 
	switch (key)
	{
	// Move snowman to left
	case 'k':
		if (x_pos <= -207) 		/* Stop snowman movement at end of left side of canvas */
		{
			x_pos = -207;		/* Limit x offset for snowman */
			manbox.left = 0;	/* Limit position left side of bounding box of snowman */
			manbox.right = 120;	/* Limit position right side of bounding box of snowman */
		}
		else 
		{
			x_pos -= 3;			/* Update x offset of snowman */
			manbox.left -= 3;	/* Update position of left side of bounding box of snowman */
			manbox.right -= 3;	/* Ipdate position of right ide of bounding box of snowman */
		}

		break;

	// Move snowman to right
	case 'l':
		if (x_pos >= 270) 		/* Stop snowman movement at end of right side of canvas*/
		{
			x_pos = 270;		/* Limit x offset for snowman */
			manbox.left = 480;	/* Limit position of left side of bounding box of snowman */
			manbox.right = 600;	/* limit position of right side of bounding box of snowman */
		}
		else 
		{
			x_pos += 3;			/* Update x offset of snowman */
			manbox.left += 3;	/* Update position of left side of bounding box of snowman */
			manbox.right += 3;	/* Update position of right side of bounding box of snowman */
		}
		break;
	
	default:	/* Register nothing when other keys are pressed */
		break;
	}

}

/*===========================================================
display_func

Display handler to draw string messages, snowman, and snowflake 
and draw the animation cycle frame-by-frame. 

===========================================================*/
void display_func()
{
	
	glClearColor(0.00764, 0.0, 0.2078, 1.0);	/* Set background to dark blue */
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw before start of animation cycle
	if (!start) 
	{
		glRasterPos3f(210.0, 575.0, 0.0);                
		glCallList(startTxt);				/* Call display list to draw start message*/
		glRasterPos3f(180.0, 555.0, 0.0);             
		glCallList(ctrlTxt);				/* Call display list to draw messge for control of snowman movement */
	}

	// Draw score
	glRasterPos3f(500.0, 575.0, 0.0);            
	scoreTxt = bitmapOutputSetup(scoreStr);		/* EXTRA: Set integer score text to base value of display list */
	glCallList(scoreTxt);						/* EXTRA: Call display list scoreTxt to draw messgae of score */
	glDeleteLists(scoreTxt,1);					/* EXTRA: Deallocate display list scoreTxt to preserve memory */
	
	// Draw snowman
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();					/* Push current position matrix to stack*/
	glTranslatef(x_pos, 0.0, 0.0);	/* Move snowman based on x position offset */
	glListBase(snowman);			/* Specify array of offset for display list */
	glCallLists(3, GL_INT, list);	/* Call display lists to draw snowman */
	glPopMatrix();					/* Pop position matrix from stack */
	
	// Collision Detection
	checkCollision();
	if (hit)
	{
		glColor3f(1.0, 0.0, 0.0);					/* Set message color to red */
		glRasterPos3f(228.0 + x_pos, 165.5, 0.0); 
		glCallList(hitTxt);							/* Call display list to print collition message*/
		glColor3f(1.0, 1.0, 1.0);					/* Reset color state */
	}

	// Draw Snowflake
	flipColor(flip);			/* EXTRA: Set color state depending on animation cycle */
	drawSnowflake();			/* Cal function to draw snowflake */
	glColor3f(1.0, 1.0, 1.0);

	glFlush();						/* Clear Buffer */
}


#define canvas_Width 600 			/* width of canvas */
#define canvas_Height 600 			/* height of canvas */
char canvas_Name[]= "Program 2"; 	/* name in window top */


/*===========================================================
main 

Main set initial values of global variables and the setup of 
the OpenGL canvas. Main also registers the display handler,
display_func, and the keyboard hander, key_input_func. 

===========================================================*/
int main(int argc, char ** argv)
{

	glutInit(&argc, argv);
	my_setup(canvas_Width, canvas_Height, canvas_Name);

	x_pos = 0;							/* Init x position offset for snowman */
	y_pos = 0;							/* Init y position offset for snowflake */
	score = 0;							/* EXTRA: Init score */
	flip = false;						/* EXTRA: Init state for snowflake color */
	start = false;						/* Init boolean start animation cycle */
	
	hit = false;						/* Init bool for collision state of snowman */
	wasHit = false;						/* Init bool for collision state during animation cycle */

	scoreStr = "score = " + std::to_string(score);	/* Init string message for score */

	drawSnowmanBoxSetup();											/* Setup display list for snowman */
	startTxt = bitmapOutputSetup("Press any key to start");			/* Setup display list for animation start message*/
	ctrlTxt = bitmapOutputSetup("Press K and L to move snowman");	/* Setup display list for snowman movement control message*/
	hitTxt = bitmapOutputSetup("OUCH, COLD!");						/* Setup display list for collision detection message*/

	glutDisplayFunc(display_func); 			/* Registers the display callback */
	glutKeyboardFunc(keyInput_func);		/* Registers the keyboard callback */
	glutMainLoop(); 						/* execute until killed */
	return 0;
}