#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>


#include "board.h"

enum Direction {up, down, left, right};
//ai starts as black always; temporary

class Game : public QObject
{
    Q_OBJECT

    Board * board;
    Player currentPlayer;
    QMutex mutex;

    Coord selection;
    bool selected;

    bool isRun;

    int shortestPath(Coord from, Coord to, QVector <Coord> * path);
    bool makeAIMoveDumb();


public:
    explicit Game(QObject *parent = nullptr);
    ~Game();

    bool pick(Coord pos);
    void unpick();
    bool move(Direction direction);
    bool move(Coord pos);
    Coord getSelected();
    void changePlayer();

    void connectBoard(Board * b);

    bool makeAIMove();

    void reset();
    void start() { isRun = true; }


    Player getCurrentPlayer() const;

    bool getIsRun() const;

signals:
    void signalVictory(Player);
    void signalNoMoreAIMoves();


};

#endif // GAME_H
