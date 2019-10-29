
#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <math.h>
#include "CxOpenGLIPC.h"
#include <nanomsg/nn.h>
#include <nanomsg/pair.h>


CxOpenGLIPC::CxOpenGLIPC(const char* url)
{
    initContext();
    //initializeOpenGLFunctions();
    //initializeGL(); //why doesn't it call subclass
    createIPC();
    w = h = 400;
    this->startTimer(1000);
    strcpy(this->url,url);
}

void    CxOpenGLIPC::createIPC()
{
    if ((sock = nn_socket(AF_SP, NN_PAIR)) < 0) {
            qDebug("nn_socket");
    }
    if (nn_bind(sock, "ipc:///tmp/pair.ipc") < 0) {
            qDebug("nn_connect");
    }
    int to = 100;
    if (nn_setsockopt(sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to,
        sizeof (to)) < 0) {
            qDebug("nn_setsockopt");
    }
}

CxOpenGLIPC::~CxOpenGLIPC()
{
}


void CxOpenGLIPC::initializeGL()
{

}

bool CxOpenGLIPC::paintGL()
{
    return 0;
}

void CxOpenGLIPC::resizeGL(int , int )
{
}

int    CxOpenGLIPC::width()
{
    return w;
}

int    CxOpenGLIPC::height()
{
    return h;
}

void   CxOpenGLIPC::setGLSize(int w, int h)
{
    this->w = w;
    this->h = h;

    this->resizeGL(w,h);
}

void   CxOpenGLIPC::setUpdateInterval( int ms )
{
    this->startTimer(ms);
}

void   CxOpenGLIPC::timerEvent(QTimerEvent *)
{

    if(!_context.makeCurrent(&window))
        return;

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(2);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject fbo(QSize(width(),height()), fboFormat);
    fbo.bind();

    bool ret = this->paintGL();
    if( ret )
    {
        QImage targetImage = fbo.toImage();
        nn_send(sock, targetImage.bits(), targetImage.bytesPerLine()*targetImage.height(),0);
    }

    fbo.release();
}

void CxOpenGLIPC::initContext()
{

    QSurfaceFormat format;
    format.setMajorVersion(2);
    format.setMinorVersion(0);

    window.setSurfaceType(QWindow::OpenGLSurface);
    window.setFormat(format);
    window.create();

    _context.setFormat(format);
    if (!_context.create())
        qFatal("Cannot create the requested OpenGL context!");

    _context.makeCurrent(&window);
    //const QRect drawRect(0, 0, width(), height());
    //const QSize drawRectSize = drawRect.size();

    //this->setGLSize(drawRectSize.width(),drawRectSize.height());

}

