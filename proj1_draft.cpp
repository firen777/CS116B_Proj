/*Un Hou CHan*/
/*Reference: http://jamie-wong.com/2014/08/19/metaballs-and-marching-squares/ */
//=======Include=======//
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <cstdio>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif
//=======Constant=======//
#define X_RESOLUTION 800 
#define Y_RESOLUTION 600 
#define UNDEFINED -1
#define NORTH 0
#define NORTHEAST 1
#define EAST 2
#define SOUTHEAST 3
#define SOUTH 4
#define SOUTHWEST 5
#define WEST 6
#define NORTHWEST 7
#define TRUE 1
#define FALSE 0

#define SQUARE_X 80
#define SQUARE_Y 60
//=======Struct=======//
/**int r,g,b
*/
typedef struct ball_color
{
  int r;
  int g;
  int b;
} ball_color;

/**int x,y,z; direction, see #define
*/
typedef struct vec3
{
  int x;
  int y;
  int z;
  int direction;
} vec3;

/**vec3 position, int radius, ball_color color
*/
typedef struct ball_type
{
  vec3 position;
  int radius;
  ball_color color;
} ball_type;
//=======Global Var=======//
ball_type ball1;
ball_type ball2;
ball_type ball3;
ball_type ball4;
ball_type ball5;
ball_type ball6;
ball_type ball7;
ball_type ball8;

//data matrix for marching square corners. Float value is for Linear Interpolation
float dat_mat [SQUARE_X+1][SQUARE_Y+1]; 
//global control for Linear Interpolation, Plain Circle 
bool linear_interp, plain_circle;
//=======Func Proto=======//
/**Determine whether the ball hit the wall
 * @param ball_type The ball to be checked
 * @return TRUE if hit; FALSE if not hit. NOTE: TRUE, FALSE defined as 1 and 0
*/
int ball_hit_wall (ball_type);
/**Generate a new direction with a given ball
 * @param ball_type ball to be analyzed
 * @return direction
*/
int select_ball_direction (ball_type);
/**Move the ball position by 1 on X axis or Y axis or both, depending on the ball direction
 * @param ball_type ball to be moved
 * @return new ball data.
*/
ball_type move_ball (ball_type);
/**The draw function for balls
 * @param ball_type ball to be drawn
*/
void draw_ball (ball_type ball);
/**The draw function for metaballs. Using global variables of balls
 * 
*/
void draw_meta ();
/**Calculate Marching squares corners value. Using global variables of dat_mat
 * 
*/
void calc_dat_mat();

void display (void);
void reshape (int, int);
void keyboard (unsigned char, int, int);

//=======Func Implementations=======//
int ball_hit_wall (ball_type ball)
{
  int ball_hit_wall;

  ball_hit_wall = FALSE; //default false
  switch (ball.position.direction) //Direction determine which side potentially collide
  {
    case NORTH:
      if ((ball.position.y - ball.radius) <= 0)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case NORTHEAST:
      if (((ball.position.y - ball.radius) <= 0) || ((ball.position.x + ball.radius) >= X_RESOLUTION))
      {
        ball_hit_wall = TRUE;
      }
    break;
    case EAST:
      if ((ball.position.x + ball.radius) >= X_RESOLUTION)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case SOUTHEAST:
      if (((ball.position.y + ball.radius) >= Y_RESOLUTION) || ((ball.position.x + ball.radius) >= X_RESOLUTION))
      {
        ball_hit_wall = TRUE;
      }
    break;
    case SOUTH:
      if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case SOUTHWEST:
      if (((ball.position.y + ball.radius) >= Y_RESOLUTION) || ((ball.position.x - ball.radius) <= 0))
      {
        ball_hit_wall = TRUE;
      }
    break;
    case WEST:
      if ((ball.position.x - ball.radius) <= 0)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case NORTHWEST:
      if (((ball.position.y - ball.radius) <= 0) || ((ball.position.x - ball.radius) <= 0))
      {
        ball_hit_wall = TRUE;
      }
    break;
    default:
    break;
  }
  return ball_hit_wall;
}
int select_ball_direction (ball_type ball)
{
  int new_ball_direction, random_direction;
  float new_move_north, new_move_northeast, new_move_east, new_move_southeast, new_move_south, new_move_southwest, new_move_west, new_move_northwest;
  float random_number, lower_bound, upper_bound;

  switch (ball.position.direction)
  {
    case NORTH:
      if (((ball.position.x - ball.radius) <= 0) && ((ball.position.y - ball.radius) <= 0)) //collide left && collide top
      {
        new_ball_direction = SOUTHEAST;
      }
      else
      {
        if (((ball.position.x + ball.radius) >= X_RESOLUTION) && ((ball.position.y - ball.radius) <= 0)) //collide right && collide top
        {
          new_ball_direction = SOUTHWEST;
        }
        else //RNG to determine SW or SE
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = SOUTHEAST;
            break;
            case 1:
              new_ball_direction = SOUTHWEST;
            break;
            default:
              new_ball_direction = SOUTHEAST;
            break;
          }
        }
      }     
    break;
    case NORTHEAST:
      if ((ball.position.x + ball.radius) >= X_RESOLUTION) //collide right
      {
        if ((ball.position.y - ball.radius) <= 0) //collide top
        {
          new_ball_direction = SOUTHWEST; //top right collision
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHWEST;
            break;
            case 1:
              new_ball_direction = WEST;
            break;
            default:
              new_ball_direction = NORTHWEST;
            break;
          }
        }
      }
      else
      {
        random_number = (float) random() / (float) RAND_MAX * 2.0f;
        random_direction = (int) random_number;
        switch (random_direction)
        {
          case 0:
            new_ball_direction = SOUTHEAST;
          break;
          case 1:
            new_ball_direction = EAST;
          break;
          default:
            new_ball_direction = SOUTHEAST;
          break;
        }
      }
    break;
    case EAST:
      if ((ball.position.y - ball.radius) <= 0)
      {
        new_ball_direction = SOUTHWEST;
      }
      else
      {
        if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
        {
          new_ball_direction = NORTHWEST; 
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHWEST; 
            break;
            case 1:
              new_ball_direction = SOUTHWEST;
            break;
            default:
              new_ball_direction = NORTHWEST;
            break;
          }
        }
      }
    break;
    case SOUTHEAST:
      if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
      {
        if ((ball.position.x + ball.radius) >= X_RESOLUTION)
        {
          new_ball_direction = NORTHWEST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTH;
            break;
            case 1:
              new_ball_direction = NORTHEAST;
            break;
            case 2:
              new_ball_direction = EAST;
            break;
            default:
              new_ball_direction = NORTH;
            break;
          }
        }
      }
      else
      {
        random_number = (float) random() / (float) RAND_MAX * 2.0f;
        random_direction = (int) random_number;
        switch (random_direction)
        {
          case 0:
            new_ball_direction = SOUTH;
          break;
          case 1:
            new_ball_direction = SOUTHWEST;
          break;
          case 2:
            new_ball_direction = WEST;
          break;
          default:
            new_ball_direction = SOUTH;
          break;
        }
      }
    break;
    case SOUTH:
      if ((ball.position.x + ball.radius) >= X_RESOLUTION)
      {
        new_ball_direction = NORTHWEST;
      }
      else
      {
        new_ball_direction = NORTHEAST;
      }
    break;
    case SOUTHWEST:
      if ((ball.position.x - ball.radius) <= 0)
      { 
        if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
        {
          new_ball_direction = NORTHEAST;
        }
        else
        {
          new_ball_direction = SOUTHEAST;
        }
      }
      else
      {
        random_number = (float) random() / (float) RAND_MAX * 2.0f;
        random_direction = (int) random_number;
        switch (random_direction)
        {
          case 0:
            new_ball_direction = NORTH;
          break;
          case 1:
            new_ball_direction = NORTHWEST;
          break;
          case 2:
            new_ball_direction = WEST;
          break;
          default:
            new_ball_direction = NORTH;
          break;
        }
      }
    break;
    case WEST:
      if ((ball.position.y - ball.radius) <= 0)
      {
        new_ball_direction = SOUTHEAST;
      }
      else
      {
        if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
        {
          new_ball_direction = NORTHEAST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHEAST;
            break;
            case 1:
              new_ball_direction = SOUTHEAST;
            break;
            default:
              new_ball_direction = NORTHEAST;
            break;
          }
        }
      }
    break;
    case NORTHWEST:
      if ((ball.position.x - ball.radius) <= 0)
      { 
        if ((ball.position.y - ball.radius) <= 0)
        {
          new_ball_direction = SOUTHEAST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHEAST;
            break;
            case 1:
              new_ball_direction = EAST;
            break;
            default:
              new_ball_direction = NORTHEAST;
            break;
          }
        }
      }
      else
      {
        if ((ball.position.y - ball.radius) <= 0)
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = SOUTHWEST;
            break;
            case 1:
              new_ball_direction = SOUTH;
            break;
            case 2:
              new_ball_direction = WEST;
            break;
            default:
              new_ball_direction = SOUTHWEST;
            break;
          }
        }
      }
    break;
    default:
    break;
  }
  ball.position.direction = new_ball_direction;
  return ball.position.direction;
}
ball_type move_ball (ball_type ball)
{
  ball_type new_ball;

  new_ball = ball;
  switch (ball.position.direction)
  {
    case NORTH:
      new_ball.position.y--;
    break;
    case NORTHEAST:
      new_ball.position.x++;
      new_ball.position.y--;
    break;
    case EAST:
      new_ball.position.x++;
    break;
    case SOUTHEAST:
      new_ball.position.x++;
      new_ball.position.y++;
    break;
    case SOUTH:
      new_ball.position.y++;
    break;
    case SOUTHWEST:
      new_ball.position.x--;
      new_ball.position.y++;
    break;
    case WEST:
      new_ball.position.x--;
    break;
    case NORTHWEST:
      new_ball.position.x--;
      new_ball.position.y--;
    break;
    default:
    break;
  }
  return new_ball;
}
void draw_ball (ball_type ball)
{
  float theta, circle_iterations = 12.0;

  glColor3ub (ball.color.r, ball.color.g, ball.color.b);
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  glBegin (GL_POLYGON);
  glVertex3f (ball.position.x + ball.radius, ball.position.y, 0.0f);
  for (theta = 0; theta < 2 * M_PI; theta += M_PI / circle_iterations)
  {
    glVertex3f (ball.position.x + cos(theta) * ball.radius, ball.position.y + sin(theta) * ball.radius, 0.0f);
  }
  glEnd();
}
void draw_meta ()
{
  glColor3ub (0, 0xff, 0);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  if (linear_interp)
  {
    //TODO: implement Linear Interpolation if have time
  }
  else
  {
    for (int i=0; i<SQUARE_X; i++)
      for (int j=0; j<SQUARE_Y; j++)
      {
        float x0   = ((float)i)*(float)X_RESOLUTION/(float)SQUARE_X; //left corners of the square
        float y0   = ((float)j)*(float)Y_RESOLUTION/(float)SQUARE_Y; //top corners of the square
        float x1   = ((float)i+1.0f)*(float)X_RESOLUTION/(float)SQUARE_X; //right corners of the square
        float y1   = ((float)j+1.0f)*(float)Y_RESOLUTION/(float)SQUARE_Y; //bottom corners of the square
        float xMid = ((float)i+0.5f)*(float)X_RESOLUTION/(float)SQUARE_X; //X center of the square
        float yMid = ((float)j+0.5f)*(float)Y_RESOLUTION/(float)SQUARE_Y; //Y center of the square
        unsigned char sqr = 0;
        //====//
        //1  2//
        //4  8//
        //====//
        sqr | (dat_mat[i][j]>=1.0    ? 1:0);
        sqr | (dat_mat[i+1][j]>=1.0  ? 2:0);
        sqr | (dat_mat[i][j+1]>=1.0  ? 4:0);
        sqr | (dat_mat[i+1][j+1]>=1.0? 8:0);
        glBegin(GL_POLYGON);
        switch(sqr)
        {
          case 0x0:
            // o o
            // o o
            break;
          case 0x1:
            // x o
            // o o
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            break;
          case 0x2:
            // o x
            // o o
            glVertex3f(x1, y0, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            break;
          case 0x3:
            // x x
            // o o
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            glVertex3f(x1, y0, 0.0f);
            break;
          case 0x4:
            // o o
            // x o
            glVertex3f(x0, y1, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            break;
          case 0x5:
            // x o
            // x o
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, y1, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            break;
          case 0x6:
            // o x
            // x o
            glVertex3f(x1, y0, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            glVertex3f(x0, y1, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            break;
          case 0x7:
            // x x
            // x o
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, y1, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            glVertex3f(x1, y0, 0.0f);
            break;
          case 0x8:
            // o o
            // o x
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            break;
          case 0x9:
            // x o
            // o x
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            break;
          case 0xA:
            // o x
            // o x
            glVertex3f(x1, y0, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(x1, y1, 0.0f);
            break;
          case 0xB:
            // x x
            // o x
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            glVertex3f(xMid, y1, 0.0f);
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x1, y0, 0.0f);
            break;
          case 0xC:
            // o o
            // x x
            glVertex3f(x0, y1, 0.0f);
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            glVertex3f(x0, yMid, 0.0f);
            break;
          case 0xD:
            // x o
            // x x
            glVertex3f(x0, y0, 0.0f);
            glVertex3f(x0, y1, 0.0f);
            glVertex3f(x1, y1, 0.0f);
            glVertex3f(x1, yMid, 0.0f);
            glVertex3f(xMid, y0, 0.0f);
            break;
          case 0xE:
            // o x
            // x x
        }
        glEnd();

      }
  }
}
void calc_dat_mat()
{
  for (int i=0; i<SQUARE_X+1; i++)
    for (int j=0; j<SQUARE_Y+1; j++)
    {
      float tempVar = 0.0f; //temp Answer to be stored in matrix
      float tempX = (float)i*(float)X_RESOLUTION/(float)SQUARE_X; //X coordinate of the corner to be checked
      float tempY = (float)j*(float)Y_RESOLUTION/(float)SQUARE_Y; //X coordinate of the corner to be checked
      
      tempVar += (float)(ball1.radius*ball1.radius)/
                 ((tempX-(float)ball1.position.x)*(tempX-(float)ball1.position.x)+
                 (tempY-(float)ball1.position.y)*(tempY-(float)ball1.position.y));
      tempVar += (float)(ball2.radius*ball2.radius)/
                 ((tempX-(float)ball2.position.x)*(tempX-(float)ball2.position.x)+
                 (tempY-(float)ball2.position.y)*(tempY-(float)ball2.position.y));
      tempVar += (float)(ball3.radius*ball3.radius)/
                 ((tempX-(float)ball3.position.x)*(tempX-(float)ball3.position.x)+
                 (tempY-(float)ball3.position.y)*(tempY-(float)ball3.position.y));
      tempVar += (float)(ball4.radius*ball4.radius)/
                 ((tempX-(float)ball4.position.x)*(tempX-(float)ball4.position.x)+
                 (tempY-(float)ball4.position.y)*(tempY-(float)ball4.position.y));
      tempVar += (float)(ball5.radius*ball5.radius)/
                 ((tempX-(float)ball5.position.x)*(tempX-(float)ball5.position.x)+
                 (tempY-(float)ball5.position.y)*(tempY-(float)ball5.position.y));
      tempVar += (float)(ball6.radius*ball6.radius)/
                 ((tempX-(float)ball6.position.x)*(tempX-(float)ball6.position.x)+
                 (tempY-(float)ball6.position.y)*(tempY-(float)ball6.position.y));
      tempVar += (float)(ball7.radius*ball7.radius)/
                 ((tempX-(float)ball7.position.x)*(tempX-(float)ball7.position.x)+
                 (tempY-(float)ball7.position.y)*(tempY-(float)ball7.position.y));
      tempVar += (float)(ball8.radius*ball8.radius)/
                 ((tempX-(float)ball8.position.x)*(tempX-(float)ball8.position.x)+
                 (tempY-(float)ball8.position.y)*(tempY-(float)ball8.position.y));
      
      dat_mat[i][j]=tempVar;
    }
  //printf("%f\n",dat_mat[SQUARE_X/2][SQUARE_Y/2]);
}
void display(void)
{
  // * Ball 1 *
  if (ball_hit_wall(ball1))
  {
    ball1.position.direction = select_ball_direction(ball1);
  }
  ball1 = move_ball(ball1);
  // * Ball 2 *
  if (ball_hit_wall(ball2))
  {
    ball2.position.direction = select_ball_direction(ball2);
  }
  ball2 = move_ball(ball2);
  // * Ball 3 *
  if (ball_hit_wall(ball3))
  {
    ball3.position.direction = select_ball_direction(ball3);
  }
  ball3 = move_ball(ball3);
  // * Ball 4 *
  if (ball_hit_wall(ball4))
  {
    ball4.position.direction = select_ball_direction(ball4);
  }
  ball4 = move_ball(ball4);
  // * Ball 5 *
  if (ball_hit_wall(ball5))
  {
    ball5.position.direction = select_ball_direction(ball5);
  }
  ball5 = move_ball(ball5);
  // * Ball 6 *
  if (ball_hit_wall(ball6))
  {
    ball6.position.direction = select_ball_direction(ball6);
  }
  ball6 = move_ball(ball6);
  // * Ball 7 *
  if (ball_hit_wall(ball7))
  {
    ball7.position.direction = select_ball_direction(ball7);
  }
  ball7 = move_ball(ball7);
  // * Ball 8 *
  if (ball_hit_wall(ball8))
  {
    ball8.position.direction = select_ball_direction(ball8);
  }
  ball8 = move_ball(ball8);

  // clear the screen to black
  glColor3ub(0, 0, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glRecti(0, 0, X_RESOLUTION, Y_RESOLUTION);

  calc_dat_mat();
  draw_meta();

  if (plain_circle)
  {
    draw_ball (ball1);
    draw_ball (ball2);
    draw_ball (ball3);
    draw_ball (ball4);
    draw_ball (ball5);
    draw_ball (ball6);
    draw_ball (ball7);
    draw_ball (ball8);
  }

  glutSwapBuffers();
  glutPostRedisplay();
}
void reshape (int w, int h)  
{
  glViewport (0, 0, w, h);
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();  
  if (h == 0)  
  {
    glOrtho (0.0, X_RESOLUTION, 1, 0, -1.0, 1.0);
  }
  else
  {
    glOrtho (0.0, X_RESOLUTION, Y_RESOLUTION, 0, -1.0, 1.0);
  }
  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity(); 
}
void keyboard (unsigned char key, int x, int y) 
{
  switch (key) 
  {
    case 27:    //Esc key to quit
      exit (0);
    break;  
    default: 
    break;
  }
}

int main (int argc, char *argv[]) 
{
  // **************************
  // * ball 1 characteristics *
  // **************************
    ball1.position.x = X_RESOLUTION / 2;
    ball1.position.y = Y_RESOLUTION / 2,
    ball1.position.z = 0;
    ball1.position.direction = NORTH;
    ball1.radius = 50;
    ball1.color.r = 255;
    ball1.color.g = 255;
    ball1.color.b = 0;
  // **************************
  // * ball 2 characteristics *
  // **************************
    ball2.position.x = X_RESOLUTION / 2;
    ball2.position.y = Y_RESOLUTION / 2;
    ball2.position.z = 0;
    ball2.position.direction = EAST;
    ball2.radius = 50;
    ball2.color.r = 255;
    ball2.color.g = 255;
    ball2.color.b = 0;
  // **************************
  // * ball 3 characteristics *
  // **************************
    ball3.position.x = X_RESOLUTION / 2;
    ball3.position.y = Y_RESOLUTION / 2;
    ball3.position.z = 0;
    ball3.position.direction = SOUTH;
    ball3.radius = 50;
    ball3.color.r = 255;
    ball3.color.g = 255;
    ball3.color.b = 0;
  // **************************
  // * ball 4 characteristics *
  // **************************
    ball4.position.x = X_RESOLUTION / 2;
    ball4.position.y = Y_RESOLUTION / 2;
    ball4.position.z = 0;
    ball4.position.direction = WEST;
    ball4.radius = 50;
    ball4.color.r = 255;
    ball4.color.g = 255;
    ball4.color.b = 0;
  // **************************
  // * ball 5 characteristics *
  // **************************
    ball5.position.x = X_RESOLUTION / 2;
    ball5.position.y = Y_RESOLUTION / 2;
    ball5.position.z = 0;
    ball5.position.direction = NORTHEAST;
    ball5.radius = 50;
    ball5.color.r = 255;
    ball5.color.g = 255;
    ball5.color.b = 0;
  // **************************
  // * ball 6 characteristics *
  // **************************
    ball6.position.x = X_RESOLUTION / 2;
    ball6.position.y = Y_RESOLUTION / 2;
    ball6.position.z = 0;
    ball6.position.direction = NORTHWEST;
    ball6.radius = 50;
    ball6.color.r = 255;
    ball6.color.g = 255;
    ball6.color.b = 0;
  // **************************
  // * ball 7 characteristics *
  // **************************
    ball7.position.x = X_RESOLUTION / 2;
    ball7.position.y = Y_RESOLUTION / 2;
    ball7.position.z = 0;
    ball7.position.direction = SOUTHEAST;
    ball7.radius = 50;
    ball7.color.r = 255;
    ball7.color.g = 255;
    ball7.color.b = 0;
  // **************************
  // * ball 8 characteristics *
  // **************************
    ball8.position.x = X_RESOLUTION / 2;
    ball8.position.y = Y_RESOLUTION / 2;
    ball8.position.z = 0;
    ball8.position.direction = SOUTHEAST;
    ball8.radius = 50;
    ball8.color.r = 255;
    ball8.color.g = 255;
    ball8.color.b = 0;
  //
  // seed the random number generator
  srandom (time(0));
  // initialize control for drawing plain circle
  plain_circle = true;
  // initialize control for using Linear Interpolation
  linear_interp = false;

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
  glutInitWindowSize (X_RESOLUTION, Y_RESOLUTION);
  glutCreateWindow ("Marching Squares");
  glutDisplayFunc (display);  
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutMainLoop();
}

