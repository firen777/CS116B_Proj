/**Author: Un Hou Chan
 * Framerate independant animation reference:
 * http://hdrlab.org.nz/articles/amiga-os-articles/minigl-templates/frame-rate-independent-animation-using-glut/
*/

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

  #define GRAVITY 0.1f
  #define TIMERMSECS 33 // 33 ms per timer step
  #define TIMESTEP 0.033f  // 0.033 s per time step, ideally

  #define SPRING_L 10
  #define SPRING_K 1

  #define PART_COUNT 10
  #define PART_RADIUS 0.01f
// *************

// * Class Declarations *
  /**Particle class is the mathematicle model and data for a particle
   * member var: 
   *  s: Current Position to be drawn
   *  s_prev: Previous position. Used for Verlet
   *  r: collision radius of the particle
   *  a: acceleration that will be used in Verlet integration during next timestep. Reset after integration.
   *  fixed: indicate whether the particle is fixed or not
   * member func:
   *  verletStep(float dT): Verlet integrate the current position 
   *    as well as replace s_prev and resetting a. Fixed particle have no effect.
   *  accumA(Vec3f _a): Accumilating a via simple vector addition
   *  setV(Vec3f v): EXPERIMENTAL: hacky way to mimick velocity by manipulating the s_prev accordingly.
   *  
  */
  class Particle {
    public:
      Vec3f s; //Current Position
      Vec3f s_prev; //Previous Position
      float r;
      Vec3f a;
      int fixed; //to indicate whether the point is fixed or not
      
    public:
      /**Constructor
       * @param const Vec3f& _s, initial position of particle
      */
      Particle(const Vec3f& _s, float _r, int _fixed = FALSE): 
        s(_s), s_prev(_s), r(_r), a(Vec3f()), fixed(_fixed){}
      /**Constructor
       * @param float x,y,z; initial position of particle
      */
      Particle(float x, float y, float z, float _r, int _fixed = FALSE):
        s(Vec3f(x,y,z)), s_prev(Vec3f(x,y,z)), r(_r), a(Vec3f()), fixed(_fixed){}
      /**Default Constructor, set everything to 0*/
      Particle():s(Vec3f()),s_prev(Vec3f()),r(0.0f),a(Vec3f()),fixed(FALSE){}
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
      /**Accumilate acceleration for next timestep update
       * @param Vec3f _a; acceleration
      */
      void accumA(const Vec3f& _a){
        a = a + _a;
      }

      /** set s_prev according to desired v */
      void setV(const Vec3f& v) {
        s_prev = s - v*(TIMESTEP);
      }

  };

  /**SolidObject Interface is for object that can be collided with*/
  class SolidObject {
    public:
      /**Return surface normal given collision point
       * @param collision_point point to check against
       * @return Unit vector of the surface normal
      */
      virtual Vec3f surfaceNormal (const Vec3f& collision_point) = 0;
      /**Check if the a vertex collide w/ the object or not*/
      virtual bool isHit (const Vec3f& check_point) = 0;
      /**Return surface normal given collision particle
       * useful when particle is a sphere instead of a point
       * @param p Particle to check against
       * @return Unit vector of the surface normal
      */
      virtual Vec3f surfaceNormal (const Particle& p) = 0;
      /**Check if the a Particle collide w/ the object or not
       * useful when particle is a sphere instead of a point
      */
      virtual bool isHit (const Particle& p) = 0;
  };

  class SolidBall:SolidObject {
    public:
      float r;
      Vec3f c;
    public:
      /**Constructor*/
      SolidBall(const Vec3f& _center, float _radius):c(_center), r(_radius){}
      /**Defaul Constructor, set everything to 0*/
      SolidBall():r(0.0f), c(Vec3f()){}
      
      Vec3f surfaceNormal (const Vec3f& collision_point) {
        return (collision_point - c).getUnit();
      }
      
      bool isHit (const Vec3f& check_point) {
        if ((check_point - c).getL() <= r){
          return TRUE;
        }
        return FALSE;
      }
      
      Vec3f surfaceNormal (const Particle& p) {
        return (p.s - c).getUnit();
      }

      bool isHit (const Particle& p) {
        if ((p.s-c).getL() <= r + p.r){
          return TRUE;
        }
        return FALSE;
      }
  };

  /**Spring class descript spring between 2 Particle
   * Hooke's Law is expressed as follow: F=Kx -> ma=Kx
   * Since we only care about acceleration in this project:
   * a = (K/m)x
   * and let another constant k = (K/m)
   * 
   * member var:
   *  head ptr: pointer to one end of the spring
   *  end ptr: pointer to another end of the spring
   *  k: constant (K/m)
   *  l: rest length of the spring
   * member func:
   *  
  */
  class Spring {
    private:
      Particle* head;
      Particle* end;
      float k; //Hooke's Law
      float l;
    public:
      /**Constructor*/
      Spring(Particle* _head, Particle* _end):head(_head), end(_end), k(SPRING_K), l(SPRING_L){}
      /**Default Constructor, set everything to 0 or NULL*/
      Spring():head(NULL),end(NULL),k(0.0f),l(0.0f){}
      /**Hooke's Law, F=kx; a=(k/m)x.
       * Since we only care about acceleration here,
       * let (k/m) be another constant.
       * 
       * invoke accumilateA on both head and end
      */
      void springAct(){
        if (head!=NULL && end!=NULL){
          Vec3f direction = end->s - head->s;
          float currL = direction.getL();
          Vec3f x = (currL - l) * direction;
          head->accumA(k*x);
          end->accumA(-k*x);
        }
      }
  };

  /** "Director" of the physical world */
  class PhysSystem {
    public:
      Particle* part_list;
      int part_count;
      Spring* spring_list;
      int spring_count; //aka # of segments of rope
      SolidBall ball;
    public:
    /**Constructor. _head and _end is the position of the two ends of the rope*/
      PhysSystem (const Vec3f& _head, const Vec3f& _end, const SolidBall& _ball, int _part_count = PART_COUNT){
        part_count = _part_count;
        spring_count = _part_count - 1;
        ball = _ball;

        part_list = new Particle[part_count];
        spring_list = new Spring[spring_count];

        //initializing particles
        float segment_length = (_end - _head).getL() / spring_count;
        Vec3f head_end_direction = (_end - _head).getUnit();
        Vec3f part_position = _head;
        int fixed = FALSE;
        for (int i=0; i<part_count; i++) {
          fixed = TRUE ? FALSE : (i==0 || i==part_count); //fix first and last particles
          part_list[i] = Particle(part_position, PART_RADIUS, fixed);

          part_position = part_position + head_end_direction * segment_length;
        }
        



      }
      /**Default Constructor, set all to 0 or NULL*/
      PhysSystem (): part_list(NULL), part_count(0), spring_list(NULL), spring_count(0), ball(SolidBall()){}
      /**Destructor*/
      ~PhysSystem(){
        if (part_list!=NULL){
          delete [] part_list;
          part_list = NULL;
        }
        if (spring_list!=NULL){
          delete [] spring_list;
          spring_list = NULL;
        }
      }
  };

// **********************

// * GLOBAL variables and Objects.
  int pause = TRUE;
  //timer
  int startTime; 
  int prevTime;



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

  //Drawing function
  void drawBackground();
  void drawBall();
  void drawRope();

  /** animate function for time based animation */
  void animate(int value);

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
  drawBackground();
  
  glEnable (GL_LIGHTING);
  // glTranslatef (-6.5, 6, -9.0f); // move camera out and center on the rope
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

void drawBackground(){
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
}

void drawBall(){

}

void drawRope(){

}

void animate(int value){
  // Set up the next timer tick (do this first)
    glutTimerFunc(TIMERMSECS, animate, 0);

	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;

	// ##### REPLACE WITH YOUR OWN GAME/APP MAIN CODE HERE #####

	// ##### END OF GAME/APP MAIN CODE #####

	

	// Force a redisplay to render the new image
	glutPostRedisplay();

	prevTime = currTime;
}