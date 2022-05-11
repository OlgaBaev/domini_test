#include "boardogl.h"

BoardOGL::BoardOGL(QWidget *parent)
{
    fieldSize = 64;

    board = new Board(SIZEX, SIZEY);
    currentSelected = Coord(-1,-1);

    painter = new QPainter();
    layoutMain = new QGridLayout;
    fields = new FieldOGL*[board->getSizeX() * board->getSizeY()];
    for(int i = 0; i < board->getSizeX(); i++)
        for(int j = 0; j < board->getSizeY(); j++)
        {
            fields[i*board->getSizeY() + j] = new FieldOGL(Coord(i, j), painter, this);
            fields[i*board->getSizeY() + j]->connectToField(board->getField(i,j));
            fields[i*board->getSizeY() + j]->setFixedSize(fieldSize, fieldSize);
            connect(fields[i*board->getSizeY() + j], SIGNAL(signalFieldSelected(Coord)),
                    this, SLOT(slotFieldSelected(Coord)));
            layoutMain->addWidget(fields[i*board->getSizeY() + j], i, j);
        }
    setLayout(layoutMain);


    update();
}

BoardOGL::~BoardOGL()
{
    delete painter;
    for(int i = 0; i < board->getSizeX(); i++)
        for(int j = 0; j < board->getSizeY(); j++)
            delete getField(i,j);
    delete [] fields;
}

Board *BoardOGL::getBoard() const
{
    return board;
}

FieldOGL *BoardOGL::getField(int x, int y)
{
    return fields[x*board->getSizeY() + y];

}

void BoardOGL::paintEvent(QPaintEvent *event)
{
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->end();
}



void BoardOGL::slotFieldSelected(Coord position)
{
    //board->Move(currentSelected.x, currentSelected.y, position.x, position.y);
    //getField(currentSelected.x, currentSelected.y)->unselect();
    //currentSelected = position;
    //getField(currentSelected.x, currentSelected.y)->select();
    emit signalFieldSelected(position);
}
