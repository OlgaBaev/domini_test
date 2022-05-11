#ifndef STUFF_H
#define STUFF_H

#include <QPair>

typedef struct coord
{
    int x;
    int y;

    coord(int a, int b)
    {
        x = a;
        y = b;
    }

    coord()
    {
        x = -1;
        y = -1;
    }
} Coord;

bool operator==(Coord a, Coord b);
bool operator!=(Coord a, Coord b);

enum Player {none=0, white, black, both};

const int SIZEX = 8;
const int SIZEY = 8;

const Player cDefultPositions8[8*8] =
{
    black, black, black, none, none, none, none, none,
    black, black, black, none, none, none, none, none,
    black, black, black, none, none, none, none, none,
    none, none, none, none, none, none, none, none,
    none, none, none, none, none, none, none, none,
    none, none, none, none, none, white, white, white,
    none, none, none, none, none, white, white, white,
    none, none, none, none, none, white, white, white
};

/*//test
const Player cDefultPositions8[8*8] =
{
    none, none, none, none, none, none, none, none,
    none, none, none, none, none, none, none, none,
    none, none, none, none, none, none, none, none,
    none, none, none, black, none, none, none, none,
    none, none, none, none, white, none, none, none,
    none, none, none, none, none, none, none, none,
    none, none, none, none, none, none, none, none,
    none, none, none, none, none, none, none, none
};*/

#endif // STUFF_H
