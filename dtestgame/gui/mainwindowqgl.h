#ifndef MAINWINDOWQGL_H
#define MAINWINDOWQGL_H

#include <QOpenGLWidget>
#include <QObject>
#include <QLabel>
#include <QKeyEvent>

#include "boardogl.h"
#include "game.h"

class MainWindowQGL : public QOpenGLWidget
{
    Q_OBJECT

    QGridLayout * layoutMain;
    QPainter * painter;

    QLabel * lInfo;
    QLabel * lControlInfo;

    BoardOGL * board;
    Game * game;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *ke) override;
    void mousePressEvent(QMouseEvent *me) override;

public:
    MainWindowQGL();
    ~MainWindowQGL();

    bool pick(Coord pos);
    bool unpick();

public slots:
    void slotFieldSelected(Coord);
    void slotVictory(Player);
    void slotNoMoreAIMoves();
};

#endif // MAINWINDOWQGL_H
