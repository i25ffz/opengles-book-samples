//
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com
//

// Simple_VertexShader.c
//
//    This is a simple example that draws a rotating cube in perspective
//    using a vertex shader to transform the object
//
#include <stdlib.h>
#include "esUtil.h"

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Attribute locations
   GLint  positionLoc;
   GLint  normalLoc;

   // Uniform locations
   GLint  mvLoc;
   GLint  projLoc;
   GLint  lightPosLoc;
   GLint  lightColorLoc;
   GLint  matColorLoc;

   // Vertex daata
   GLfloat  *vertices;
   GLfloat  *normals;
   GLuint *indices;
   int       numIndices;

   // Rotation angle
   GLfloat   angle;

   // MVP matrix
   // ESMatrix  mvpMatrix;
   ESMatrix  mvMatrix;
   ESMatrix  projMatrix;
} UserData;

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
   esContext->userData = malloc(sizeof(UserData));

   UserData *userData = esContext->userData;

   GLbyte vShaderStr[] =
        "precision highp float;\n"
        "precision highp int;\n"
        "\n"
        "uniform mat4 u_projectionMatrix;\n"
        "uniform mat4 u_modelViewMatrix;\n"
        "attribute vec3 a_position;\n"
        "attribute vec3 a_normal;\n"
        "varying vec3 vNormal;\n"
        "varying vec3 vPosition;\n"
        "\n"
        "void main() \n"
        "{\n"
        "  vNormal = a_normal;\n"
        "  vPosition = a_position;\n"
        "  \n"
        "  gl_Position = u_projectionMatrix * u_modelViewMatrix * vec4(a_position, 1.0);\n"
        "}";

   GLbyte fShaderStr[] =
        "precision highp float;\n"
        "\n"
        "varying vec3 vNormal;\n"
        "varying vec3 vPosition;\n"
        "\n"
        "uniform mat4 u_modelViewMatrix;    \n"
        "uniform vec3 u_lightPosition;\n"
        "uniform vec3 u_lightColor;\n"
        "uniform vec3 u_materialColor;\n"
        "\n"
        "void main(void) {\n"
        "    float ToonThresholds[4];\n"
        "    ToonThresholds[0] = 0.97;\n"
        "    ToonThresholds[1] = 0.7;\n"
        "    ToonThresholds[2] = 0.2;\n"
        "    ToonThresholds[3] = 0.03;\n"
        "\n"
        "    float ToonBrightnessLevels[5];\n"
        "    ToonBrightnessLevels[0] = 1.0;\n"
        "    ToonBrightnessLevels[1] = 0.06;\n"
        "    ToonBrightnessLevels[2] = 0.01;\n"
        "    ToonBrightnessLevels[3] = 0.35;\n"
        "    ToonBrightnessLevels[4] = 0.45;\n"
        "    \n"
        "    // Light\n"
        "    vec3 lightVectorW = normalize(vec3(vec4( u_lightPosition, 1.0) * u_modelViewMatrix) - vPosition);\n"
        "    \n"
        "    // diffuse\n"
        "    float ndl = max(0.0, dot(vNormal, lightVectorW));\n"
        "    \n"
        "    vec3 color = u_materialColor;\n"
        "    \n"
        "    if (ndl > ToonThresholds[0]) {\n"
        "        color += u_lightColor * ToonBrightnessLevels[0];\n"
        "    } else if (ndl > ToonThresholds[1])  {\n"
        "        color += u_lightColor * ToonBrightnessLevels[1];\n"
        "    } \n"
        "    \n"
        "    if ( ndl < ToonThresholds[3]) {\n"
        "        color -= u_lightColor * ToonBrightnessLevels[3];\n"
        "    } else if ( ndl < ToonThresholds[2] ) {\n"
        "        color -= u_lightColor * ToonBrightnessLevels[2];\n"
        "    }\n"
        "    \n"
        "    gl_FragColor = vec4( color, 1.0 );\n"
        "}";

   // Load the shaders and get a linked program object
   userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

   // Get the attribute locations
   userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );
   userData->normalLoc = glGetAttribLocation ( userData->programObject, "a_normal" );

   // Get the uniform locations
   userData->mvLoc = glGetUniformLocation( userData->programObject, "u_modelViewMatrix" );
   userData->projLoc = glGetUniformLocation( userData->programObject, "u_projectionMatrix" );
   userData->lightPosLoc = glGetUniformLocation( userData->programObject, "u_lightPosition" );
   userData->lightColorLoc = glGetUniformLocation( userData->programObject, "u_lightColor" );
   userData->matColorLoc = glGetUniformLocation( userData->programObject, "u_materialColor" );

   // Generate the vertex data
   userData->numIndices = esGenCube( 1.0, &userData->vertices,
                                     &userData->normals, NULL, &userData->indices );

   // Starting rotation angle for the cube
   userData->angle = 45.0f;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   glEnable(GL_CULL_FACE);
   return GL_TRUE;
}


///
// Update MVP matrix based on time
//
void Update ( ESContext *esContext, float deltaTime )
{
   UserData *userData = (UserData*) esContext->userData;
   // ESMatrix perspective;
   // ESMatrix modelview;
   float    aspect;

   // Compute a rotation angle based on time to rotate the cube
   userData->angle += ( deltaTime * 40.0f );
   if( userData->angle >= 360.0f )
      userData->angle -= 360.0f;

   // Compute the window aspect ratio
   aspect = (GLfloat) esContext->width / (GLfloat) esContext->height;

   // Generate a perspective matrix with a 60 degree FOV
   esMatrixLoadIdentity( &userData->projMatrix );
   esPerspective( &userData->projMatrix, 60.0f, aspect, 0.1f, 100.0f );

   // Generate a model view matrix to rotate/translate the cube
   esMatrixLoadIdentity( &userData->mvMatrix );

   // Translate away from the viewer
   esTranslate( &userData->mvMatrix, 0.0, 0.0, -2.0 );

   // Rotate the cube
   esRotate( &userData->mvMatrix, userData->angle, 1.0, 0.0, 1.0 );

   // Compute the final MVP by multiplying the
   // modevleiw and perspective matrices together
   // esMatrixMultiply( &userData->mvpMatrix, &modelview, &perspective );
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( userData->programObject );

   // Load the vertex position
   glVertexAttribPointer ( userData->positionLoc, 3, GL_FLOAT,
                           GL_FALSE, 3 * sizeof(GLfloat), userData->vertices );
   glVertexAttribPointer ( userData->normalLoc, 3, GL_FLOAT,
                           GL_FALSE, 3 * sizeof(GLfloat), userData->normals);

   glEnableVertexAttribArray ( userData->positionLoc );
   glEnableVertexAttribArray ( userData->normalLoc );

   // Load the MVP matrix
   glUniformMatrix4fv( userData->mvLoc, 1, GL_FALSE, (GLfloat*) &userData->mvMatrix.m[0][0] );
   glUniformMatrix4fv( userData->projLoc, 1, GL_FALSE, (GLfloat*) &userData->projMatrix.m[0][0] );

   glUniform3f( userData->lightPosLoc, 1.5f, -0.5f, 1.2f );
   glUniform3f( userData->lightColorLoc, 1.0f, 1.0f, 1.0f );
   glUniform3f( userData->matColorLoc, 0.5f, 0.5f, 0.3f );

   // Draw the cube
   glDrawElements ( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices );
}

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
   UserData *userData = esContext->userData;

   if ( userData->vertices != NULL )
   {
      free ( userData->vertices );
   }

   if ( userData->indices != NULL )
   {
      free ( userData->indices );
   }

   // Delete program object
   glDeleteProgram ( userData->programObject );

   free(userData);
}

int main ( int argc, char *argv[] )
{
   ESContext esContext;
   UserData  userData;

   esInitContext ( &esContext );
   esContext.userData = &userData;

   esCreateWindow ( &esContext, "Toon Light", 640, 480, ES_WINDOW_RGB );

   if ( !Init ( &esContext ) )
      return 0;

   esRegisterDrawFunc ( &esContext, Draw );
   esRegisterUpdateFunc ( &esContext, Update );

   esMainLoop ( &esContext );

   ShutDown ( &esContext );
}

