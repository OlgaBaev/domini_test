#ifndef FIELD_H
#define FIELD_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>

#include "stuff.h"

class Field : public QObject
{
    Q_OBJECT

    Coord pos;
    Player occupied;

    QMutex mutex;

public:
    explicit Field(int X, int Y, QObject *parent = nullptr);
    explicit Field(Coord c, QObject *parent = nullptr);
    ~Field();

    Player occupiedStatus() const;
    bool Occupy(Player player);
    bool Free();

    Coord getCoord() const;

signals:

};

#endif // FIELD_H
