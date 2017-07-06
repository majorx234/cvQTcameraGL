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
    //_texture = 0;
    _fps = 0;
}

GLWidget::~GLWidget() {

    glDeleteTextures(1, &depth_tex);

    glDeleteTextures(1, &webcam_tex);
    glDeleteTextures(1, &noise_tex);

    glDeleteTextures(1, &pass_tex[0]);
    glDeleteTextures(1, &pass_tex[1]);
    glDeleteTextures(1, &pass_tex[2]);
    glDeleteTextures(1, &pass_tex[3]);


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
    int camera_num;
    std::string shader_path;

    if (!(std::cin >> camera_num)) // do the conversion
        camera_num = 0; // if conversion fails, set myint to a default value
    std::cout << "Using camera_num: " << camera_num << '\n';

    if (!(std::cin >> shader_path)) // do the conversion
        shader_path = "error"; // if conversion fails, set myint to a default value
    std::cout << "Using shader_path: " << shader_path << '\n';

    glewInit();
    // Set clear color as violet to be able to spot a broken shader
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    // Select pixel storage mode used by glTexImage2D
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_TEXTURE_2D);

    // Create the texture
    glGenTextures(1, &webcam_tex);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, webcam_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 32, 32, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &noise_tex);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, noise_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 32, 32, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);

    std::cout << "Starting the creation of fb" << std::endl;






    std::cout << "Opening the camera" << std::endl;

    /* Open default camera device */
    cv_capture.open(camera_num);
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::initializeGL: !!! Failed to open camera" << std::endl;
        _fps = 30;
        //return;
    } else {

        // Retrieve FPS from the camera
        _fps = cv_capture.get(CV_CAP_PROP_FPS);
        if (!_fps) // if the function fails, fps is set to 15
            _fps = 15;
    }

    std::cout << "Preparing the shaders" << std::endl;
    setShaders(shader_path);

    std::cout << "GLWidget::initializeGL: " << _fps << " fps" << std::endl;

    /* Start the timer */

    _tick();
}

void GLWidget::paintGL()
{
    bool image_captured = false;
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    // Abort drawing if OpenCV was unable to open the camera
    if (!cv_capture.isOpened())
    {
        std::cout << "GLWidget::paintGL: !!! Failed to open camera" << std::endl;
        //return;
    } else {

        // Note: trying to retrieve more frames than the camera can give you
        // will make the output video blink a lot.
        cv_capture >> cv_frame;
        if (cv_frame.empty())
        {
            std::cout << "GLWidget::paintGL: !!! Failed to retrieve frame" << std::endl;
            //return;
        } else {
            cv::cvtColor(cv_frame, cv_frame, CV_BGR2RGBA);

            glActiveTextureARB(GL_TEXTURE4);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, webcam_tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

            // Transfer image data to the GPU
            if (!cv_frame.empty()) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cv_frame.cols, cv_frame.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, cv_frame.data);
                image_captured = true;
            }
        }
    }
    glEnable(GL_TEXTURE_2D);


    for (int i=0; i<4; i++) {
        bool render_to_backbuffer = false;

        if (i == PASS_COUNT - 1) {
            render_to_backbuffer = true;
        }


        for (int i = 0; i < PASS_COUNT; i++) {
            glActiveTextureARB(GL_TEXTURE0 + i);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, pass_tex[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        }



        glActiveTextureARB(GL_TEXTURE4);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, webcam_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);



        GLenum error_code = glGetError();

        if (glGetError() != GL_NO_ERROR)
        {
            std::cout << "GLWidget::paintGL: !!! Failed glTexImage2D with errorcode: " << error_code << " shouldn't be : " << GL_NO_ERROR << std::endl;
        }


        glActiveTextureARB(GL_TEXTURE5);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, noise_tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);




        GLint p = pass_p[i];
        GLint fs = pass_fs[i];

        // Invoke glUseProgram() to activate your GLSL shader;
        glUseProgram(p);

        GLint tex_loc;

        tex_loc = glGetUniformLocation(p, "pass_1_tex");
        if(tex_loc != -1) glUniform1i(tex_loc, 0);


        tex_loc = glGetUniformLocation(p, "pass_2_tex");
        if(tex_loc != -1) glUniform1i(tex_loc, 1);


        tex_loc = glGetUniformLocation(p, "pass_3_tex");
        if(tex_loc != -1) glUniform1i(tex_loc, 2);


        tex_loc = glGetUniformLocation(p, "pass_final_tex");
        if(tex_loc != -1) glUniform1i(tex_loc, 3);


        tex_loc = glGetUniformLocation(p, "webcam_tex");
        if(tex_loc != -1) glUniform1i(tex_loc, 4);


        tex_loc = glGetUniformLocation(p, "noise_tex");
        if(tex_loc != -1) glUniform1i(tex_loc, 5);


        GLint time_loc = glGetUniformLocation(p, "iGlobalTime");
        GLint resolution_loc = glGetUniformLocation(p, "iResolution");
        glUniform1f(time_loc, time / 1000.0);
        time += 1000 / _fps / 2;
        glUniform2f(resolution_loc, _width, _height);


        if (render_to_backbuffer) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        } else {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pass_fbo[i]);

            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, DrawBuffers);
        }



        // Clear the screen and depth buffer (with black)
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Select the model view matrix and reset it
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0.0f, 0.0f, 0.0f);

        // Draw a 2D face with texture
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);                         glVertex2f(2, 1.5);
        glTexCoord2f(cv_frame.cols, 0);             glVertex2f(-2, 1.5);
        glTexCoord2f(cv_frame.cols, cv_frame.rows); glVertex2f(-2, -1.5);
        glTexCoord2f(0, cv_frame.rows);             glVertex2f(2, -1.5);
        glEnd();

        //Disable the shader program
        glUseProgram(0);


        for (int i = 0; i < PASS_COUNT; i++) {
            glActiveTextureARB(GL_TEXTURE0 + i);
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glActiveTextureARB(GL_TEXTURE4);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTextureARB(GL_TEXTURE5);
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

    }
    //Disable the texture target 
    glDisable(GL_TEXTURE_2D);
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



    // Creating each pass texture
    for (int i = 0; i < PASS_COUNT; i++) {
        glGenTextures(1, &pass_tex[i]);
        glActiveTextureARB(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, pass_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, _width, _height, 0, GL_BGRA, GL_FLOAT, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        glGenFramebuffersEXT(1, &pass_fbo[i]);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pass_fbo[i]);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, pass_tex[i], 0);

    }
}

void GLWidget::setShaders(std::string shader_path) {
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }
    char *fs1 = NULL, *fs2 = NULL, *fs3 = NULL, *fs4 = NULL, *vs = NULL;

    pass_fs[0] = glCreateShader(GL_FRAGMENT_SHADER);
    pass_fs[1] = glCreateShader(GL_FRAGMENT_SHADER);
    pass_fs[2] = glCreateShader(GL_FRAGMENT_SHADER);
    pass_fs[3] = glCreateShader(GL_FRAGMENT_SHADER);
    common_vs = glCreateShader(GL_VERTEX_SHADER);

    std::string fs_path1 = "shader/" + shader_path + "/pass_1.fs";
    std::string fs_path2 = "shader/" + shader_path + "/pass_2.fs";
    std::string fs_path3 = "shader/" + shader_path + "/pass_3.fs";
    std::string fs_path4 = "shader/" + shader_path + "/pass_final.fs";
    char* vs_path = "shader/toonf2.vert";
    fs1 = textFileRead(fs_path1.c_str());
    fs2 = textFileRead(fs_path2.c_str());
    fs3 = textFileRead(fs_path3.c_str());
    fs4 = textFileRead(fs_path4.c_str());
    vs = textFileRead(vs_path);

    
    //std::string 
    //std::string
    
    //qDebug("%d", strlen(vv));
    //printf("source: %s\n\n", vv);
    glShaderSource(pass_fs[0], 1, &fs1, NULL);
    glShaderSource(pass_fs[1], 1, &fs2, NULL);
    glShaderSource(pass_fs[2], 1, &fs3, NULL);
    glShaderSource(pass_fs[3], 1, &fs4, NULL);
    glShaderSource(common_vs, 1, &vs, NULL);

    free(fs1);
    free(fs2);
    free(fs3);
    free(fs4);
    free(vs);

    glCompileShader(pass_fs[0]);
    glCompileShader(pass_fs[1]);
    glCompileShader(pass_fs[2]);
    glCompileShader(pass_fs[3]);
    glCompileShader(common_vs);

    std::cout << "1" << std::endl;
    printShaderInfoLog(pass_fs[0]);

    std::cout << "2" << std::endl;
    printShaderInfoLog(pass_fs[1]);

    std::cout << "3" << std::endl;
    printShaderInfoLog(pass_fs[2]);

    std::cout << "4" << std::endl;
    printShaderInfoLog(pass_fs[3]);

    std::cout << "vs" << std::endl;
    printShaderInfoLog(common_vs);

    
    pass_p[0] = glCreateProgram();
    pass_p[1] = glCreateProgram();
    pass_p[2] = glCreateProgram();
    pass_p[3] = glCreateProgram();

    glAttachShader(pass_p[0], common_vs);
    glAttachShader(pass_p[0], pass_fs[0]);

    glAttachShader(pass_p[1], common_vs);
    glAttachShader(pass_p[1], pass_fs[1]);

    glAttachShader(pass_p[2], common_vs);
    glAttachShader(pass_p[2], pass_fs[2]);

    glAttachShader(pass_p[3], common_vs);
    glAttachShader(pass_p[3], pass_fs[3]);



    glLinkProgram(pass_p[0]);
    glLinkProgram(pass_p[1]);
    glLinkProgram(pass_p[2]);
    glLinkProgram(pass_p[3]);


    std::cout << "1" << std::endl;
    printProgramInfoLog(pass_p[0]);

    std::cout << "2" << std::endl;
    printProgramInfoLog(pass_p[1]);
 
    std::cout << "3" << std::endl;
    printProgramInfoLog(pass_p[2]);

    std::cout << "4" << std::endl;
    printProgramInfoLog(pass_p[3]);

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
