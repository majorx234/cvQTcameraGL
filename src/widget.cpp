/* Copyright (C) 2013 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#include "widget.h"
 #include "textfile.h"

#include <iostream>
#include <QKeyEvent>
#include <QTimer>
#include <QString>

// forward declaration debug functions
#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char *file, int line); 
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

GLWidget::GLWidget(QWidget *parent)
: QGLWidget(parent)
{
    _width = 0;
    _height = 0;
    _texture = 0;
    _fps = 0;
}

GLWidget::~GLWidget()
{
    glDeleteTextures(1, &_texture);
}

void GLWidget::_tick()
{
    // triggers paintGL()
    update();

    // Set timer according to FPS
    QTimer::singleShot(1000/_fps, this, SLOT(_tick()));
}

void GLWidget::initializeGL()
{
    // Set clear color as black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Select pixel storage mode used by glTexImage2D
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // Create the texture
    glGenTextures(1, &_texture);

    /* Open default camera device */

    cv_capture.open(0);
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::initializeGL: !!! Failed to open camera" << std::endl;
        return;
    }

    // Retrieve FPS from the camera
    _fps = cv_capture.get(CV_CAP_PROP_FPS);
    if (!_fps) // if the function fails, fps is set to 15
        _fps = 15;

    setShaders();

    std::cout << "GLWidget::initializeGL: " << _fps << " fps" << std::endl;

    /* Start the timer */

    _tick();
}

void GLWidget::paintGL()
{
    // Clear the screen and depth buffer (with black)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Select the model view matrix and reset it
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, 0.0f);

    // Abort drawing if OpenCV was unable to open the camera
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to open camera" << std::endl;
        return;
    }

    // Note: trying to retrieve more frames than the camera can give you
    // will make the output video blink a lot.
    cv_capture >> cv_frame;
    if (cv_frame.empty())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to retrieve frame" << std::endl;
        return;
    }
    cv::cvtColor(cv_frame, cv_frame, CV_BGR2RGBA);




    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Transfer image data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_RGBA8, cv_frame.cols, cv_frame.rows, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, cv_frame.data);
    if (glGetError() != GL_NO_ERROR)
    {
        std::cout << "GLWidget::paintGL: !!! Failed glTexImage2D" << std::endl;
    }

    glDisable(GL_TEXTURE_2D);




    // Typical texture generation using data from the bitmap
    glBindTexture(GL_TEXTURE_2D, _texture);


    GLint tex_loc;

    tex_loc = glGetUniformLocation(p, "pass_1_tex");
    glUniform1i(tex_loc, GL_TEXTURE0);


    // Invoke glUseProgram() to activate your GLSL shader;
    glUseProgram(p);
    GLint time_loc = glGetUniformLocation(p, "iGlobalTime");
    GLint resolution_loc = glGetUniformLocation(p, "iResolution");
    glUniform1f(time_loc, 0.95f);
    glUniform2f(resolution_loc, _width, _height);

    // Draw a 2D face with texture
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0);                         glVertex2f(2, 1.5);
        glTexCoord2f(cv_frame.cols, 0);             glVertex2f(-2, 1.5);
        glTexCoord2f(cv_frame.cols, cv_frame.rows); glVertex2f(-2, -1.5);
        glTexCoord2f(0, cv_frame.rows);             glVertex2f(2, -1.5);
    glEnd();

    //Disable the shader program
    glUseProgram(0);

    //Disable the texture target 
    //glDisable(GL_TEXTURE_2D);
}

void GLWidget::resizeGL( int w, int h)
{

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;
    if(w == 0)
        w = 1;


    _width = w;
    _height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);  // Select the projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (h == 0)  // Calculate aspect ratio of the window
       gluPerspective(60, (float) w, 1.0, 50.0);
    else
       gluPerspective(60, (float) w / (float) h, 1.0, 50.0);

    gluLookAt(0.0,  0.0, 2.0,   // eye
              0.0,  0.0, 0.0,   // center
              0.0,  1.0, 0.0);  // up

    glMatrixMode(GL_MODELVIEW);  // Select the model view matrix
    glLoadIdentity();           // Reset the model view matrix
}

void GLWidget::setShaders() {
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }
    char *vs = NULL,*fs = NULL,*fs2 = NULL;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    f2 = glCreateShader(GL_FRAGMENT_SHADER);

    char* vs_path = "shader/toonf2.vert";
    char* fs_path = "shader/toonf2.frag";
    vs = textFileRead(vs_path);
    fs = textFileRead(fs_path);

    char * vv = vs;
    char * ff = fs;
    
    //std::string 
    //std::string
    
    //qDebug("%d", strlen(vv));
    //printf("source: %s\n\n", vv);
    glShaderSource(v, 1, &vv,NULL);
    glShaderSource(f, 1, &ff,NULL);

    free(vs);free(fs);

    glCompileShader(v);
    glCompileShader(f);

    printShaderInfoLog(v);
    printShaderInfoLog(f);
    printShaderInfoLog(f2);

    p = glCreateProgram();
    glAttachShader(p,v);
    glAttachShader(p,f);

    glLinkProgram(p);
    printProgramInfoLog(p); 

}


/**************** Debug Functions ***************/

int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}
