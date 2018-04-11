/**Author: Un Hou Chan
 * 
 * README:
 * !! g++ compiler is needed for compiling "new" and "delete" operator !!
 * Keyboard control:
 * Esc: quit program
 * 'w': move ball forward
 * 's': move ball backward
 * arrow_up: move ball upward
 * arrow_down: move ball downward
 * arrow_left: move ball left
 * arrow_right: move ball right
 * 
 * 
 * 
 * 
 * Framerate independant animation reference:
 * http://hdrlab.org.nz/articles/amiga-os-articles/minigl-templates/frame-rate-independent-animation-using-glut/
 * 
 * 
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
  #define BALL_POSITION_X 7.0f
  #define BALL_POSITION_Y -2.0f
  #define BALL_POSITION_Z -40.0f
  #define BALL_RADIUS 6.5f
  #define TRUE 1
  #define FALSE 0

  #define BALL_TRANSLATION 0.5f

  #define GRAVITY 20.0f
  #define AIR_DRAG_K 0.1f
  #define DAMPEN_K 0.97f
  #define TIMERMSECS 5 // 33 ms per timer step
  #define TIMESTEP 0.05f  // 0.05 s per time step

  #define SPRING_L 8.0f //shouldn't use
  #define SPRING_K 150.0f

  #define PART_POSITION_X_1 -30.0f
  #define PART_POSITION_X_2 30.0f
  #define PART_POSITION_Y 8.0f
  #define PART_POSITION_Z -40.0f
  #define PART_COUNT 30
  #define PART_RADIUS 0.5f
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
    private:
      /**air drag
       * Force = 1/2 * (rho) * v^2 * K * Area
       * a = k * v^2
       * 
       * ...ANNNNNDDD it didn't work
      */
      void air_drag(){
        Vec3f v = s-s_prev;
        // Vec3f drag_force = -(v).getUnit() * v.getL() * v.getL() * AIR_DRAG_K ;

        // Vec3f drag_force = -(v).getUnit() * AIR_DRAG_K ;

        float v_scale = v.getL() / TIMESTEP;

        Vec3f drag_force = -(v).getUnit() * v_scale * AIR_DRAG_K ;
        
        accumA(drag_force);
      }
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
        if (fixed != TRUE) {
          Vec3f temp = s;
          //Acceleration Dampening
          // accumA((s_prev - s) * 0.2f * a.getL()); //-v dampening
          
          // float damp = 0.3f;
          // if (a.getL()<=damp)
          //   accumA(-a); //a thershold dampening
          // else
          //   accumA(-a.getUnit() * damp);

          air_drag();
          s = s + (s - s_prev)*DAMPEN_K + a*dT*dT;
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
      float k; 
      float l;
    public:
      Particle* head;
      Particle* end;
    public:
      /**Constructor*/
      Spring(Particle* _head, Particle* _end, float _k=SPRING_K, float _l=SPRING_L)
        :head(_head), end(_end), k(_k), l(_l){}
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
          Vec3f x = (currL - l) * direction.getUnit();
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

      Spring* stiff_list;
      int stiff_count;

      SolidBall ball;
    private:
      /**Accumilate gravity on all particles. Delegate function*/
      void accumGrav() {
        for (int i=0; i<part_count; i++) {
          part_list[i].accumA(Vec3f(0,-GRAVITY,0));
        }
      }
      /**Command All spring to act. Delegate function*/
      void springAllAct() {
        for (int i=0; i<spring_count; i++){
          spring_list[i].springAct();
        }
      }
      /**Command all particle to integrate. Delegate function*/
      void partIntegrate(float dT) {
        for (int i=0; i<part_count; i++) {
          part_list[i].verletStep(dT);
        }
      }
      /**Command all stiff spring to act...*/
      void stiffAllAct() {
        for (int i=0; i<stiff_count; i++){
          stiff_list[i].springAct();
        }
      }
      /**Particle colliding with ball*/
      void collisionCheckList() {
        for (int i=0; i<part_count; i++){
          if (ball.isHit(part_list[i])){
            Vec3f corrected_position = ball.c + ball.surfaceNormal(part_list[i]) * (ball.r + part_list[i].r);
            part_list[i].s = corrected_position;
            part_list[i].s_prev = corrected_position;
          }
        }
      }
    public:
      /**Constructor. _head and _end is the position of the two ends of the rope*/
      PhysSystem (const Vec3f& _head, const Vec3f& _end, const SolidBall& _ball, int _part_count = PART_COUNT){
        part_count = _part_count;
        spring_count = _part_count - 1;
        stiff_count = _part_count - 2;
        ball = _ball;

        part_list = new Particle[part_count];
        spring_list = new Spring[spring_count];
        stiff_list = new Spring[stiff_count];

        //initializing particles
        float segment_length = (_end - _head).getL() / spring_count;
        Vec3f head_end_direction = (_end - _head).getUnit();
        Vec3f part_position = _head;
        int fixed = FALSE;
        for (int i=0; i<part_count; i++) {
          fixed = (i==0 || i==part_count-1)? TRUE : FALSE ; //fix first and last particles
          part_list[i] = Particle(part_position, PART_RADIUS, fixed);

          part_position = part_position + head_end_direction * segment_length;
        }
        //initializing spring
        for (int i=0; i<spring_count; i++){
          spring_list[i] = Spring(&(part_list[i]), &(part_list[i+1]), SPRING_K, segment_length/1.0f);
        }
        //initializing stiff spring
        for (int i=0; i<stiff_count; i++){
          stiff_list[i] = Spring(&(part_list[i]), &(part_list[i+2]), SPRING_K, segment_length/0.5f);
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
      //TODO: collision
      void timestep(float dT){
        accumGrav();
        springAllAct();
        stiffAllAct();
        partIntegrate(dT);
        collisionCheckList();
      }

      void drawAll(float r=0.0f, float g=0.0f, float b=0.0f){
        glColor3f(r,g,b);
        //draw particles
        Vec3f p; //temp position of particle
        float p_r; //temp radius of particle
        for (int i=0; i<part_count; i++){
          p = part_list[i].s;
          p_r = part_list[i].r;

          glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            glutSolidSphere(p_r, 10, 10);
          glPopMatrix();
        }
        //draw rope segment
        Vec3f p1;
        Vec3f p2;
        glLineWidth(7.0f);
        for (int i=0; i<spring_count; i++){
          p1 = spring_list[i].head->s;
          p2 = spring_list[i].end ->s;
          glBegin(GL_LINES);
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
          glEnd();
        }

        //draw ball
        glColor3f(r+0.3f, g+0.3f, b+0.3f);
        glPushMatrix();
            glTranslatef(ball.c.x, ball.c.y, ball.c.z);
            glutSolidSphere(ball.r, 50, 50);
        glPopMatrix();

      }
  };

// **********************

// * GLOBAL variables and Objects.
  int pause = TRUE;
  //timer start and prev time, in ms
  int startTime = 0; 
  int prevTime = 0;

  SolidBall global_ball = SolidBall(Vec3f(BALL_POSITION_X, BALL_POSITION_Y, BALL_POSITION_Z), BALL_RADIUS);
  PhysSystem global_sys = PhysSystem(Vec3f(PART_POSITION_X_1, PART_POSITION_Y, PART_POSITION_Z),
                                     Vec3f(PART_POSITION_X_2, PART_POSITION_Y, PART_POSITION_Z),
                                     global_ball);

  //tester
  // Particle p1(Vec3f(BALL_POSITION_X-30, BALL_POSITION_Y +30, BALL_POSITION_Z), PART_RADIUS);
  // Particle p2(Vec3f(BALL_POSITION_X+30, BALL_POSITION_Y +30, BALL_POSITION_Z), PART_RADIUS);
  // Spring s12(&p1, &p2);

  
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
  // void drawBall();
  // void drawRope(); //packaged inside physsystem...
  void testDraw(); //testing function; render individual components for testing

  /** animate function for time based animation */
  void animate(int value);

// ***********************

void testDraw(){
  // glColor3f(1.0f,0,0);
  // glPushMatrix();
  //   glTranslatef(p1.s.x, p1.s.y, p1.s.z);
  //   glutSolidSphere(p1.r, 10, 10);
  // glPopMatrix();

  // glPushMatrix();
  //   glTranslatef(p2.s.x, p2.s.y, p2.s.z);
  //   glutSolidSphere(p2.r, 10, 10);
  // glPopMatrix();
  

}

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
  // Start the timer

  glutTimerFunc(TIMERMSECS, animate, 0);

	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

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

  drawBackground();
  
  glEnable (GL_LIGHTING);
  // glTranslatef (-6.5, 0, -4.0f); // move camera out and center on the rope
  // global_sys.timestep(1.1f);
  global_sys.drawAll(0.0f, 0.5f, 0.0f);
  // testDraw();

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
    case 'w':
      global_sys.ball.c = global_sys.ball.c - Vec3f(0, 0, BALL_TRANSLATION);
    break;
    case 's':
      global_sys.ball.c = global_sys.ball.c + Vec3f(0, 0, BALL_TRANSLATION);
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
      global_sys.ball.c = global_sys.ball.c + Vec3f(0, BALL_TRANSLATION, 0);
    break;
    case GLUT_KEY_DOWN: 
      global_sys.ball.c = global_sys.ball.c - Vec3f(0, BALL_TRANSLATION, 0);
    break;
    case GLUT_KEY_LEFT:
      global_sys.ball.c = global_sys.ball.c - Vec3f(BALL_TRANSLATION, 0, 0);
    break;
    case GLUT_KEY_RIGHT:
      global_sys.ball.c = global_sys.ball.c + Vec3f(BALL_TRANSLATION, 0, 0);
    break;
    default:
    break;
  }
}

void drawBackground(){
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


void animate(int value){
  // Set up the next timer tick (do this first)
    glutTimerFunc(TIMERMSECS, animate, 0);

	// Measure the elapsed time
	// int currTime = glutGet(GLUT_ELAPSED_TIME);
	// int timeSincePrevFrame = currTime - prevTime;
  // float timeSincePrev_ms = timeSincePrevFrame/2000.0f;
	// int elapsedTime = currTime - startTime;

	// ##### REPLACE WITH YOUR OWN GAME/APP MAIN CODE HERE #####
  // if (timeSincePrev_ms < 0.07f) {
  //   global_sys.timestep(timeSincePrev_ms);
  // }


  // 
  // printf("%f\n", timeSincePrevFrame/1000.0f);
  // printf("%f\n", TIMESTEP);
  global_sys.timestep(TIMESTEP);

	// ##### END OF GAME/APP MAIN CODE #####

	//!! Non-constant dT sucks lol

	// Force a redisplay to render the new image
	glutPostRedisplay();

	// prevTime = currTime;
}

