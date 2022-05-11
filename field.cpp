#include "field.h"


Field::Field(int X, int Y, QObject *parent) : QObject(parent)
{
    //Field(Coord(X,Y), parent);
    pos.x = X;
    pos.y = Y;
}

Field::Field(Coord c, QObject *parent) : QObject(parent)
{
    pos = c;
}

Field::~Field()
{

}

Coord Field::getCoord() const
{
    return pos;
}

bool Field::Occupy(Player player)
{
    QMutexLocker locker(&mutex);
    if(player != Player::both)
    {
        occupied = player;
        return true;
    }
    return false;

}

bool Field::Free()
{
    QMutexLocker locker(&mutex);
    occupied = Player::none;

}


Player Field::occupiedStatus() const
{
    return occupied;
}
