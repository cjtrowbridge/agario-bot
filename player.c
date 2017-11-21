#include <time.h>
#include <stdio.h>
#include "bot.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

double meX;
double meY;
int globalMode;

const int modeGatherer =        1; //Find food and eat it
const int modeEvadeVirus =      2; //Evade viruses until the closest one is at least EvadeVirusDistance away
const int modeHunter =          3; //Hunt other cells

const int EvadeVirusTrigger =   200; //The distance from viruses when we trigger evade virus mode
const int EvadeVirusDistance =  300; //The distance to get away from viruses when we get too close to them

struct cell corners[4] = {
    {7,7,0,0},
    {7,4991,0,0},
    {4991,7,0,0},
    {4991,4991,0,0}
};


int foodSorter(const void *a, const void *b)
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

int cellSorter(const void *a, const void *b)
{
    //distance from myself to a
    double aX = (*(struct cell *)a).x;
    double aY = (*(struct cell *)a).y;
    double distanceA = sqrt(
        (meX - aX)*(meX - aX) +
        (meY - aY)*(meY - aY)
    );
    
    //distance from myself to b
    double bX = (*(struct cell *)b).x;
    double bY = (*(struct cell *)b).y;
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
    int prospectiveX;
    int prospectiveY;
    
    //Sort food, virii, and corners by distance
    qsort(foods, nfood, sizeof(struct food), foodSorter);
    qsort(virii, nvirii, sizeof(struct cell), cellSorter);
    qsort(corners, 4, sizeof(struct cell), cellSorter);
    
    
    //food[0] is now the closest food
    //virii[0] is now the closest virus
    
    printf("(Mode: %d)(Pos: %4d,%4d) ",globalMode,(int)meX,(int)meY);
    
    // Maybe move in a random direction
    //prospectiveX = rand() % (100 + 1)-50;
    //prospectiveY = rand() % (100 + 1)-50;
        
    //Move by default towards the furthest corner
    prospectiveX = corners[3].x;
    prospectiveY = corners[3].y;
    
    
    if(nfood > 0)
    {
        // Maybe move to the closest food
        prospectiveX = foods[0].x - meX;
        prospectiveY = foods[0].y - meY;
    }else{
        printf("(No Food In View)");
    }
    
    
    if(nvirii > 0)
    {
        //calculate distance to nearest virus, if it is less than EvadeVirusTrigger, set global mode to evade virus.
        //double myradius = cbrt(3 * me.totalMass / 4 * 3.14);
        double myradius = cbrt(me.totalMass / 3.14);
        int minViriiEdgeDistance = 99999999;
        
        double virusDistance = sqrt(
            abs(meX - virii[0].x)*abs(meX - virii[0].x) +
            abs(meY - virii[0].y)*abs(meY - virii[0].y)
        );//-
        //myradius-
        //virii[0].radius;
        
        if( virusDistance < EvadeVirusTrigger )
        {
            globalMode = modeEvadeVirus;
        }else{
            if( virusDistance > EvadeVirusDistance )
            {
                globalMode = modeGatherer;
            }
        }
        
        if( globalMode == 2 )
        {
            prospectiveX = -100 * virii[0].x;
            prospectiveY = -100 * virii[0].y;
        }
        
        printf("(Closest Virus: %d)(My R: %lf)(V R: %lf)",(int)virusDistance, myradius,virii[0].radius);
        
    }else{
        printf("(No Viruses In View)");
    }
    
    
    act.dx = prospectiveX;
    act.dy = prospectiveY;
        
    printf("\n");
    
    act.fire = 0;
    act.split = 0;
    
    return act;
}