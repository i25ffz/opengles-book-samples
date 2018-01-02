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

    // Uniform locations
    GLint  mvpLoc;

    // Vertex daata
    GLfloat  *vertices;
    GLuint *indices;
    int       numIndices;

    // Rotation angle
    GLfloat   angle;

    // MVP matrix
    ESMatrix  mvpMatrix;
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
        "uniform mat4 modelMatrix;\n"
        "uniform mat4 modelViewMatrix;\n"
        "uniform mat4 projectionMatrix;\n"
        "attribute vec3 position;\n"
        "attribute vec2 uv;\n"
        "varying vec2 vUv;\n"
        "\n"
        "vec4 Universe_Shader1462567918635_91_main()\n"
        "{\n"
        "    vec4 Universe_Shader1462567918635_91_gl_Position = vec4(0.0);\n"
        "    vUv = uv;\n"
        "    Universe_Shader1462567918635_91_gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);\n"
        "    return Universe_Shader1462567918635_91_gl_Position *= 1.0;\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = Universe_Shader1462567918635_91_main();\n"
        "}\n";

    GLbyte fShaderStr[] =
        "precision highp float;\n"
        "precision highp int;\n"
        "uniform float swirl;\n"
        "uniform float time;\n"
        "uniform float speed;\n"
        "varying vec2 vUv;\n"
        "\n"
        "vec4 Universe_Shader1462567918635_91_main()\n"
        "{\n"
        "    vec4 Universe_Shader1462567918635_91_gl_FragColor = vec4(0.0);\n"
        "    vec2 uv = vUv - 0.5;\n"
        "    float time = time * .1 + ((.25 + .05 * sin(time * speed * 0.1)) / (length(uv.xy) + .07)) * 2.2;\n"
        "    float si = sin(time * swirl);\n"
        "    float co = cos(time * 1.0 / swirl);\n"
        "    mat2 ma = mat2(co, si, -si, co);\n"
        "    float c = 1.0;\n"
        "    float v1 = 0.0;\n"
        "    float v2 = 0.0;\n"
        "    for (int i = 0; i < 100; i++)\n"
        "    {\n"
        "        float s = float(i) * .035;\n"
        "        vec3 p = s * vec3(uv, 0.0);\n"
        "        p.xy *= ma;\n"
        "        p += vec3(.22, .3, s - 1.5 - sin(time * .13) * .1);\n"
        "        for (int i = 0; i < 8; i++)\n"
        "        {\n"
        "            p = abs(p) / dot(p, p) - 0.659;\n"
        "        }\n"
        "        v1 += dot(p, p) * .0015 * (1.8 + sin(length(uv.xy * 13.0) + .5 - time * .2));\n"
        "        v2 += dot(p, p) * .0015 * (1.5 + sin(length(uv.xy * 13.5) + 2.2 - time * .3));\n"
        "        c = length(p.xy * .5) * .35;\n"
        "    }\n"
        "\n"
        "    float len = length(uv);\n"
        "    v1 *= smoothstep(.7, .0, len);\n"
        "    v2 *= smoothstep(.6, .0, len);\n"
        "    float re = clamp(c, 0.0, 1.0);\n"
        "    float gr = clamp((v1 + c) * .25, 0.0, 1.0);\n"
        "    float bl = clamp(v2, 0.0, 1.0);\n"
        "    vec3 col = vec3(re, gr, bl) + smoothstep(0.15, .0, len) * .9;\n"
        "    Universe_Shader1462567918635_91_gl_FragColor = vec4(col, 1.0);\n"
        "    return Universe_Shader1462567918635_91_gl_FragColor *= 1.0;\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = (Universe_Shader1462567918635_91_main());\n"
        "}\n";

    // Load the shaders and get a linked program object
    userData->programObject = esLoadProgram ( vShaderStr, fShaderStr );

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation ( userData->programObject, "a_position" );

    // Get the uniform locations
    userData->mvpLoc = glGetUniformLocation( userData->programObject, "u_mvpMatrix" );

    // Generate the vertex data
    userData->numIndices = esGenCube( 1.0, &userData->vertices,
                                     NULL, NULL, &userData->indices );

    // Starting rotation angle for the cube
    userData->angle = 45.0f;

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
    return GL_TRUE;
}


///
// Update MVP matrix based on time
//
void Update ( ESContext *esContext, float deltaTime )
{
    UserData *userData = (UserData*) esContext->userData;
    ESMatrix perspective;
    ESMatrix modelview;
    float    aspect;

    // Compute a rotation angle based on time to rotate the cube
    userData->angle += ( deltaTime * 40.0f );
    if( userData->angle >= 360.0f )
      userData->angle -= 360.0f;

    // Compute the window aspect ratio
    aspect = (GLfloat) esContext->width / (GLfloat) esContext->height;

    // Generate a perspective matrix with a 60 degree FOV
    esMatrixLoadIdentity( &perspective );
    esPerspective( &perspective, 60.0f, aspect, 1.0f, 20.0f );

    // Generate a model view matrix to rotate/translate the cube
    esMatrixLoadIdentity( &modelview );

    // Translate away from the viewer
    esTranslate( &modelview, 0.0, 0.0, -2.0 );

    // Rotate the cube
    esRotate( &modelview, userData->angle, 1.0, 0.0, 1.0 );

    // Compute the final MVP by multiplying the
    // modevleiw and perspective matrices together
    esMatrixMultiply( &userData->mvpMatrix, &modelview, &perspective );
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

    glEnableVertexAttribArray ( userData->positionLoc );


    // Load the MVP matrix
    glUniformMatrix4fv( userData->mvpLoc, 1, GL_FALSE, (GLfloat*) &userData->mvpMatrix.m[0][0] );

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

    esCreateWindow ( &esContext, "Simple Texture 2D", 320, 240, ES_WINDOW_RGB );

    if ( !Init ( &esContext ) )
        return 0;

    esRegisterDrawFunc ( &esContext, Draw );
    esRegisterUpdateFunc ( &esContext, Update );

    esMainLoop ( &esContext );

    ShutDown ( &esContext );
}

