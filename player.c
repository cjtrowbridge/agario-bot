#include <time.h>
#include <stdlib.h>
#include "bot.h"
#include <stdlib.h>
#include <math.h>

double meX;
double meY;

int distanceSorter(const void *a, const void *b)
{
    //distance from myself to a
    double aX = (*(struct food *)a).x;
    double aY = (*(struct food *)a).y;
    double distanceA = sqrt(
        (meX - aX)*(meX - aX) +
        (meY - aY)*(meY - aY)
    );
    
    //distance from myself to b
    double bX = (*(struct food *)b).x;
    double bY = (*(struct food *)b).y;
    double distanceB = sqrt(
        (meX - bX)*(meX - bX) +
        (meY - bY)*(meY - bY)
    );
    
    return distanceA - distanceB;
}

struct action playerMove(struct player me, 
                         struct player * players, int nplayers,
                         struct food * foods, int nfood,
                         struct cell * virii, int nvirii,
                         struct cell * mass, int nmass)
{
    struct action act;
    
    meX = me.x;
    meY = me.y;
    
    //Sort food by distance
    qsort(foods, nfood, sizeof(struct food), distanceSorter);
    
    //food[0] is now the closest food
    
    // Move to the lower-right
    act.dx = meX - foods[0].x;
    act.dy = meY - foods[0].y;
    act.fire = 0;
    act.split = 0;
    
    return act;
}