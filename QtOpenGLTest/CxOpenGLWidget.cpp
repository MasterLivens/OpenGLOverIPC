
#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <math.h>
#include "bubble.h"
#include "cxownopengl.h"

const int bubbleNum = 8;

CxOwnOpenGL::CxOwnOpenGL( const QColor &background):m_showBubbles(true),
      m_qtLogo(true),
      m_frames(0),
      m_program1(0),
      m_program2(0),
      m_texture(0),
      m_transparent(false),
      m_background(background)
{
    InitContex();
    this->initializeGL();
}

CxOwnOpenGL::~CxOwnOpenGL()
{
    // And now release all OpenGL resources.
    this->context.makeCurrent(&window);
    delete m_texture;
    delete m_program1;
    delete m_program2;
    delete m_vshader1;
    delete m_fshader1;
    delete m_vshader2;
    delete m_fshader2;
    m_vbo1.destroy();
    m_vbo2.destroy();
    this->context.doneCurrent();
}

void CxOwnOpenGL::setScaling(int scale)
{
    if (scale > 30)
        m_fScale = 1 + qreal(scale - 30) / 30 * 0.25;
    else if (scale < 30)
        m_fScale =  1 - (qreal(30 - scale) / 30 * 0.25);
    else
        m_fScale = 1;
}

void CxOwnOpenGL::setLogo()
{
    m_qtLogo = true;
}

void CxOwnOpenGL::setTexture()
{
    m_qtLogo = false;
}

void CxOwnOpenGL::setShowBubbles(bool bubbles)
{
    m_showBubbles = bubbles;
}

void CxOwnOpenGL::paintQtLogo()
{
    m_program1->enableAttributeArray(m_vertexAttr1);
    m_program1->enableAttributeArray(m_normalAttr1);

    m_vbo1.bind();
    // The data in the buffer is placed like this:
    // vertex1.x, vertex1.y, vertex1.z, normal1.x, normal1.y, normal1.z, vertex2.x, ...
    m_program1->setAttributeBuffer(m_vertexAttr1, GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    m_program1->setAttributeBuffer(m_normalAttr1, GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    m_vbo1.release();

    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

    m_program1->disableAttributeArray(m_normalAttr1);
    m_program1->disableAttributeArray(m_vertexAttr1);
}

void CxOwnOpenGL::paintTexturedCube()
{
    m_texture->bind();

    if (!m_vbo2.isCreated()) {
        static GLfloat afVertices[] = {
            -0.5, 0.5, 0.5, 0.5,-0.5,0.5,-0.5,-0.5,0.5,
            0.5, -0.5, 0.5, -0.5,0.5,0.5,0.5,0.5,0.5,
            -0.5, -0.5, -0.5, 0.5,-0.5,-0.5,-0.5,0.5,-0.5,
            0.5, 0.5, -0.5, -0.5,0.5,-0.5,0.5,-0.5,-0.5,

            0.5, -0.5, -0.5, 0.5,-0.5,0.5,0.5,0.5,-0.5,
            0.5, 0.5, 0.5, 0.5,0.5,-0.5,0.5,-0.5,0.5,
            -0.5, 0.5, -0.5, -0.5,-0.5,0.5,-0.5,-0.5,-0.5,
            -0.5, -0.5, 0.5, -0.5,0.5,-0.5,-0.5,0.5,0.5,

            0.5, 0.5,  -0.5, -0.5, 0.5,  0.5,  -0.5,  0.5,  -0.5,
            -0.5,  0.5,  0.5,  0.5,  0.5,  -0.5, 0.5, 0.5,  0.5,
            -0.5,  -0.5, -0.5, -0.5, -0.5, 0.5,  0.5, -0.5, -0.5,
            0.5, -0.5, 0.5,  0.5,  -0.5, -0.5, -0.5,  -0.5, 0.5
        };

        static GLfloat afTexCoord[] = {
            0.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,
            1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,
            1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,
            0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,

            1.0f,1.0f, 1.0f,0.0f, 0.0f,1.0f,
            0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,
            0.0f,0.0f, 1.0f,1.0f, 1.0f,0.0f,
            1.0f,1.0f, 0.0f,0.0f, 0.0f,1.0f,

            0.0f,1.0f, 1.0f,0.0f, 1.0f,1.0f,
            1.0f,0.0f, 0.0f,1.0f, 0.0f,0.0f,
            1.0f,0.0f, 1.0f,1.0f, 0.0f,0.0f,
            0.0f,1.0f, 0.0f,0.0f, 1.0f,1.0f
        };

        GLfloat afNormals[] = {

            0,0,-1, 0,0,-1, 0,0,-1,
            0,0,-1, 0,0,-1, 0,0,-1,
            0,0,1, 0,0,1, 0,0,1,
            0,0,1, 0,0,1, 0,0,1,

            -1,0,0, -1,0,0, -1,0,0,
            -1,0,0, -1,0,0, -1,0,0,
            1,0,0, 1,0,0, 1,0,0,
            1,0,0, 1,0,0, 1,0,0,

            0,-1,0, 0,-1,0, 0,-1,0,
            0,-1,0, 0,-1,0, 0,-1,0,
            0,1,0, 0,1,0, 0,1,0,
            0,1,0, 0,1,0, 0,1,0
        };

        m_vbo2.create();
        m_vbo2.bind();
        m_vbo2.allocate(36 * 8 * sizeof(GLfloat));
        m_vbo2.write(0, afVertices, sizeof(afVertices));
        m_vbo2.write(sizeof(afVertices), afTexCoord, sizeof(afTexCoord));
        m_vbo2.write(sizeof(afVertices) + sizeof(afTexCoord), afNormals, sizeof(afNormals));
        m_vbo2.release();
    }

    m_program2->setUniformValue(m_textureUniform2, 0); // use texture unit 0

    m_program2->enableAttributeArray(m_vertexAttr2);
    m_program2->enableAttributeArray(m_normalAttr2);
    m_program2->enableAttributeArray(m_texCoordAttr2);

    m_vbo2.bind();
    // In the buffer we first have 36 vertices (3 floats for each), then 36 texture
    // coordinates (2 floats for each), then 36 normals (3 floats for each).
    m_program2->setAttributeBuffer(m_vertexAttr2, GL_FLOAT, 0, 3);
    m_program2->setAttributeBuffer(m_texCoordAttr2, GL_FLOAT, 36 * 3 * sizeof(GLfloat), 2);
    m_program2->setAttributeBuffer(m_normalAttr2, GL_FLOAT, 36 * 5 * sizeof(GLfloat), 3);
    m_vbo2.release();

    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_program2->disableAttributeArray(m_vertexAttr2);
    m_program2->disableAttributeArray(m_normalAttr2);
    m_program2->disableAttributeArray(m_texCoordAttr2);
}


void CxOwnOpenGL::initializeGL()
{
    initializeOpenGLFunctions();

    m_texture = new QOpenGLTexture(QImage("./qt.png"));

    m_vshader1 = new QOpenGLShader(QOpenGLShader::Vertex);
    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";
    m_vshader1->compileSourceCode(vsrc1);

    m_fshader1 = new QOpenGLShader(QOpenGLShader::Fragment);
    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";
    m_fshader1->compileSourceCode(fsrc1);

    m_program1 = new QOpenGLShaderProgram;
    m_program1->addShader(m_vshader1);
    m_program1->addShader(m_fshader1);
    m_program1->link();

    m_vertexAttr1 = m_program1->attributeLocation("vertex");
    m_normalAttr1 = m_program1->attributeLocation("normal");
    m_matrixUniform1 = m_program1->uniformLocation("matrix");

    m_vshader2 = new QOpenGLShader(QOpenGLShader::Vertex);
    const char *vsrc2 =
        "attribute highp vec4 vertex;\n"
        "attribute highp vec4 texCoord;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying highp vec4 texc;\n"
        "varying mediump float angle;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    angle = max(dot(normal, toLight), 0.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    m_vshader2->compileSourceCode(vsrc2);

    m_fshader2 = new QOpenGLShader(QOpenGLShader::Fragment);
    const char *fsrc2 =
        "varying highp vec4 texc;\n"
        "uniform sampler2D tex;\n"
        "varying mediump float angle;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec3 color = texture2D(tex, texc.st).rgb;\n"
        "    color = color * 0.2 + color * 0.8 * angle;\n"
        "    gl_FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);\n"
        "}\n";
    m_fshader2->compileSourceCode(fsrc2);

    m_program2 = new QOpenGLShaderProgram;
    m_program2->addShader(m_vshader2);
    m_program2->addShader(m_fshader2);
    m_program2->link();

    m_vertexAttr2 = m_program2->attributeLocation("vertex");
    m_normalAttr2 = m_program2->attributeLocation("normal");
    m_texCoordAttr2 = m_program2->attributeLocation("texCoord");
    m_matrixUniform2 = m_program2->uniformLocation("matrix");
    m_textureUniform2 = m_program2->uniformLocation("tex");

    m_fAngle = 0;
    m_fScale = 1;

    createGeometry();

    // Use a vertex buffer object. Client-side pointers are old-school and should be avoided.
    m_vbo1.create();
    m_vbo1.bind();
    // For the cube all the data belonging to the texture coordinates and
    // normals is placed separately, after the vertices. Here, for the Qt logo,
    // let's do something different and potentially more efficient: create a
    // properly interleaved data set.
    const int vertexCount = m_vertices.count();
    QVector<GLfloat> buf;
    buf.resize(vertexCount * 3 * 2);
    GLfloat *p = buf.data();
    for (int i = 0; i < vertexCount; ++i) {
        *p++ = m_vertices[i].x();
        *p++ = m_vertices[i].y();
        *p++ = m_vertices[i].z();
        *p++ = m_normals[i].x();
        *p++ = m_normals[i].y();
        *p++ = m_normals[i].z();
    }
    m_vbo1.allocate(buf.constData(), buf.count() * sizeof(GLfloat));
    m_vbo1.release();

    createBubbles(bubbleNum - m_bubbles.count());
}

int    CxOwnOpenGL::width()
{
    return 400;
}

int    CxOwnOpenGL::height()
{
    return 400;
}

void CxOwnOpenGL::InitContex()
{

    QSurfaceFormat format;
    format.setMajorVersion(2);
    format.setMinorVersion(0);

    window.setSurfaceType(QWindow::OpenGLSurface);
    window.setFormat(format);
    window.create();


    context.setFormat(format);
    if (!context.create())
        qFatal("Cannot create the requested OpenGL context!");
    context.makeCurrent(&window);


    const QRect drawRect(0, 0, width(), height());
    const QSize drawRectSize = drawRect.size();

    device = new QOpenGLPaintDevice(drawRectSize);

}

QImage CxOwnOpenGL::paintGL()
{
    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(2);
    fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    const QRect drawRect(0, 0, width(), height());
    const QSize drawRectSize = drawRect.size();
    QOpenGLFramebufferObject fbo(drawRectSize, fboFormat);
    fbo.bind();

    createBubbles(bubbleNum - m_bubbles.count());

    QPainter painter;
    painter.begin(device);

    painter.beginNativePainting();

    //glClearColor(m_background.redF(), m_background.greenF(), m_background.blueF(), m_transparent ? 0.0f : 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 modelview;
    modelview.rotate(m_fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(m_fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(m_fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(m_fScale);
    modelview.translate(0.0f, -0.2f, 0.0f);

    if (m_qtLogo) {
        m_program1->bind();
        m_program1->setUniformValue(m_matrixUniform1, modelview);
        paintQtLogo();
        m_program1->release();
    } else {
        m_program2->bind();
        m_program2->setUniformValue(m_matrixUniform2, modelview);
        paintTexturedCube();
        m_program2->release();
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    painter.endNativePainting();

    if (m_showBubbles)
        foreach (Bubble *bubble, m_bubbles) {
            bubble->drawBubble(&painter);
        }

    if (const int elapsed = m_time.elapsed()) {
        QString framesPerSecond;
        framesPerSecond.setNum(m_frames /(elapsed / 1000.0), 'f', 2);
        painter.setPen(m_transparent ? Qt::black : Qt::white);
        painter.drawText(20, 40, framesPerSecond + " paintGL calls / s");
        qDebug()<<framesPerSecond + " paintGL calls / s";
    }

    painter.end();
    fbo.release();

    QMutableListIterator<Bubble*> iter(m_bubbles);

    while (iter.hasNext()) {
        Bubble *bubble = iter.next();
        bubble->move(QRect(0,0,width(),height()));
    }
    if (!(m_frames % 100)) {
        m_time.start();
        m_frames = 0;
    }
    m_fAngle += 1.0f;
    ++m_frames;

    // When requested, follow the ideal way to animate: Rely on
    // blocking swap and just schedule updates continuously.
    return fbo.toImage();
}

void CxOwnOpenGL::createBubbles(int number)
{
    for (int i = 0; i < number; ++i) {
        QPointF position(width()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))),
                         height()*(0.1 + (0.8*qrand()/(RAND_MAX+1.0))));
        qreal radius = qMin(width(), height())*(0.0175 + 0.0875*qrand()/(RAND_MAX+1.0));
        QPointF velocity(width()*0.0175*(-0.5 + qrand()/(RAND_MAX+1.0)),
                         height()*0.0175*(-0.5 + qrand()/(RAND_MAX+1.0)));

        m_bubbles.append(new Bubble(position, radius, velocity));
    }
}

void CxOwnOpenGL::createGeometry()
{
    m_vertices.clear();
    m_normals.clear();

    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(x1, y1, x2, y2, y2, x2, y1, x1);
    quad(x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(x1, y1, x2, y2);
    extrude(x2, y2, y2, x2);
    extrude(y2, x2, y1, x1);
    extrude(y1, x1, x1, y1);
    extrude(x3, y3, x4, y4);
    extrude(x4, y4, y4, x4);
    extrude(y4, x4, y3, x3);

    const qreal Pi = 3.14159f;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i) {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(x6, y6, x7, y7);
        extrude(x8, y8, x5, y5);
    }

    for (int i = 0;i < m_vertices.size();i++)
        m_vertices[i] *= 2.0f;
}

void CxOwnOpenGL::quad(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4)
{
    m_vertices << QVector3D(x1, y1, -0.05f);
    m_vertices << QVector3D(x2, y2, -0.05f);
    m_vertices << QVector3D(x4, y4, -0.05f);

    m_vertices << QVector3D(x3, y3, -0.05f);
    m_vertices << QVector3D(x4, y4, -0.05f);
    m_vertices << QVector3D(x2, y2, -0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(x4 - x1, y4 - y1, 0.0f));

    m_normals << n;
    m_normals << n;
    m_normals << n;

    m_normals << n;
    m_normals << n;
    m_normals << n;

    m_vertices << QVector3D(x4, y4, 0.05f);
    m_vertices << QVector3D(x2, y2, 0.05f);
    m_vertices << QVector3D(x1, y1, 0.05f);

    m_vertices << QVector3D(x2, y2, 0.05f);
    m_vertices << QVector3D(x4, y4, 0.05f);
    m_vertices << QVector3D(x3, y3, 0.05f);

    n = QVector3D::normal
        (QVector3D(x2 - x4, y2 - y4, 0.0f), QVector3D(x1 - x4, y1 - y4, 0.0f));

    m_normals << n;
    m_normals << n;
    m_normals << n;

    m_normals << n;
    m_normals << n;
    m_normals << n;
}

void CxOwnOpenGL::extrude(qreal x1, qreal y1, qreal x2, qreal y2)
{
    m_vertices << QVector3D(x1, y1, +0.05f);
    m_vertices << QVector3D(x2, y2, +0.05f);
    m_vertices << QVector3D(x1, y1, -0.05f);

    m_vertices << QVector3D(x2, y2, -0.05f);
    m_vertices << QVector3D(x1, y1, -0.05f);
    m_vertices << QVector3D(x2, y2, +0.05f);

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    m_normals << n;
    m_normals << n;
    m_normals << n;

    m_normals << n;
    m_normals << n;
    m_normals << n;
}

#if 0
void CxOwnOpenGL::setTransparent(bool transparent)
{
    setAttribute(Qt::WA_AlwaysStackOnTop, transparent);
    m_transparent = transparent;
    // Call update() on the top-level window after toggling AlwayStackOnTop to make sure
    // the entire backingstore is updated accordingly.
    window()->update();
}
#endif
void CxOwnOpenGL::resizeGL(int, int)
{
}

