/* Copyright (C) 2013 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 */
#pragma once
 #ifndef __APPLE__
 //   #include <GL/glu.h>
    #include <GL/glew.h>
    #include <GL/glut.h>
#else
    #include <glu.h>
#endif
#include <opencv2/opencv.hpp>
#include <QGLWidget>
#include <QImage>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget* parent = 0);
    virtual ~GLWidget();

    /* OpenGL initialization, viewport resizing, and painting */

    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height);
    void setShaders();
private:
    int _width;
    int _height;
    cv::Mat cv_frame;
    cv::VideoCapture cv_capture;
    int _fps;
    GLuint  _texture;
    GLint loc;
    GLuint v,f,f2,p;

protected slots:
    void _tick();
};
