#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <nanomsg/nn.h>
#include <nanomsg/pair.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    if ((sock = nn_socket(AF_SP, NN_PAIR)) < 0) {
            qDebug("nn_socket");
    }
    if (nn_connect(sock, "ipc:///tmp/pair.ipc") < 0) {
            qDebug("nn_connect");
    }

    startTimer(5);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::timerEvent(QTimerEvent *event)
{
    char *buf = NULL;
    int result = nn_recv(sock, &buf, NN_MSG, 1);
    if (result > 0) {
        //printf("%s: RECEIVED \"%s\"\n", name, buf);
        QImage  targetImage((uchar*)buf,400,400,QImage::Format_ARGB32);

        this->ui->label->setPixmap(QPixmap::fromImage(targetImage));
        nn_freemsg(buf);
    }
}

