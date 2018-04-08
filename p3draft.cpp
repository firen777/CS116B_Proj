//standard lib
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//GLUT include thingy
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif

//Custom Library
#include "aclib/vec3.h"


// * Constants *
  #define BALL_POSITION_X 6
  #define BALL_POSITION_Y -2
  #define BALL_POSITION_Z 0
  #define BALL_RADIUS 0.75
  #define TRUE 1
  #define FALSE 0

  #define GRAVITY 1.0f
// *************

// * Class Declarations *
  /**Particle class is the mathematicle model and data for a particle
   * member: 
   *  position s: Current Position to be drawn
   *  previous position s_prev: Previous position. Used for Verlet
   *  acceleration a,
  */
  class Particle {
    private:
      Vec3f s; //Current Position
      Vec3f s_prev; //Previous Position
      Vec3f a;
      int fixed; //to indicate whether the point is fixed or not
    public:
      /**Constructor
       * @param const Vec3f& _s, initial position of particle
      */
      Particle(const Vec3f& _s, int _fixed = FALSE){
        s = _s;
        s_prev = _s;
        a = Vec3f();
        fixed = _fixed;
      }
      /**Constructor
       * @param float x,y,z; initial position of particle
      */
      Particle(float x, float y, float z, int _fixed = FALSE){
        s = Vec3f(x,y,z);
        s_prev = Vec3f(x,y,z);
        a = Vec3f();
        fixed = _fixed;
      }

      /**stepping w/ verlet integration
       * update s and s_prev
       * reset a in the process
       * @param float dT timestep
      */
      void verletStep(float dT) {
        if (fixed != 1) {
          Vec3f temp = s;
          s = 2*s - s_prev + a*dT*dT;
          s_prev = temp;
        }
        a = Vec3f();
      }
      
      //**********Mutators**********//
      /**Accumilate acceleration for next timestep update
       * @param Vec3f _a; acceleration
      */
      void accumilateA(Vec3f _a){
        a = a + _a;
      }
      /**set s*/
      void setS(Vec3f _s){
        s = _s;
      }
      /**set s_prev*/
      void setS_prev(Vec3f _s_prev){
        s_prev = _s_prev;
      }

      //**********Accessors**********//
      Vec3f getS(){
        return s;
      }


  };

  /**SolidObject Interface is for object that can be collided with
   * TODO: TO BE FINISHED
  */
  class SolidObject {
    public:
      virtual Vec3f surfaceNormal (Vec3f collision_point) = 0;
  };

  //TODO: TO BE FINISHED
  class SolidBall:SolidObject {
    private:
      Vec3f center;
    public:
      SolidBall(Vec3f _center):center(_center){}
      Vec3f surfaceNormal (Vec3f collision_point){
        return Vec3f();
      }

  };

  /**Spring class descript spring between 2 Particle*/
  class Spring {
    private:
      Particle* head;
      Particle* end;
      float k;
      float l;
    public:
      Spring(Particle* _head, Particle* _end):head(_head), end(_end), k(1), l(15){}

      /**Hooke's Law, F=kx; a=(k/m)x.
       * Since we only care about acceleration here,
       * let (k/m) be another constant.
       * 
       * invoke accumilateA on both head and end
      */
      void springAct(){
        Vec3f direction = end->getS() - head->getS();
        float currL = direction.getL;
        Vec3f x = (currL - l) * direction;
        head->accumilateA(k*x);
        end->accumilateA(-k*x);
      }
  };

  class PhysSystem {

  };
// **********************





// * GLOBAL variables and Objects.
  int pause = TRUE;


// ****************


// * Function prototypes *

  /** Main function */
  int main (int argc, char *argv[]);
  /** Initialize Lighting and Stuffs */
  void init (void);
  /** Display callback for GLUT */
  void display (void);
  /** Reshape callback for GLUT */
  void reshape (int w, int h);
  /** Keyboard callback for GLUT */
  void keyboard (unsigned char key, int x, int y);
  /** Arrow callback for GLUT */
  void arrow_keys (int a_keys, int x, int y); 

// ***********************

int main (int argc, char *argv[]) 
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
  glutInitWindowSize (1280, 720 ); 
  glutCreateWindow ("Rope simulator");
  init ();
  glutDisplayFunc (display);  
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc (arrow_keys);
  glutMainLoop ();
}

void init (void)
{
  glShadeModel (GL_SMOOTH);
  glClearColor (0.2f, 0.2f, 0.4f, 0.5f);				
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_COLOR_MATERIAL);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  GLfloat lightPos[4] = {-1.0, 1.0, 0.5, 0.0};
  glLightfv (GL_LIGHT0, GL_POSITION, (GLfloat *) &lightPos);
  glEnable (GL_LIGHT1);
  GLfloat lightAmbient1[4] = {0.0, 0.0,  0.0, 0.0};
  GLfloat lightPos1[4]     = {1.0, 0.0, -0.2, 0.0};
  GLfloat lightDiffuse1[4] = {0.5, 0.5,  0.3, 0.0};
  glLightfv (GL_LIGHT1,GL_POSITION, (GLfloat *) &lightPos1);
  glLightfv (GL_LIGHT1,GL_AMBIENT, (GLfloat *) &lightAmbient1);
  glLightfv (GL_LIGHT1,GL_DIFFUSE, (GLfloat *) &lightDiffuse1);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void display (void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();
  glDisable (GL_LIGHTING);
  glBegin (GL_POLYGON); //Skyblue Background 
    glColor3f (0.8f, 0.8f, 1.0f);
    glVertex3f (-200.0f, -100.0f, -100.0f);
    glVertex3f (200.0f, -100.0f, -100.0f);
    glColor3f (0.4f, 0.4f, 0.8f);	
    glVertex3f (200.0f, 100.0f, -100.0f);
    glVertex3f (-200.0f, 100.0f, -100.0f);
  glEnd ();
  glEnable (GL_LIGHTING);
  glTranslatef (-6.5, 6, -9.0f); // move camera out and center on the rope
  // glTranslatef (6.5, 6, -9.0f);
  glPushMatrix ();
  glTranslatef (BALL_POSITION_X, BALL_POSITION_Y, BALL_POSITION_Z);
  glColor3f (1.0f, 1.0f, 0.0f);
  glutSolidSphere (BALL_RADIUS - 0.1, 50, 50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of rope penetrating the ball slightly
  glLineWidth(10);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(10, 0, 0);
  glEnd();
  glPopMatrix ();
  glutSwapBuffers();
  glutPostRedisplay();
}





void reshape (int w, int h)  
{
  glViewport (0, 0, w, h);
  glMatrixMode (GL_PROJECTION); 
  glLoadIdentity ();  
  if (h == 0)  
  { 
    gluPerspective (80, (float) w, 1.0, 5000.0);
  }
  else
  {
    gluPerspective (80, (float) w / (float) h, 1.0, 5000.0);
  }
  glMatrixMode (GL_MODELVIEW);  
  glLoadIdentity (); 
}





void keyboard (unsigned char key, int x, int y) 
{
  switch (key) 
  {
    case 27:    
      exit(0);
    break;  
    case 32:
      pause = 1 - pause;
      break;
    default: 
    break;
  }
}





void arrow_keys (int a_keys, int x, int y) 
{
  switch(a_keys) 
  {
    case GLUT_KEY_UP:
      glutFullScreen();
    break;
    case GLUT_KEY_DOWN: 
      glutReshapeWindow (1280, 720 );
    break;
    default:
    break;
  }
}







