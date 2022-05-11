#ifndef BOARDOGL_H
#define BOARDOGL_H

#include <QObject>
#include <QOpenGLWidget>
#include <QGridLayout>
#include <QFrame>

#include "board.h"
#include "fieldogl.h"

class BoardOGL : public QWidget
{
    Q_OBJECT

    QMutex mutex;
    QPainter * painter;
    FieldOGL ** fields;
    Board * board;

    int fieldSize;
    Coord fieldCount;
    Coord currentSelected;

    QColor background;

    QGridLayout * layoutMain;


protected:
    void paintEvent(QPaintEvent *event) override;

public:
    BoardOGL(QWidget *parent = nullptr);
    ~BoardOGL();

    void ConnectToBoard(Board * b);

    Player getFieldStatus(int x, int y) { return board->getFieldStatus(x,y); };
    FieldOGL * getField(int x, int y);
    Field *getFieldPtr(int x, int y) { return board->getField(x,y); };

    Board *getBoard() const;

public slots:
    void slotFieldSelected(Coord);

signals:
    void signalFieldSelected(Coord);
};

#endif // BOARDOGL_H
