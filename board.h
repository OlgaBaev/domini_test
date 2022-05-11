#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

#include "stuff.h"
#include "field.h"



class Board : public QObject
{
    Q_OBJECT


    int sizeX;
    int sizeY;

    //int whiteCount;
    //int blackCount;

    Field** fields;
    bool * winConditionWhite;
    bool * winConditionBlack;

    QMutex mutex;

    QVector <Coord> whitePawns;
    QVector <Coord> blackPawns;
    QVector <Coord> whiteFinish;
    QVector <Coord> blackFinish;

    bool checkWinCondition(bool* winCondition, Player player);
    void fillPawnLists();

    void initBoard();
    int shortestPath(Coord from, Coord to, QVector <Coord> * path);



public:
    explicit Board(int SizeX, int SizeY, QObject *parent = nullptr);
    ~Board();

    void ResetBoard();

    Player getFieldStatus(int x, int y);
    Field *getField(int x, int y);

    bool Move(int fromX, int fromY, int toX, int toY);

    bool didBlackWon();
    bool didWhiteWon();

    int getSizeX() const;
    int getSizeY() const;

    int getWhiteCount() const;
    int getBlackCount() const;

    Coord getRandomPawn(Player player);
    Coord getPawn(Player player);
    Coord bestNextMove(Player player, Coord pawn);


signals:

};

#endif // BOARD_H
