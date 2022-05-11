#include "stuff.h"

bool operator==(Coord a, Coord b)
{
    if(a.x == b.x && a.y == b.y)
        return true;
    return false;
}

bool operator!=(Coord a, Coord b)
{
    if(!(a==b))
        return true;
    return false;
}
