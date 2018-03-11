#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif

int   cube_exploded = 0;
float angle = 0.0; // camera rotation angle

// Light sources


void display (void);
void keyboard (unsigned char, int, int);
void reshape (int, int);
void idle (void);
void explode_cube (void);
void init(void);

void init(void)
{
  GLfloat  light0Amb[4] =  { 1.0, 0.6, 0.2, 1.0 };
  GLfloat  light0Dif[4] =  { 1.0, 0.6, 0.2, 1.0 };   
  GLfloat  light0Spec[4] = { 0.0, 0.0, 0.0, 1.0 };   
  GLfloat  light0Pos[4] =  { 0.0, 0.0, 0.0, 1.0 };
  GLfloat  light1Amb[4] =  { 0.0, 0.0, 0.0, 1.0 };
  GLfloat  light1Dif[4] =  { 1.0, 1.0, 1.0, 1.0 };   
  GLfloat  light1Spec[4] = { 1.0, 1.0, 1.0, 1.0 };   
  GLfloat  light1Pos[4] =  { 0.0, 5.0, 5.0, 0.0 };

  glShadeModel(GL_SMOOTH);
  glClearColor (0.5f, 0.5f, 0.5f, 0.0f);
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_COLOR_MATERIAL);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glEnable (GL_LIGHT0);
  glEnable (GL_LIGHT1);
  glLightfv (GL_LIGHT0, GL_AMBIENT, light0Amb);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light0Dif);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light0Spec);
  glLightfv (GL_LIGHT0, GL_POSITION, light0Pos);
  glLightfv (GL_LIGHT1, GL_AMBIENT, light1Amb);
  glLightfv (GL_LIGHT1, GL_DIFFUSE, light1Dif);
  glLightfv (GL_LIGHT1, GL_SPECULAR, light1Spec);
  glLightfv (GL_LIGHT1, GL_POSITION, light1Pos);
  glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  // glEnable (GL_NORMALIZE);
}

void display (void)
{
  int i;

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();
  //
  // Place the camera
  glTranslatef (0.0, 0.0, -10.0);
  glRotatef (angle, 0.0, 1.0, 0.0);
  //
  // If no explosion, draw cube
  if (!cube_exploded)
  {
    glEnable (GL_LIGHTING);
    glDisable (GL_LIGHT0);
    glEnable (GL_DEPTH_TEST);
    glColor3f (1.0f, 0.0f, 0.0f);
    glutSolidCube (1.0);
  }
  glutSwapBuffers ();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key)
  {
    case ' ':
      explode_cube();
      exit (0);
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

  //loop
  glutMainLoop ();
  return 0;
}
