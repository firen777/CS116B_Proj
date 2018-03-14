#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif

//custom library
#include "aclib/vec3.h"
#include "aclib/aclib.h"

//Constants
#define PARTICLES_NUM 10000


//Global var:
int   cube_exploded = 0;
float angle = 0.0f; // camera rotation angle
float particle_color = 1.0f; // particle color
Vec3f particleS[PARTICLES_NUM]; //particle position S
Vec3f particleV[PARTICLES_NUM]; //particle velocity V

//function header
void display (void);
void keyboard (unsigned char, int, int);
void reshape (int, int);
void idle (void);
void explode_cube (void);
//init lighting
void init(void);
//init particles data and srand
void particleInit(void);
//RNGesus
float nextRand(void);
//draw spinning cube
void drawCube(void);
//draw Particles
void drawParticles(void);

void init(void)
{
  // Light sources
  // GLfloat  light0Amb[4] =  { 1.0, 0.6, 0.2, 1.0 };
  // GLfloat  light0Dif[4] =  { 1.0, 0.6, 0.2, 1.0 };   
  // GLfloat  light0Spec[4] = { 0.0, 0.0, 0.0, 1.0 };   
  // GLfloat  light0Pos[4] =  { 0.0, 0.0, 0.0, 1.0 };
  GLfloat  light1Amb[4] =  { 0.0, 0.0, 0.0, 1.0 };
  GLfloat  light1Dif[4] =  { 1.0, 1.0, 1.0, 1.0 };   
  GLfloat  light1Spec[4] = { 1.0, 1.0, 1.0, 1.0 };   
  GLfloat  light1Pos[4] =  { 0.0, 5.0, 5.0, 0.0 };

  glShadeModel(GL_SMOOTH);
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_COLOR_MATERIAL);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
  // glEnable (GL_LIGHT0);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT1);
  // glLightfv (GL_LIGHT0, GL_AMBIENT, light0Amb);
  // glLightfv (GL_LIGHT0, GL_DIFFUSE, light0Dif);
  // glLightfv (GL_LIGHT0, GL_SPECULAR, light0Spec);
  // glLightfv (GL_LIGHT0, GL_POSITION, light0Pos);
  glLightfv (GL_LIGHT1, GL_AMBIENT, light1Amb);
  glLightfv (GL_LIGHT1, GL_DIFFUSE, light1Dif);
  glLightfv (GL_LIGHT1, GL_SPECULAR, light1Spec);
  glLightfv (GL_LIGHT1, GL_POSITION, light1Pos);
  glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable (GL_NORMALIZE);
}

float nextRand(void)
{
  //0.00001f to 0.10001f
  float ans = (float)rand()/(float)(RAND_MAX/0.1f)+0.00001f;
  // 50% +, 50% -
  if (rand()%2 == 1)
    ans *= -1.0;
  return ans;
}

void particleInit(void)
{
  srand((unsigned int)time(NULL));

  for(int i=0; i<PARTICLES_NUM; i++) {
    particleS[i] = Vec3f();
    particleV[i] = Vec3f(nextRand(),nextRand(),nextRand());
  }
}

void drawCube(void)
{
  

  

  glRotatef (angle, 0.0, 1.0, 0.0);
  glColor3f (1.0f, 0.0f, 0.0f);
  glutSolidCube (1.0);



  
}

void drawParticles(void)
{
  
  if (particle_color > 0.0f)
  {
    glColor3f (particle_color, particle_color, 0.0f);
    for (int i=0; i<PARTICLES_NUM; i++)
    {
      glPushMatrix();
        glTranslatef (particleS[i].x, particleS[i].y, particleS[i].z);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLES);
          glVertex3f(0.01f, 0.0f, 0.0f);
          glVertex3f(0.01f, 0.01f, 0.0f);
          glVertex3f(0.0f, 0.01f, 0.0f);
        glEnd();

      glPopMatrix();
      particleS[i] = particleS[i] + particleV[i];
    }
  }
  
}

void display (void)
{
  int i;

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  glTranslatef (0.0, 0.0, -10.0);

  // If no explosion, draw cube
  if (!cube_exploded)
  {
    drawCube();
  } 
  else 
  {
    drawParticles();
  }
  glutSwapBuffers ();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
    case ' ':
      explode_cube();
      // exit (0);
    break;
    case 27:
      exit (0);
    break;
  }
}

void reshape (int w, int h)
{
  glViewport (0.0, 0.0, (GLfloat) w, (GLfloat) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, (GLfloat) w / (GLfloat) h, 0.1, 100.0);
  glMatrixMode (GL_MODELVIEW);
}

void idle (void)
{
  angle += 0.3;  /* Always continue to rotate the camera */
  if (cube_exploded)
    particle_color -= 0.005f;
  glutPostRedisplay ();
}

void explode_cube(void)
{
  cube_exploded = 1;
  printf ("BOOM!\n");
}




int main (int argc, char *argv[])
{
  glutInit (&argc, argv);
  glutInitWindowPosition (0, 0);
  glutInitWindowSize (1280, 1024);
  glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow ("Particle explosion");

  //callback functions
  glutKeyboardFunc (keyboard);
  glutIdleFunc (idle);
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);

  //init
  init();
  particleInit();

  //loop
  glutMainLoop ();
  return 0;
}
