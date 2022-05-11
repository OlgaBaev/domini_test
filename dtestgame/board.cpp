#include "board.h"
#include <QRandomGenerator>


Board::Board(int SizeX, int SizeY, QObject *parent) : QObject(parent)
{
    if(SizeX < 1) sizeX = 1;
    if(SizeY < 1) sizeY = 1;

    sizeX = SizeX; sizeY = SizeY;

    fields = new Field*[SizeX*SizeY];
    for(int i = 0; i < sizeX; i++)
        for(int j = 0; j < sizeY; j++)
        {
            fields[i*sizeY + j] = new Field(i, j);
        }

    winConditionBlack = new bool[sizeX*sizeY];
    winConditionWhite = new bool[sizeX*sizeY];

    initBoard();


}

Board::~Board()
{
    for(int i = 0; i < sizeX; i++)
        for(int j = 0; j < sizeY; j++)
        {
            delete fields[i*sizeY + j];
        }
    delete[] fields;
    delete[] winConditionBlack;
    delete[] winConditionWhite;

}


int Board::getSizeX() const
{
    return sizeX;
}

int Board::getSizeY() const
{
    return sizeY;
}

int Board::getWhiteCount() const
{
    return whitePawns.size();
}

int Board::getBlackCount() const
{
    return blackPawns.size();
}

Coord Board::getRandomPawn(Player player)
{
    int size = player == Player::white ? getWhiteCount() : getBlackCount();
    QVector <Coord> * ptPawns = player == Player::white ? &whitePawns : &blackPawns;

    int id = QRandomGenerator::global()->bounded(size);
    return (*ptPawns)[id];
}

Coord Board::getPawn(Player player)
{
    static int id = 0;
    QVector <Coord> * ptPawns = player == Player::white ? &whitePawns : &blackPawns;

    Coord ret = (*ptPawns)[id];
    id++;
    if(id==ptPawns->size())
        id = 0;

    return ret;

}

Coord Board::bestNextMove(Player player, Coord pawn)
{
    QVector <Coord> * ptFinish = player == Player::white ? &whiteFinish : &blackFinish;

    QVector <Coord> path;
    int shortest = 100;
    QVector <Coord> ::iterator sh;
    for(QVector <Coord> ::iterator it = (*ptFinish).begin(); it != (*ptFinish).end(); ++it)
    {
        int p = shortestPath(pawn, *it, &path);
        if(p < shortest)
        {
            shortest = p;
            sh = it;
        }
    }
    if(shortest == 100 || shortest == -1)
        return Coord(-1, -1);
    shortestPath(pawn, *sh, &path);
    return *path.rbegin();

}

bool Board::checkWinCondition(bool *winCondition, Player player)
{
    if(!winCondition) return false;
    //QMutexLocker locker(&mutex);

    bool won = true;
    int id = 0;
    for(int i = 0; i < sizeX; i++)
        for(int j = 0; j < sizeY; j++)
        {
            id = i*sizeY + j;
            if(i == 4 && j == 4)
            {
                Player p = fields[id]->occupiedStatus();
                bool w = winCondition[id];
                bool r = fields[id]->occupiedStatus() & winCondition[id];
                int g = 0;
            }
            won = won & (((fields[id]->occupiedStatus() == player) & winCondition[id]) == winCondition[id]);
            if(!won) return false;
        }

    return won;
}

void Board::fillPawnLists()
{
    int id = 0;
    for(int i = 0; i < sizeX; i++)
        for(int j = 0; j < sizeY; j++)
        {
            id = i*sizeY + j;
            if(fields[id]->occupiedStatus() == Player::white)
            {
                whitePawns.append(Coord(i, j));
            }
            if(fields[id]->occupiedStatus() == Player::black)
            {
                blackPawns.append(Coord(i, j));
            }
        }

}

void Board::initBoard()
{
    int id = 0;
    for(int i = 0; i < sizeX; i++)
        for(int j = 0; j < sizeY; j++)
        {
            id = i*sizeY + j;
            fields[id]->Occupy(cDefultPositions8[id]);

            winConditionBlack[id] = fields[id]->occupiedStatus() == Player::white;
            if(winConditionBlack[id]) blackFinish.append(Coord(i, j));
            winConditionWhite[id] = fields[id]->occupiedStatus() == Player::black;
            if(winConditionWhite[id]) whiteFinish.append(Coord(i, j));

            //if(fields[id]->occupiedStatus() == Player::white) whiteCount++;
            //if(fields[id]->occupiedStatus() == Player::black) blackCount++;
        }

    whitePawns.clear();
    blackPawns.clear();
    fillPawnLists();

}

int Board::shortestPath(Coord from, Coord to, QVector<Coord> *path)
{
    int points[SIZEX][SIZEY];
    for(int i = 0; i < SIZEX; i++)
        for(int j = 0; j < SIZEY; j++)
        {
            if(getFieldStatus(i, j) == Player::none)
                points[i][j] = -1;
            else points[i][j] = 100;
        }

    points[from.x][from.y] = 0;
    bool b = true;
    int step = 0;

    while (points[to.x][to.y] == -1)
    {
        b = false;
        for(int i = 0; i < SIZEX; i++)
            for(int j = 0; j < SIZEY; j++)
                if(points[i][j] == step)
                {
                    if(points[qMax(0, i-1)][j] == -1) points[qMax(0, i-1)][j] = step+1;
                    if(points[qMin(i+1, SIZEX-1)][j]  == -1) points[qMin(i+1, SIZEX-1)][j] = step+1;
                    if(points[i][qMax(0, j-1)] == -1) points[i][qMax(0, j-1)] = step+1;
                    if(points[i][qMin(j+1, SIZEY -1)] == -1) points[i][qMin(j+1, SIZEY -1)] = step+1;

                    b = true;

                }
        if(!b) points[to.x][to.y] = 0;
        else step++;
    }

        if(step > 64)
        {
            path->clear();
            return -1;
        }

        int shortest = step;
        if(points[to.x][to.y]>0 && points[to.x][to.y] < 100)
        {
            path->clear();
            path->append(Coord(to.x, to.y));
            Coord minc = Coord(to.x, to.y);
            while(step-1)
            {
                Coord curr = Coord(qMax(0, minc.x-1), minc.y);
                if(points[curr.x][curr.y] == step-1)
                {
                    minc = curr; step--;
                    path->append(Coord(minc.x, minc.y));
                    continue;
                }
                curr = Coord(qMin(minc.x+1, SIZEX-1), minc.y);
                if(points[curr.x][curr.y] == step-1)
                {
                    minc = curr; step--;
                    path->append(Coord(minc.x, minc.y));
                    continue;
                }
                curr = Coord(minc.x, qMax(0, minc.y-1));
                if(points[curr.x][curr.y] == step-1)
                {
                    minc = curr; step--;
                    path->append(Coord(minc.x, minc.y));
                    continue;
                }
                curr = Coord(minc.x, qMin(minc.y+1, SIZEY-1));
                //if(points[curr.x][curr.y] == step-1)
                {
                    minc = curr; step--;
                    path->append(Coord(minc.x, minc.y));
                    continue;
                }
            }

            return shortest;
        }
        else
            return -1;


}


void Board::ResetBoard()
{
    initBoard();

}

Player Board::getFieldStatus(int x, int y)
{
    return fields[x*sizeY + y]->occupiedStatus();

}

Field *Board::getField(int x, int y)
{
    return fields[x*sizeY + y];


}

bool Board::Move(int fromX, int fromY, int toX, int toY)
{
    if(fromX >= sizeX || fromY >= sizeY
            || toX >= sizeX || toY >= sizeY
            || fromX < 0 || fromY < 0
            || toX < 0 || toY < 0)
        return false;


    Field * from = fields[fromX * sizeY + fromY];
    Field * to = fields[toX * sizeY + toY];

    if(to->occupiedStatus() != Player::none)
        return false;

    QVector <Coord> * ptPawns = from->occupiedStatus() == Player::white
            ? &whitePawns : &blackPawns;

    int i = -1;
    i = ptPawns->indexOf(Coord(fromX, fromY));
    if(i == -1) return false;
    (*ptPawns)[i] = Coord(toX, toY);


    to->Occupy(from->occupiedStatus());
    from->Free();
    return true;
}

bool Board::didBlackWon()
{
    return checkWinCondition(winConditionBlack, Player::black);
}

bool Board::didWhiteWon()
{
    return checkWinCondition(winConditionWhite, Player::white);
}
