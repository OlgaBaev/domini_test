#include "fieldogl.h"


FieldOGL::FieldOGL(Coord pos, QPainter * p, QWidget *parent) : QWidget(parent)
{
    //field = new Field(pos);
    selected = false;
    position = pos;
    background = Qt::cyan;
    painter = p;

}

FieldOGL::~FieldOGL()
{

}

Coord FieldOGL::getPosition() const
{
    return position;
}

void FieldOGL::mousePressEvent(QMouseEvent *me)
{
    emit signalFieldSelected(position);
    update();

}

void FieldOGL::connectToField(Field *f)
{
    field = f;
    if(field->getCoord().x == 4 && field->getCoord().y == 5)
        int i = 10;
    position = field->getCoord();

}

void FieldOGL::init()
{

}

void FieldOGL::resize(int nWidth, int nHeight)
{

}

void FieldOGL::paint()
{
}

void FieldOGL::select()
{
    //QMutexLocker locker(&mutex);
    selected = true;
    update();

}

void FieldOGL::unselect()
{
    //QMutexLocker locker(&mutex);
    selected = false;
    update();

}

void FieldOGL::drawPawn(QColor color)
{
    Coord pos = Coord(size().width()/2, size().height()/2);
    painter->setPen(color);
    painter->setBrush(color);
    painter->drawEllipse(pos.x-8, pos.y-8, 16, 16);
}

void FieldOGL::clear()
{
    //QMutexLocker locker(&mutex);

}

void FieldOGL::paintEvent(QPaintEvent *event)
{
    QMutexLocker locker(&mutex);
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    if(selected)
    {
        painter->setPen(Qt::green);
        painter->setBrush(Qt::green);
    }
    else
    {
        painter->setPen(Qt::yellow);
        painter->setBrush(Qt::yellow);
    }


    painter->drawRect(2, 2, size().width() - 4, size().height() - 4);
    if(field->occupiedStatus() == Player::white)
        drawPawn(Qt::white);
    if(field->occupiedStatus() == Player::black)
        drawPawn(Qt::black);
    painter->end();
    //update();

}

