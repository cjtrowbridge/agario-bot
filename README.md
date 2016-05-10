# Installation

1. Install boost

    sudo apt-get install --yes libboost-dev

2. Clone this repo

    git clone https://github.com/profbbrown/agario-bot.git
    
# Initial build

Run `make`

# Run the bot

First, start the Agar.io Clone server. In the server directory:

    npm start
    
Then run your bot

    ./bot

# Program your bot

Edit player.c

In there you will find playerMove. It is given:

* struct player me -- Info about the current player
* struct player *players -- Info about other players
* int nplayers -- Number of other players
* struct food *foods -- Array of the nearest food
* int nfood -- Length of the food array
* struct cell *virii -- Array of virii
* int nvirii -- Length of virii array
* struct cell *mass -- Array of visible mass
* int nmass -- Length of mass array

Use these arrays and variables to calculate your next move.
Return a `struct target` with your x-y move.
Both the x and y values are deltas (how far to move in a particular
direction).