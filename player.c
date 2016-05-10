#include "bot.h"

struct action playerMove(struct player me, 
                         struct player * players, int nplayers,
                         struct food * foods, int nfood,
                         struct cell * virii, int nvirii,
                         struct cell * mass, int nmass)
{
    struct action act;
    
    // Move to the lower-right
    act.dx = 100;
    act.dy = 100;
    act.fire = 0;
    act.split = 0;
    
    return act;
}