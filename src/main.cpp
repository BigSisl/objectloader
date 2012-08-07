// Because it is so much simpler at the moment, I am going to show you how to use
// textures under Windows only, the only thing is the actual, The original loading
// code I was using had it’s limitations so I have reworked it to make it more general.
// But back to the actual texturing, because it is under windows we have to include
// the windows header file, then we also need to add the stdio header file.

// using GLuint we create a variable for the texture, called ‘texture’, but you can call it
// whatever you wish.

// in the ’display’ function we first set the variable ’texture’ to the actual
// loaded image using:
// texture = LoadTexture( ”texture.raw”, 256, 256 );
// where 256, 256 is the width and the height of the file respectively.
// then we enable the 2D Texturing, this is done with:
// glEnable( GL_TEXTURE_2D );
// and bind the texture with:
// glBindTexture( GL_TEXTURE_2D, texture );

// then after drawing everything we need with the texture, we clear it to save
// system resources.

// ——————————————————————————

// Section 1: The loading of the texture:

//GLuint LoadTexture( const char * filename, int width, int height )
//{
//    GLuint texture;
//    unsigned char * data;
//    FILE * file;

// The following code will read in our RAW file
//    file = fopen( filename, ”rb” ); // We need to open our file
//    if ( file == NULL ) return 0; // If our file is empty, set our texture to empty
//    data = (unsigned char *)malloc( width * height * 3 ); //assign the nessecary memory for the texture
//    fread( data, width * height * 3, 1, file ); // read in our file

//    fclose( file ); //close our file, no point leaving it open

//    glGenTextures( 1, &texture ); //then we need to tell OpenGL that we are generating a texture
//    glBindTexture( GL_TEXTURE_2D, texture ); //now we bind the texture that we are working with
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters
// The parameter GL_MODULATE will blend the texture with whatever is underneath, setting it to GL_DECAL
// will tell the texture to replace whatever is on the object.

//here we are setting what textures to use and when. The MIN filter is which quality to show
//when the texture is near the view, and the MAG filter is which quality to show when the texture
//is far from the view.

//The qualities are (in order from worst to best)
//GL_NEAREST
//GL_LINEAR
//GL_LINEAR_MIPMAP_NEAREST
//GL_LINEAR_MIPMAP_LINEAR

// The two mipmap variables only work in textures with generated mipmaps, so you will see that in action
// in a later tutorial.

//And if you go and use extensions, you can use Anisotropic filtering textures which are of an
//even better quality, but this will do for now.

//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//Here we are setting the parameter to repeat the texture instead of clamping the texture
//to the edge of our shape. 

//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

//Generate the texture
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//    free( data ); //free the texture
//    return texture; //return the texture data
//}

//——————————————————————————–

//Section 2: Cleaning Up:

//If we decide not to clean up after our work, you will find that the program will inevitably
//slow down your computer, taking more and more memory.
//void FreeTexture( GLuint texture )
//{
//  glDeleteTextures( 1, &texture ); // Delete our texture, simple enough.
//}

//——————————————————————————-

//Section 3: Texture Coordinates

//This is how texture coordinates are arranged
//
//  0,1   —–   1,1
//       |     |
//       |     |
//       |     |
//  0,0   —–   1,0

// With 0,0 being the bottom left and 1,1 being the top right.

// Now the point of using the value 0,1 instead of 0,10, is so that it is mapping 1 texture to the
// coordinates. Changing that to 10 would then try to map 10 textures to the one quad. Which because
// I have the repeat parameter set in our texture, it would draw 10 across and 10 down, if we had
// it clamped, we would be still drawing 1. The repeat function is good for things like
// brick walls.

// To assign texture coordinates to a vertex look at the following code:
//    glBegin (GL_QUADS);
//    glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0); //with our vertices we have to assign a texcoord
//  glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0); //so that our texture has some points to draw to
//  glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
//  glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
//    glEnd();

//For every vertex, we are assigning a texture coordinate that corresponds with it. Keep in mind
//that this does not mean that if a vertex is a -10, 20, the texture coordinates will be -10, 20. 
// They will infact be something along the lines of 0,1.

//———————————————————————

//So now you should have a rough idea of the basics associated with texturing.
//If you have any questions, feel free to email me at swiftless@gmail.com


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

GLuint texture; //the array for our texture

GLfloat angle = 0.0;

//function to load the RAW file

GLuint LoadTexture(char* filename){
  
  
  // Data read from the header of the BMP file
  unsigned char header[54]; // Each BMP file begins by a 54-bytes header
  unsigned int dataPos;     // Position in the file where the actual data begins
  unsigned int width, height;
  unsigned int imageSize;   // = width*height*3
  // Actual RGB data
  unsigned char * data;
  GLuint textureID;
    
  // Open the file
  FILE * file = fopen(filename,"rb");
  if (!file) {
    printf("file not found\n");
    return ERROR_FILE_INVALID;
  }
  
  if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
    printf("Not a correct BMP file\n");
    fclose(file);
    return ERROR_FILE_CORRUPT;
  }
  
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);
  
  // Some BMP files are misformatted, guess missing information
  if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
  if (dataPos==0)      dataPos=54; // The BMP header is done that way
  
  // Create a buffer
  data = new unsigned char [imageSize];

  // Read the actual data from the file into the buffer
  fread(data,1,imageSize,file);

  //Everything is in memory now, the file can be closed
  fclose(file);
  
  glGenTextures( 1, &textureID ); //generate the texture with the loaded data
  glBindTexture( GL_TEXTURE_2D, textureID ); //bind the texture to it’s array
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters

  //here we are setting what textures to use and when. The MIN filter is which quality to show
  //when the texture is near the view, and the MAG filter is which quality to show when the texture
  //is far from the view.

  //The qualities are (in order from worst to best)
  //GL_NEAREST
  //GL_LINEAR
  //GL_LINEAR_MIPMAP_NEAREST
  //GL_LINEAR_MIPMAP_LINEAR

  //And if you go and use extensions, you can use Anisotropic filtering textures which are of an
  //even better quality, but this will do for now.
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  //Here we are setting the parameter to repeat the texture instead of clamping the texture
  //to the edge of our shape. 
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  //Generate the texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
  cout << textureID << endl;
  
  return textureID;
}

void FreeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture ); 
}

void square (void) {
  glBindTexture( GL_TEXTURE_2D, texture ); //bind our texture to our shape
  glRotatef( angle, 1.0f, 1.0f, 1.0f );
  glBegin (GL_QUADS);
  glTexCoord2d(0.0,0.0); glVertex2d(-1.0,-1.0); //with our vertices we have to assign a texcoord
  glTexCoord2d(1.0,0.0); glVertex2d(+1.0,-1.0); //so that our texture has some points to draw to
  glTexCoord2d(1.0,1.0); glVertex2d(+1.0,+1.0);
  glTexCoord2d(0.0,1.0); glVertex2d(-1.0,+1.0);
  glEnd();

  //This is how texture coordinates are arranged
  //
  //  0,1   —–   1,1
  //       |     |
  //       |     |
  //       |     |
  //  0,0   —–   1,0

  // With 0,0 being the bottom left and 1,1 being the top right.
  // Now the point of using the value 0,1 instead of 0,10, is so that it is mapping 1 texture to the
  // coordinates. Changing that to 10 would then try to map 10 textures to the one quad. Which because
  // I have the repeat parameter set in our texture, it would draw 10 across and 10 down, if we had
  // it clamped, we would be still drawing 1. The repeat function is good for things like
  // brick walls.
}

void display (void) {
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT);
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
    glutInitDisplayMode (GLUT_DOUBLE);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutReshapeFunc (reshape);
    
    //Load our texture
    texture = LoadTexture( "C:\\metoritewars\\objects\\player\\road.bmp");
    
    cout << texture << endl;
    
    glutMainLoop ();

    //Free our texture
    FreeTexture( texture );

    return 0;
}