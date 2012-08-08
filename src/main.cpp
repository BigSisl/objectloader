#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "Model_3DS.h"

#include <iostream>

using namespace std;

GLfloat angle = 0.0;

Model_3DS obj; //create object

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture ); 
}

void square (void) {
  //*obj.Materials->tex.texture = texture;
  glBindTexture( GL_TEXTURE_2D, *obj.Materials->tex.texture ); //bind our texture to our shape
  
  glTranslatef(0.0,0.0,-10.0f);
  //glRotatef(45, 1.0,1.0,1.0);
  /*
  glBegin(GL_QUADS);
    
  glNormal3f(0.0, 1.0f, 0.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-2.5f, -2.5f, 2.5f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(2.5f, -2.5f, 2.5f);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(2.5f, -2.5f, -2.5f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-2.5f, -2.5f, -2.5f);

  glEnd();**/
  
  
  obj.rot.x += 0.02;
  obj.rot.y += 0.02;
  
  obj.Draw();
}

void display (void) {
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); 
    glEnable( GL_TEXTURE_2D );
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    square();
    glutSwapBuffers();
    angle += 0.2;
}
void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode (GL_MODELVIEW);
}

int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutReshapeFunc (reshape);
    
    obj.Load("C:\\metoritewars\\objects\\player\\test.3ds");
    
    //enabling light object does not print right without light
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    
    glutMainLoop ();

    return 0;
}