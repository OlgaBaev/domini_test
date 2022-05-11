#include "game.h"

Game::Game(QObject *parent) : QObject(parent)
{
    //board = new Board(SIZEX, SIZEY);
    currentPlayer = Player::white;
    selection = {0, 0};
    selected = false;
    isRun = true;
}

Game::~Game()
{
    isRun = false;
    delete board;
}

Player Game::getCurrentPlayer() const
{
    return currentPlayer;
}

bool Game::getIsRun() const
{
    return isRun;
}

bool Game::pick(Coord pos)
{
    if(!isRun) return false;

    //if(board->getFieldStatus(pos.x, pos.y) == currentPlayer)
    Player status = board->getFieldStatus(pos.x, pos.y);
    if(status == currentPlayer)
    {
        selection = pos;
        selected = true;
    }
    else
    {
        selected = false;
    }

    return selected;

}

void Game::unpick()
{
    selected = false;

}

bool Game::move(Direction direction)
{
    switch (direction)
    {
        case up: return move(Coord(selection.x, selection.y - 1));
        case down: return move(Coord(selection.x, selection.y + 1));
        case left: return move(Coord(selection.x-1, selection.y));
        case right: return move(Coord(selection.x+1, selection.y));
        default: return false;
    }
}

bool Game::move(Coord pos)
{
    if(!isRun) return false;
    if(!selected) return false;
    if(abs(selection.x - pos.x) + abs(selection.y - pos.y) != 1)
        return false;
    bool res = board->Move(selection.x, selection.y, pos.x, pos.y);
    if(res) changePlayer();

    if(board->didBlackWon())
    {
        isRun = false;
        emit signalVictory(Player::black);
        //reset();
    }
    if(board->didWhiteWon())
    {
        isRun = false;
        emit signalVictory(Player::white);
        //reset();
    }

    return res;
}

Coord Game::getSelected()
{
    if(!selected) return Coord(-1, -1);
    return selection;
}

void Game::changePlayer()
{
    if(currentPlayer == Player::white)
        currentPlayer = Player::black;
    else
        currentPlayer = Player::white;
}

void Game::connectBoard(Board *b)
{
    board = b;
}

bool Game::makeAIMove()
{
    if(!isRun) return false;
    Coord pawn = board->getRandomPawn(Player::black);
    Coord movec = board->bestNextMove(Player::black,  pawn);
    if(movec.x == -1 || movec.y == -1)
    {
        unpick();
        //temp
        if(!makeAIMoveDumb())
        {
            emit signalNoMoreAIMoves();
            return false;
        }
        return true;
        //~temp
    }
    pick(pawn);
    move(movec);
    return  true;

}

bool Game::makeAIMoveDumb()
{
    //если все цели заняты, сделаем любой шаг по направлению к стандартной расстановке
    //временная версия, требуется заменить поиском временной цели

    if(!isRun) return false;

    int limit = 0;

    Coord pawn = board->getRandomPawn(Player::black);
    bool ableToMove = false;
    while(!ableToMove && limit < 100)
    {
        if(move(Direction::down))
            ableToMove = true;
        else if(move(Direction::right))
            ableToMove = true;
        else
        {
            unpick();
            pawn = board->getRandomPawn(Player::black);
            while(!pick(pawn))
                pawn = board->getRandomPawn(Player::black);
        }
        limit++;
    }
    if(ableToMove) return true;

    if(limit == 100)
    {
        limit = 0;
        while(limit < 100)
        {
            pawn = board->getPawn(Player::black);
            pick(pawn);

            if(move(Direction::up))
                return true;
            else if(move(Direction::left))
                return true;
            else
            {
                pawn = board->getPawn(Player::black);
                limit++;
            }
        }
    }

    return false;

}

void Game::reset()
{
    QMutexLocker locker(&mutex);
    isRun = false;
    board->ResetBoard();
    currentPlayer = Player::white;
    unpick();
}
