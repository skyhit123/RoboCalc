//============================================================================
// Name        : qt_opengl.cpp
// Author      : Wei Li
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include "GLWidget.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    GLWidget window;
    window.resize(800,600);
    window.show();

    return app.exec();
}
