/**Author: Un Hou Chan
 * 
 * README:
 * !! g++ compiler is needed for compiling "new" and "delete" operator !!
 * 
 * Framerate independant animation reference:
 * http://hdrlab.org.nz/articles/amiga-os-articles/minigl-templates/frame-rate-independent-animation-using-glut/
 * 
 * Keyboard function:
 * Spacebar: pause and unpause the program
 * 'r': reset the cloth
 * 'k': add some random acceleration to each particles
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
  //Boolean
  #define TRUE 1
  #define FALSE 0
  //Ball
  #define BALL_POSITION_X 7.0f
  #define BALL_POSITION_Y -2.0f
  #define BALL_POSITION_Z -10.0f
  #define BALL_RADIUS 1.5f
  #define BALL_TRANSLATION 0.1f
  //World
  #define GRAVITY 20.0f
  #define AIR_DRAG_K 0.1f
  #define DAMPEN_K 0.99f
  #define TIMERMSECS 1 // 1 ms per timer step
  #define TIMESTEP 0.01f  // 0.01 s per time step
  #define WIND_FORCE 200.0f //wind acceleration field
  #define WIND_FIELD_RADIUS 2.0f
  //Spring
  #define SPRING_L 8.0f //shouldn't use
  #define SPRING_K 20.0f
  #define SPRING_C 0.01f //Spring constraint constant. 10% more of it's rest length at most
  //Particles
  #define PART_POSITION_X_1 -5.0f
  #define PART_POSITION_X_2 5.0f
  #define PART_POSITION_Y 6.0f
  #define PART_POSITION_Z -10.0f
  #define PART_ROW_COUNT 50
  #define PART_COL_COUNT 50
  #define PART_RADIUS 0.1f
  //Boundary
  #define BOUND_LEFT -10.f
  #define BOUND_RIGHT 10.f
  #define BOUND_UP 10.f
  #define BOUND_DOWN -10.f
  #define BOUND_FORW -20.f
  #define BOUND_BACK -5.0f
// *************

// * GLOBAL Var part 1 *
  static float wind_x = 0.0f;
  static float wind_y = 0.0f;
  static float wind_z_dir = -1.0f; 
// *************

// * Class Declarations *
  enum BALL_DIR{
    BALL_LEFT,
    BALL_RIGHT,
    BALL_UP,
    BALL_DOWN,
    BALL_FORW,
    BALL_BACK
  };

  /**Particle class is the mathematicle model and data for a particle
   * member var: 
   *  s: Current Position to be drawn
   *  s_prev: Previous position. Used for Verlet
   *  s_corr: correction vector. Used for displacement correction.
   *  r: collision radius of the particle
   *  a: acceleration that will be used in Verlet integration during next timestep. Reset after integration.
   *  fixed: indicate whether the particle is fixed or not
   * member func:
   *  verletStep(float dT): Verlet integrate the current position 
   *    as well as replace s_prev and resetting a. Fixed particle have no effect.
   *  accumA(Vec3f _a): Accumilating a via simple vector addition
   *  setV(Vec3f v): EXPERIMENTAL: hacky way to mimick velocity by manipulating the s_prev accordingly.
   *  accumCorr(Vec3f _c): accumilate correction vector
   *  partCorr(): add s_corr to s and reset s_corr afterward.
   *  
  */
  class Particle {
    public:
      Vec3f s; //Current Position
      Vec3f s_prev; //Previous Position
      Vec3f s_corr; //constraint correction vector
      float r;
      Vec3f a;
      int fixed; //to indicate whether the point is fixed or not
    private:
      /**air drag
       * Force = 1/2 * (rho) * v^2 * K * Area
       * a = k * v^2
       * 
       * Unused. not much different. Use dampening instead.
      */
      void air_drag(){
        Vec3f v = s-s_prev;
        float v_scale = v.getL() / TIMESTEP;
        Vec3f drag_force = -(v).getUnit() * v_scale * AIR_DRAG_K ;
        
        accumA(drag_force);
      }
    public:
      /**Constructor
       * @param const Vec3f& _s, initial position of particle
      */
      Particle(const Vec3f& _s, float _r, int _fixed = FALSE): 
        s(_s), s_prev(_s), s_corr(Vec3f()), r(_r), a(Vec3f()), fixed(_fixed){}
      /**Constructor
       * @param float x,y,z; initial position of particle
      */
      Particle(float x, float y, float z, float _r, int _fixed = FALSE):
        s(Vec3f(x,y,z)), s_prev(Vec3f(x,y,z)), s_corr(Vec3f()), r(_r), a(Vec3f()), fixed(_fixed){}
      /**Default Constructor, set everything to 0*/
      Particle():
        s(Vec3f()),s_prev(Vec3f()),s_corr(Vec3f()),r(0.0f),a(Vec3f()),fixed(FALSE){}
      /**stepping w/ verlet integration
       * update s and s_prev
       * reset a in the process
       * @param float dT timestep
      */
      void verletStep(float dT) {
        if (fixed != TRUE) {
          Vec3f temp = s;
          // air_drag();
          s = s + (s - s_prev)*DAMPEN_K + a*dT*dT;
          s_prev = temp;
        }
        a = Vec3f(); //reset acceleration to zero after a iteration
      }      
      /**Accumilate acceleration for next timestep update
       * @param Vec3f _a; acceleration
      */
      void accumA(const Vec3f& _a){
        a = a + _a;
      }

      /**Accumilate correction vector for constraints
       * @param Vec3f _c; correction vector
      */
      void accumCorr(const Vec3f& _c){
         s_corr = s_corr + _c;
      }

      /** set s_prev according to desired v */
      void setV(const Vec3f& v) {
        s_prev = s - v*(TIMESTEP);
      }

      /**Correct the particle position according to correction vector. 
       * reset the vector afterward
      */
      void partCorr(){
        s = s + s_corr;
        s_corr = Vec3f();
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

  /**SolidBall
   * member var:
   *  r: radius of the Ball
   *  c: center point of the sphere
   *  dir: direction of the ball
   * member func:
   *  isHit(point); return boolean of whether the given point collide with the ball.
   *  surfaceNormal(collision_point); return Vec3f of the Normal unit vector given a collision point
   *  ballMoving(x); move the ball to one direction and loop inside the boundary
  */
  class SolidBall:SolidObject {
    public:
      float r;
      Vec3f c;
      BALL_DIR dir;
    public:
      /**Constructor*/
      SolidBall(const Vec3f& _center, float _radius, BALL_DIR _dir = BALL_LEFT):c(_center), r(_radius), dir(_dir){}
      /**Defaul Constructor, set everything to 0*/
      SolidBall():r(0.0f), c(Vec3f()), dir(BALL_LEFT){}
      
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

      void ballMoving(float s = BALL_TRANSLATION){
        switch (dir)
        {
          case BALL_LEFT:
            c = c + Vec3f(-s, 0.0f, 0.0f);
            if (c.x < BOUND_LEFT) c.x = BOUND_RIGHT;
            break;
          case BALL_RIGHT:
            c = c + Vec3f(s, 0.0f, 0.0f);
            if (c.x > BOUND_RIGHT) c.x = BOUND_LEFT;
            break;
          case BALL_UP:
            c = c + Vec3f(0.0f, s, 0.0f);
            if (c.y > BOUND_UP) c.y = BOUND_DOWN;
            break;
          case BALL_DOWN:
            c = c + Vec3f(0.0f, -s, 0.0f);
            if (c.y < BOUND_DOWN) c.y = BOUND_UP;
            break;
          case BALL_FORW:
            c = c + Vec3f(0.0f, 0.0f, -s);
            if (c.z < BOUND_FORW) c.z = BOUND_BACK;
            break;
          case BALL_BACK:
            c = c + Vec3f(0.0f, 0.0f, s);
            if (c.z > BOUND_BACK) c.z = BOUND_FORW;
            break;
          default:
            break;
        }
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
   *  c: spring constraint constant, percentage of the maximum deformation allowed in a spring
   * member func:
   *  springAddA(); act on the connected particles and assert the spring force (acceleration) on them
   *  springAddConstraint(); add the correction vector to head and/or end particles according to c and correct them immediately
  */
  class Spring {
    private:
      float k; 
      float l;
      float c; //constraint constant
    public:
      Particle* head;
      Particle* end;
    public:
      /**Constructor*/
      Spring(Particle* _head, Particle* _end, float _k=SPRING_K, float _l=SPRING_L, float _c=SPRING_C)
        :head(_head), end(_end), k(_k), l(_l), c(_c){}
      /**Default Constructor, set everything to 0 or NULL*/
      Spring():head(NULL),end(NULL),k(0.0f),l(0.0f),c(0.0f){}
      /**Hooke's Law, F=kx; a=(k/m)x.
       * Since we only care about acceleration here,
       * let (k/m) be another constant.
       * 
       * invoke accumilateA on both head and end
      */
      void springAddA(){
        if (head!=NULL && end!=NULL){
          Vec3f head_to_end = end->s - head->s;
          float currL = head_to_end.getL();
          Vec3f x = (currL - l) * head_to_end.getUnit();
          head->accumA(k*x);
          end->accumA(-k*x);
        }
      }

      /**Spring accumilate contraint to correct the the position of the particles
       * in order to avoid super elasticity
      */
      void springAddConstraint(){
        if (head!=NULL && end!=NULL){
          Vec3f head_to_end = end->s - head->s;
          float currL = head_to_end.getL();
          float critL = l * (1.0f + c);
          Vec3f direction = head_to_end.getUnit();

          if (head->fixed==FALSE && end->fixed==FALSE) {
            head->accumCorr((currL - critL)* 0.5f * direction);
            end->accumCorr (-(currL - critL)* 0.5f * direction);

            head->partCorr();
            end->partCorr();

            // head->accumCorr(head_to_end * (1.0f - l/currL) * 0.5f);
            // end->accumCorr(head_to_end * (1.0f - l/currL) * -0.5f);

            // head->partCorr();
            // end->partCorr();
          } 
          else if (head->fixed==TRUE && end->fixed==FALSE){
            end->accumCorr(-(currL - critL) * direction);

            end->partCorr();
            
            // end->accumCorr(head_to_end * (1.0f - l/currL) * -1.0f);

            // end->partCorr();
          }
          else if (head->fixed==FALSE && end->fixed==TRUE){
            head->accumCorr((currL - critL) * direction);

            head->partCorr();
            
            // head->accumCorr(head_to_end * (1.0f - l/currL) * 1.0f);

            // head->partCorr();
          }

        }
      }
  };

  /** "Director" of the physical world */
  class PhysSystem {
    private:
      Particle* part_list;
      int part_row_count; //how many rows
      int part_col_count; //each rows how many particles
      
      Spring* spring_hori;
      int spring_hori_row_count;
      int spring_hori_col_count;

      Spring* spring_vert;
      int spring_vert_row_count;
      int spring_vert_col_count;

      Spring* shear_tlbr; //top left -- bottom right
      int shear_tlbr_row_count;
      int shear_tlbr_col_count;

      Spring* shear_trbl; //top right -- bottom left
      int shear_trbl_row_count;
      int shear_trbl_col_count;

      Spring* stiff_hori;
      int stiff_hori_row_count;
      int stiff_hori_col_count;

      Spring* stiff_vert;
      int stiff_vert_row_count;
      int stiff_vert_col_count;

      SolidBall* ball; 
    private:
      /**Accumilate gravity on all particles. Delegate function*/
      void accumGrav() {
        for (int i=0; i<part_row_count * part_col_count; i++){
          part_list[i].accumA(Vec3f(0.0f,-GRAVITY,0.0f));
        }
      }

      /**Accumilate wind force on all particles.*/
      void accumWind(float x, float y, float r=WIND_FIELD_RADIUS, float a=WIND_FORCE){
        Vec3f windCenter(x, y, 0.0f);
        Vec3f projected_pos(0.0f, 0.0f, 0.0f);
        Vec3f wind_acceleration(0.0f, 0.0f, 0.0f);
        Vec3f wind_field(0.0f, 0.0f, -a);

        for (int i=0; i<part_row_count-1; i++){
          for (int j=0; j<part_col_count-1; j++){

            Vec3f surface_normal(part_list[i*part_row_count + j].s, 
                                 part_list[i*part_row_count + j+1].s, 
                                 part_list[(i+1)*part_row_count + j].s); //surface normal of [i][j],[i][j+1],[i+1][j]

            projected_pos.x = part_list[i*part_row_count + j].s.x;
            projected_pos.y = part_list[i*part_row_count + j].s.y;

            if ((projected_pos - windCenter).getL() <= r){ //the X-Y position is inside the wind circle
              wind_acceleration = Vec3f(0.0f,
                                   0.0f,
                                   wind_z_dir * abs(surface_normal.getUnit()*wind_field));
              part_list[i*part_row_count + j].accumA(wind_acceleration);
              // part_list[i*part_row_count + j].accumA(wind_field);

            }
          }
        }
      }

      /**Command All spring to act. Delegate function*/
      void springAllAddA() {
        for (int i=0; i<spring_hori_row_count * spring_hori_col_count; i++){
          spring_hori[i].springAddA();
        }
        for (int i=0; i<spring_vert_row_count * spring_vert_col_count; i++){
          spring_vert[i].springAddA();
        }
      }
      /**Command All shear spring to act. Delegate function*/
      void shearAllAddA() {
        for (int i=0; i<shear_tlbr_row_count * shear_tlbr_col_count; i++){
          shear_tlbr[i].springAddA();
        }
        for (int i=0; i<shear_trbl_row_count * shear_trbl_col_count; i++){
          shear_trbl[i].springAddA();
        }
      }
      /**Command all stiff spring to act. Delegate function*/
      void stiffAllAddA() {
        for (int i=0; i<stiff_hori_row_count * stiff_hori_col_count; i++){
          stiff_hori[i].springAddA();
        }
        for (int i=0; i<stiff_vert_row_count * stiff_vert_col_count; i++){
          stiff_vert[i].springAddA();
        }
      }
      /**Command all particle to integrate. Delegate function*/
      void partIntegrate(float dT) {
        for (int i=0; i<part_row_count * part_col_count; i++){
          part_list[i].verletStep(dT);
        }
      }
      /**Command all string to constraint. Delegate function*/
      void springAllConstraint(){
        for (int i=0; i<spring_hori_row_count * spring_hori_col_count; i++){
          spring_hori[i].springAddConstraint();
        }
        for (int i=0; i<spring_vert_row_count * spring_vert_col_count; i++){
          spring_vert[i].springAddConstraint();
        }
      }
      /**Command all string to constraint. Delegate function*/
      void shearAllConstraint(){
        for (int i=0; i<shear_tlbr_row_count * shear_tlbr_col_count; i++){
          shear_tlbr[i].springAddConstraint();
        }
        for (int i=0; i<shear_trbl_row_count * shear_trbl_col_count; i++){
          shear_trbl[i].springAddConstraint();
        }
      }
      /**Command all string to constraint. Delegate function*/
      void stiffAllConstraint(){
        for (int i=0; i<stiff_hori_row_count * stiff_hori_col_count; i++){
          stiff_hori[i].springAddConstraint();
        }
        for (int i=0; i<stiff_vert_row_count * stiff_vert_col_count; i++){
          stiff_vert[i].springAddConstraint();
        }
      }
      /**All Particle correct their position*/
      void partAllCorrect(){
        for (int i=0; i<part_row_count*part_col_count; i++){
          part_list[i].partCorr();
        }
      }

      /** +- 0.00001f to 50.00001f */
      float nextRand(void)
      {
        //0.00001f to 5.00001f
        float ans = (float)rand()/(float)(RAND_MAX/50.0f)+0.00001f;
        // 50% +, 50% -
        if (rand()%2 == 1)
          ans *= -1.0;
        return ans;
      }
      /**Particle colliding with ball*/ //not needed in Project 4
      void collisionCheckList() {
        for (int i=0; i<part_row_count * part_col_count; i++){
          if (ball->isHit(part_list[i])){
            Vec3f corrected_position = ball->c + ball->surfaceNormal(part_list[i]) * (ball->r + part_list[i].r);
            part_list[i].s = corrected_position;
            part_list[i].s_prev = corrected_position;
          }
        }
      }
      
    public:
      /**Constructor. */
      PhysSystem (const Vec3f& topleft, const Vec3f& topright, SolidBall* _ball,
                  int _part_row_count = PART_ROW_COUNT, int _part_col_count = PART_COL_COUNT)
      {
        //counting:

        part_row_count = _part_row_count;
        part_col_count = _part_col_count;

        spring_hori_row_count = _part_row_count;
        spring_hori_col_count = _part_col_count-1;

        spring_vert_row_count = _part_row_count-1;
        spring_vert_col_count = _part_col_count;

        shear_tlbr_row_count = _part_row_count - 1;
        shear_tlbr_col_count = _part_col_count - 1;
        
        shear_trbl_row_count = _part_row_count - 1;
        shear_trbl_col_count = _part_col_count - 1;

        stiff_hori_row_count = _part_row_count;
        stiff_hori_col_count = _part_col_count - 2;

        stiff_vert_row_count = _part_row_count - 2;
        stiff_vert_col_count = _part_col_count;

        ball = _ball;

        

        //Memory Allocation
        part_list = new Particle[part_row_count * part_col_count];    //Dynamically allocate an array of pointers
        spring_hori = new Spring[spring_hori_row_count * spring_hori_col_count]; //similar...
        spring_vert = new Spring[spring_vert_row_count * spring_vert_col_count];
        shear_tlbr = new Spring[shear_tlbr_row_count * shear_tlbr_col_count];
        shear_trbl = new Spring[shear_trbl_row_count * shear_trbl_col_count];
        stiff_hori = new Spring[stiff_hori_row_count * stiff_hori_col_count];
        stiff_vert = new Spring[stiff_vert_row_count * stiff_vert_col_count];
        
        //Initialization
        float segment_length = (topright - topleft).getL() / spring_hori_col_count;
        
        Vec3f head_end_direction = (topright - topleft).getUnit();
        Vec3f down_direction(0,-1,0);
        
        Vec3f part_position = topleft;

        for (int i=0; i<part_row_count; i++){//outer loop for operating on which row
          for (int j=0; j<part_col_count; j++){ //inner loop for particles for each row
            part_list[i * part_col_count + j] = Particle(part_position, PART_RADIUS);
            part_position = part_position + head_end_direction * segment_length;
          }
          part_position = part_position - head_end_direction * segment_length * part_col_count;
          part_position = part_position + down_direction * segment_length;
        } //[i]rows[j]columns
        part_list[0].fixed = TRUE;
        part_list[1].fixed = TRUE;
        part_list[2].fixed = TRUE;
        part_list[0 + part_col_count-1].fixed = TRUE;
        part_list[0 + part_col_count-2].fixed = TRUE;
        part_list[0 + part_col_count-3].fixed = TRUE;

        for (int i=0; i<spring_hori_row_count; i++){
          for (int j=0; j<spring_hori_col_count; j++) {
            spring_hori[i*spring_hori_col_count + j] = 
              Spring(&(part_list[i*part_col_count + j]), &(part_list[i*part_col_count + j+1]), SPRING_K, segment_length);
          }
        } //[i][j] & [i][j+1]

        for (int i=0; i<spring_vert_row_count; i++){
          for (int j=0; j<spring_vert_col_count; j++) {
            spring_vert[i*spring_vert_col_count + j] = 
              Spring(&(part_list[i*part_col_count + j]), &(part_list[(i+1)*part_col_count + j]), SPRING_K, segment_length);
          }
        } //[i][j] & [i+1][j]

        for (int i=0; i<shear_tlbr_row_count; i++){
          for (int j=0; j<shear_tlbr_col_count; j++) {
            shear_tlbr[i*shear_tlbr_col_count +j] = 
              Spring(&(part_list[i*part_col_count + j]), &(part_list[(i+1)*part_col_count + j+1]), SPRING_K, segment_length*M_SQRT2);
          }
        } //[i][j] & [i+1][j+1]

        for (int i=0; i<shear_trbl_row_count; i++){
          for (int j=0; j<shear_trbl_col_count; j++) {
            shear_trbl[i*shear_trbl_col_count + j] = 
              Spring(&(part_list[(i+1)*part_col_count + j]), &(part_list[i*part_col_count + j+1]), SPRING_K, segment_length*M_SQRT2);
          }
        } //[i+1][j] & [i][j+1]

        for (int i=0; i<stiff_hori_row_count; i++){
          for (int j=0; j<stiff_hori_col_count; j++) {
            stiff_hori[i*stiff_hori_col_count + j] = 
              Spring(&(part_list[i*part_col_count + j]), &(part_list[i*part_col_count + j+2]), SPRING_K, segment_length*2);
          }
        } //[i][j] & [i][j+2]

        for (int i=0; i<stiff_vert_row_count; i++){
          for (int j=0; j<stiff_vert_col_count; j++) {
            stiff_vert[i*stiff_vert_col_count + j] = 
              Spring(&(part_list[i*part_col_count + j]), &(part_list[(i+2)*part_col_count + j]), SPRING_K, segment_length*2);
          }
        }
      }
      /**Default Constructor, set all to 0 or NULL*/
      PhysSystem (): 
        part_list(NULL), part_row_count(0), part_col_count(0), 
        spring_hori(NULL), spring_hori_row_count(0), spring_hori_col_count(0),
        spring_vert(NULL), spring_vert_row_count(0), spring_vert_col_count(0),
        shear_tlbr(NULL), shear_tlbr_row_count(0), shear_tlbr_col_count(0),
        shear_trbl(NULL), shear_trbl_row_count(0), shear_trbl_col_count(0),
        stiff_hori(NULL), stiff_hori_row_count(0), stiff_hori_col_count(0),
        stiff_vert(NULL), stiff_vert_row_count(0), stiff_vert_col_count(0)
      {}
      /**Destructor*/
      ~PhysSystem(){
        if (part_list!=NULL) {
          delete [] part_list;
          part_list = NULL;
        }
        
        if (spring_hori!=NULL) {
          delete [] spring_hori;
          spring_hori = NULL;
        }

        if (spring_vert!=NULL) {
          delete [] spring_vert;
          spring_vert = NULL;
        }

        if (shear_tlbr!=NULL) {
          delete [] shear_tlbr;
          shear_tlbr = NULL;
        }

        if (shear_trbl!=NULL) {
          delete [] shear_trbl;
          shear_trbl = NULL;
        }

        if (stiff_hori!=NULL) {
          delete [] stiff_hori;
          stiff_hori = NULL;
        }

        if (stiff_vert!=NULL) {
          delete [] stiff_vert;
          stiff_vert = NULL;
        }
      }
      /**1. All particles accumilate gravity
       * 2. Spring, shear, stiff all add acceleration
       * 3. Spring, shear, stiff all constraint //in this order
       * 4. All particles do verlet integration
       * 
      */
      void timestep(float dT){
        // ball->ballMoving(BALL_TRANSLATION);

        

        accumGrav();
        accumWind(wind_x, wind_y);
        springAllAddA();
        shearAllAddA();
        stiffAllAddA();
        
        springAllConstraint();
        shearAllConstraint();
        stiffAllConstraint();

        collisionCheckList();

        // springAllConstraint();
        // shearAllConstraint();
        // stiffAllConstraint();

        partIntegrate(dT);

        
      }

      void drawAll(float r=0.0f, float g=0.0f, float b=0.0f){
        
        Vec3f p1;
        Vec3f p2;
        Vec3f p3;
        Vec3f normal;

        glColor3f(r,g,b);
        for (int i=0; i<part_row_count-1; i++){
          for (int j=0; j<part_col_count-1; j++){
            p1 = part_list[i*part_row_count + j].s;
            p2 = part_list[(i+1)*part_row_count + j].s;
            p3 = part_list[i*part_row_count + j+1].s;
            normal = Vec3f(p1,p2,p3).getUnit();
            glNormal3f(normal.x, normal.y, normal.z);
            glBegin(GL_TRIANGLES);
              glVertex3f(p1.x, p1.y, p1.z);
              glVertex3f(p2.x, p2.y, p2.z);
              glVertex3f(p3.x, p3.y, p3.z);
            glEnd();
          }
        }

        glColor3f(1.0f,1.0f,1.0f);
        for (int i=0; i<part_row_count-1; i++){
          for (int j=0; j<part_col_count-1; j++){
            p1 = part_list[i*part_row_count + j+1].s;
            p2 = part_list[(i+1)*part_row_count + j].s;
            p3 = part_list[(i+1)*part_row_count + j+1].s;
            normal = Vec3f(p1,p2,p3).getUnit();
            glNormal3f(normal.x, normal.y, normal.z);
            glBegin(GL_TRIANGLES);
              glVertex3f(p1.x, p1.y, p1.z);
              glVertex3f(p2.x, p2.y, p2.z);
              glVertex3f(p3.x, p3.y, p3.z);
            glEnd();
          }
        }

        glColor3f(1.0f-r, 1.0f-g, 1.0f-b);
        glPushMatrix();
            glTranslatef(ball->c.x, ball->c.y, ball->c.z);
            glutSolidSphere(ball->r, 50, 50);
        glPopMatrix();

      }

      /**Add Random acceleration to all particle*/
      void randA(){
        for (int i=0; i<part_row_count*part_col_count; i++){
          part_list[i].accumA(Vec3f(nextRand(), nextRand(), nextRand()));
        }
      }
  };

// **********************

// * GLOBAL variables and Objects.
  

  static int pause = TRUE;
  //timer start and prev time, in ms
  static int startTime = 0; 
  static int prevTime = 0;

  static BALL_DIR ball_direction = BALL_LEFT;

  SolidBall* global_ball = new SolidBall(Vec3f(BALL_POSITION_X, BALL_POSITION_Y, BALL_POSITION_Z), BALL_RADIUS);
  static PhysSystem* global_sys = new PhysSystem(Vec3f(PART_POSITION_X_1, PART_POSITION_Y, PART_POSITION_Z),
                                  Vec3f(PART_POSITION_X_2, PART_POSITION_Y, PART_POSITION_Z),
                                  global_ball
                                  );


  
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
  /** Keyboard callback for Control */
  void keyboard (unsigned char key, int x, int y);
  /** Arrow callback for Ball */
  void arrow_keys (int a_keys, int x, int y); 
  /** Mouse callback for wind*/
  void mouseFunc (int button, int state, int x, int y);

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
  glutMouseFunc (mouseFunc);
  // Start the timer
  glutTimerFunc(TIMERMSECS, animate, 0);

	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

  glutMainLoop ();
}

void init (void)
{
  srand((unsigned int)time(NULL));  //for usage of random
  
  glShadeModel (GL_SMOOTH);
  glClearColor (0.2f, 0.2f, 0.4f, 0.5f);				
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_COLOR_MATERIAL);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  GLfloat lightPos[4] = {-2.0, 1.0, 2.0, 0.0};
  // GLfloat lightPos[4] = {-0.0, 0.0, 0.0, 0.0};
  glLightfv (GL_LIGHT0, GL_POSITION, (GLfloat *) &lightPos);
  glEnable (GL_LIGHT1);
  GLfloat lightAmbient1[4] = {0.0, 0.0,  0.0, 0.0};
  GLfloat lightPos1[4]     = {1.0, 0.0, -0.2, 0.0};
  GLfloat lightDiffuse1[4] = {0.5, 0.5,  0.3, 0.0};
  glLightfv (GL_LIGHT1,GL_POSITION, (GLfloat *) &lightPos1);
  glLightfv (GL_LIGHT1,GL_AMBIENT, (GLfloat *) &lightAmbient1);
  glLightfv (GL_LIGHT1,GL_DIFFUSE, (GLfloat *) &lightDiffuse1);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

}

void display (void)
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  drawBackground();
  
  glEnable (GL_LIGHTING);

  global_sys->drawAll(1.0f, 0.0f, 0.0f);

  glutSwapBuffers();
  glutPostRedisplay();

  glFlush();
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
    case 'k':
      global_sys->randA();
    break;
    case 'r':
      delete global_sys;
      delete global_ball;
      global_ball = new SolidBall(Vec3f(BALL_POSITION_X, BALL_POSITION_Y, BALL_POSITION_Z), BALL_RADIUS);
      global_sys = new PhysSystem(Vec3f(PART_POSITION_X_1, PART_POSITION_Y, PART_POSITION_Z),
                        Vec3f(PART_POSITION_X_2, PART_POSITION_Y, PART_POSITION_Z),
                        global_ball
                        );
    break;
    case 'w':
      // global_ball->dir = BALL_FORW;
      global_ball->c = global_ball->c - Vec3f(0.0f, 0.0f, BALL_TRANSLATION);
    break;
    case 's':
      // global_ball->dir = BALL_BACK;
      global_ball->c = global_ball->c + Vec3f(0.0f, 0.0f, BALL_TRANSLATION);
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
      // global_ball->dir = BALL_UP;
      global_ball->c = global_ball->c + Vec3f(0.0f, BALL_TRANSLATION, 0.0f);
    break;
    case GLUT_KEY_DOWN: 
      // global_ball->dir = BALL_DOWN;
      global_ball->c = global_ball->c - Vec3f(0.0f, BALL_TRANSLATION, 0.0f);
    break;
    case GLUT_KEY_LEFT:
      // global_ball->dir = BALL_LEFT;
      global_ball->c = global_ball->c - Vec3f(BALL_TRANSLATION, 0.0f, 0.0f);
    break;
    case GLUT_KEY_RIGHT:
      // global_ball->dir = BALL_RIGHT;
      global_ball->c = global_ball->c + Vec3f(BALL_TRANSLATION, 0.0f, 0.0f);
    break;
    default:
    break;
  }
}

void mouseFunc (int button, int state, int x, int y)
{
  float screen_space_x_constant = (float)glutGet(GLUT_WINDOW_WIDTH)/42.0f;
  float screen_space_y_constant = (float)glutGet(GLUT_WINDOW_HEIGHT)/42.0f; //experimental value for screen to model space trasformation
  if (button == GLUT_LEFT_BUTTON) {
    wind_z_dir = -1.0f;
    if (state == GLUT_DOWN) {
      wind_x = ((float)x/(float)glutGet(GLUT_WINDOW_WIDTH) - 0.5f) * screen_space_x_constant;
      wind_y = (1 - (float)y/(float)glutGet(GLUT_WINDOW_HEIGHT) - 0.5f) * screen_space_y_constant;
      // printf("%f,%f; %d,%d\n", wind_x, wind_y, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    }
  }
  else if (button == GLUT_RIGHT_BUTTON) {
    wind_z_dir = 1.0f;
    if (state == GLUT_DOWN) {
      wind_x = ((float)x/(float)glutGet(GLUT_WINDOW_WIDTH) - 0.5f) * screen_space_x_constant;
      wind_y = (1 - (float)y/(float)glutGet(GLUT_WINDOW_HEIGHT) - 0.5f) * screen_space_y_constant;
      // printf("%f,%f; %d,%d\n", wind_x, wind_y, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    }
  }
}

void drawBackground(){
  glDisable (GL_LIGHTING);
  glBegin (GL_POLYGON); //Skyblue Background 
    glColor3f (0.8f, 0.8f, 1.0f);
    glVertex3f (-400.0f, -400.0f, -200.0f);
    glVertex3f (400.0f, -400.0f, -200.0f);
    glColor3f (0.4f, 0.4f, 0.8f);	
    glVertex3f (400.0f, 400.0f, -200.0f);
    glVertex3f (-400.0f, 400.0f, -200.0f);
  glEnd ();
}


void animate(int value){
  glutTimerFunc(TIMERMSECS, animate, 0);

  if (pause == FALSE){
    global_sys->timestep(TIMESTEP);
  }
  
	glutPostRedisplay();
}

