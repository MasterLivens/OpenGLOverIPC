#ifndef CXOWNOPENGL_H
#define CXOWNOPENGL_H


#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>
#include <QImage>
#include <QWindow>
class Bubble;
class MainWindow;

QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)
QT_FORWARD_DECLARE_CLASS(QOpenGLShader)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
class QOpenGLFramebufferObject;
class QOpenGLPaintDevice;

class CxOwnOpenGL : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    CxOwnOpenGL(const QColor &background);
    ~CxOwnOpenGL();
public:
    void  Paint();
    int   width();
    int   height();
protected:
    void InitContex();
private:
    QOpenGLContext context;
    QWindow window;

    QOpenGLFramebufferObject *fbo;
    QOpenGLPaintDevice *device;
public slots:
    void setScaling(int scale);
    void setLogo();
    void setTexture();
    void setShowBubbles(bool);


protected:
    void resizeGL(int w, int h) ;
    void initializeGL() ;
public:
    QImage paintGL() ;
private:
    void paintTexturedCube();
    void paintQtLogo();
    void createGeometry();
    void createBubbles(int number);
    void quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4);
    void extrude(qreal x1, qreal y1, qreal x2, qreal y2);

    qreal m_fAngle;
    qreal m_fScale;
    bool m_showBubbles;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    bool m_qtLogo;
    QList<Bubble *> m_bubbles;
    int m_frames;
    QTime m_time;
    QOpenGLShader *m_vshader1;
    QOpenGLShader *m_fshader1;
    QOpenGLShader *m_vshader2;
    QOpenGLShader *m_fshader2;
    QOpenGLShaderProgram *m_program1;
    QOpenGLShaderProgram *m_program2;
    QOpenGLTexture *m_texture;
    QOpenGLBuffer m_vbo1;
    QOpenGLBuffer m_vbo2;
    int m_vertexAttr1;
    int m_normalAttr1;
    int m_matrixUniform1;
    int m_vertexAttr2;
    int m_normalAttr2;
    int m_texCoordAttr2;
    int m_matrixUniform2;
    int m_textureUniform2;
    bool m_transparent;
    bool m_hasButton;
    QColor m_background;
};



#endif // CXOWNOPENGL_H
