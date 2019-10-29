#ifndef CXOPENGLIPC_H
#define CXOPENGLIPC_H


#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>
#include <QImage>
#include <QWindow>
#include <QTimerEvent>
#include <QOpenGLPaintDevice>

class QOpenGLFramebufferObject;
class QOpenGLPaintDevice;

class CxOpenGLIPC : public QObject//, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    CxOpenGLIPC( const char *url );
    virtual ~CxOpenGLIPC();
public:
    int         width();
    int         height();
    void        setGLSize(int w, int h);
    void        setUpdateInterval( int ms );
private:
    void        createIPC();
protected:
    void        timerEvent(QTimerEvent *event);
protected:
    void        initContext();
protected:
    QOpenGLContext      _context;
    QWindow             window;
    int                 w, h;
    char                url[200];
    int                 sock;
public:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual bool paintGL() ;
};



#endif // CXOWNOPENGL_H
