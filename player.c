#include "bot.h"

struct target playerMove(struct player me, 
                         struct player * players, int nplayers,
                         struct food * foods, int nfood,
                         struct cell * virii, int nvirii,
                         struct cell * mass, int nmass)
{
    struct target t;
    
    // Move to the lower-right
    t.dx = 100;
    t.dy = 100;
    
    return t;
}