#include "bot.h"

struct target playerMove(struct player me, 
                         struct player * players, int nplayers,
                         struct food * foods, int nfood,
                         struct cell * virii, int nvirii,
                         struct cell * mass, int nmass)
{
    struct target t;
    
    t.dx = 0;
    t.dy = 0;
    
    return t;
}