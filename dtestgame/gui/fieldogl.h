#ifndef FIELDOGL_H
#define FIELDOGL_H

#include <QGLWidget>
#include <QObject>
#include <QColor>
#include <QMutex>
#include <QMutexLocker>

#include "stuff.h"
#include "field.h"

class FieldOGL : public QWidget
{
    Q_OBJECT

    QMutex mutex;
    QPainter * painter;

    Coord position; //the number of field

    Field *field;
    bool selected;

    QColor background;

    //drawing
    void drawPawn(QColor color);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *me) override;

public:
    FieldOGL(Coord pos, QPainter * p, QWidget *parent = nullptr);
    ~FieldOGL();

    Coord getPosition() const;


    void connectToField(Field* f);

    Player occupiedStatus() { return field->occupiedStatus(); };
    bool Occupy(Player player) { return field->Occupy(player); };
    bool Free() {return field->Free(); };

    //drawing
    void init();
    void resize(int nWidth, int nHeight);
    void paint();
    void select();
    void unselect();

signals:
    void signalFieldSelected(Coord);

};

#endif // FIELDOGL_H
