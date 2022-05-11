#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    windowogl = new MainWindowQGL;

    layoutMain = new QGridLayout;
    layoutMain->addWidget(windowogl, 0, 0);

    QLabel label("TEST");
    layoutMain->addWidget(&label, 1, 0);

    //windowogl->setFixedSize(8*16, 8*16);

    //setLayout(layoutMain);

    ui->setupUi(this);
    ui->centralwidget->setLayout(layoutMain);
}

MainWindow::~MainWindow()
{
    delete ui;
}

