## Sierra College: CSCI--0046



## Lab 10: Agar.io Bot

## Introduction
In this assignment you will be programming an AI bot for an Agario-like game. Your bot will connect to the Agario arena and be given information about nearby objects. It will make a decision about which way to go, make the move, and then repeat its decision-making based on new information.

## Setting Up for Cloud9
Installing the Agar.io-clone server
You only need to do this if you want to run your own server. If you just want to use mine, skip ahead to the next section.

## Open a Terminal tab.
Download the Agar.io-clone project which will act as the server. It runs on NodeJS, which is a server-side implementation of JavaScript. NodeJS is already installed on Cloud9.
git clone https://github.com/profbbrown/agar.io-clone.git
Edit the config.json file and change the port to 8080.
Install additional NodeJS packages.
cd agar.io-clone
npm install
It will take a while for the installation to complete. You will probably see several WARN messages go by; you can ignore them.
Run the server.
npm start
You can stop the server any time by pressing CTRL-C.

At this point you should be able to play the game in your browser. The hostname of the game is constructed out of the workspace name and user name for Cloud9. For example, mine is csci46-bbrown.c9users.io.

http://workspace-user.c9users.io:8080

## Installing the bot client
1. Fork my BitButcket repository at https://bitbucket.org/profbbrown/agario-bot (Links to an external site.)Links to an external site.
1. Clone your copy of the repository.
1. At the top of bot.cpp you'll see a string for BOTNAME. You may want to change this.
1. If you are connecting to my server, change the URL to http://flame.runwire.com:8080
1. make
1. Run the executable.
You can stop the bot with CTRL-C.

Use your browser to watch the game in Spectate mode. Note: on my browser the Spectate mode doesn't always show the whole arena. You may have to scroll around. Or refresh the page to get a new Spectate screen.

As written, the bot moves to the lower right. You should be able to see a small cell moving on the screen.

## Give your bot a brain
All your programming happens in the playerMove function in player.c.

The function is given the following pieces of data:

*. struct player me -- Information about the current player (the bot)
*. struct player * players -- Array of other visible players
*. int nplayers -- Number of players
*. struct food * foods -- Array of visible food
*. int nfood -- Number of visible food
*. struct cell * virii -- Array of visible virii
*. int nvirii -- Number of visible virii
*. struct cell * mass -- Array of visible mass
*. int nmass -- Number of visible mass
Every time playerMove is called, the arrays are filled with the objects that are visible. The visibility is limited to a 600x600 square around the bot. Your job is to use the information in the arrays to make a better decision about where to move.

Make your decision and put the information into a struct action:

*. dx -- direction to move
*. dy -- direction to move
*. fire -- 0 or 1
*. split -- 0 or 1

Some things that you could try are:

Sort the array of food by distance from me and go toward the closest one.
Find the largest player that is smaller than you and go toward it.
Find the players larger than you and go away from them.
Fire mass (equivalent to pressing 'w' in the browser game).
Split your cells (equivalent to pressing 'space').

## Sorting the food array
To sort the food array by distance from the player, use the qsort function:

`qsort(foods, nfood, sizeof(struct food), dist);`

With the array sorted you can simply use the 0th element of the array and go toward its x-y location by subtracting them from the player's x-y location:

```
act.dx = foods[0].x - me.x;
act.dy = foods[0].y - me.y; 
```

To make the sorting work you'll need a comparison function. Here I've called it dist:

```
int dist(const void *a, const void *b)
{
    double ax = (*(struct food *)a).x;
    double ay = (*(struct food *)a).y;
    double bx = (*(struct food *)b).x;
    double by = (*(struct food *)b).y;
    
    double dist_to_a = sqrt((myx - ax)*(myx - ax) + (myy - ay)*(myy - ay)); 
    double dist_to_b = sqrt((myx - bx)*(myx - bx) + (myy - by)*(myy - by));
    return dist_to_a - dist_to_b;
} 
```

You'll need to set up a couple of global doubles to hold the player's x and y position so they are available to the dist function. Here's what playerMove ends up looking like:

```
struct action playerMove(struct player me, 
                         struct player * players, int nplayers,
                         struct food * foods, int nfood,
                         struct cell * virii, int nvirii,
                         struct cell * mass, int nmass)
{
    struct action act;

    // Copy player's x-y to globals
    myx = me.x;
    myy = me.y;
    
    // Sort food by distance
    qsort(foods, nfood, sizeof(struct food), dist);
    
    // Move toward closest food
    act.dx = foods[0].x - me.x;
    act.dy = foods[0].y - me.y;
    act.fire = 0;
    act.split = 0;
    
    return act;
} 
```
