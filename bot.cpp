#include <iostream>
#include "sio_client.h"
#include "sio_message.h"
#include "sio_socket.h"
#include <unistd.h>

#include "bot.h"

#define PLAYERNAME "test"
#define URL "http://localhost:8080"

sio::message::ptr currentPlayerJson;
sio::client h;

void onMessage(sio::event &e)
{
    std::cout << e.get_name() << " " << e.get_message() << std::endl;
}

void onWelcome(sio::event &e)
{
    onMessage(e);
    currentPlayerJson = e.get_message();
        
    if(currentPlayerJson->get_flag() == sio::message::flag_object)
    {
        
        std::shared_ptr<sio::object_message> player = std::static_pointer_cast<sio::object_message>(currentPlayerJson);
        
        player->insert("name", sio::string_message::create(PLAYERNAME));
        player->insert("screenHeight", sio::int_message::create(600));
        player->insert("screenWidth", sio::int_message::create(600));

        /*
        std::map<std::string, sio::message::ptr> p = currentPlayerJson->get_map();

        for (std::pair<std::string, sio::message::ptr> key : p)
        {
            std::cout << key.first << " = " << key.second << std::endl;
        }
        */

        h.socket()->emit("gotit", currentPlayerJson);
    }
}

void onTellPlayerMove(sio::event &e)
{
    onMessage(e);
    
    sio::message::list events = e.get_messages();
    
    // 0 - visible players
    // 1 - visible food
    // 2 - visible mass
    // 3 - visible virus
    
    std::shared_ptr<sio::array_message> foods = 
        std::static_pointer_cast<sio::array_message>(events[1]);
    std::shared_ptr<sio::array_message> players =
        std::static_pointer_cast<sio::array_message>(events[0]);

    //std::shared_ptr<sio::array_message> foodList = 
    //    std::static_pointer_cast<sio::array_message>(dataList->at(1));

    // Get player cell
    struct player me;
    for (int i = 0; i < players->size(); i++)
    {
        std::shared_ptr<sio::object_message> p =
            std::static_pointer_cast<sio::object_message>(players->at(i));
        if (! p->has("id"))
        {
            me.x = p->at("x")->get_double();
            me.y = p->at("y")->get_double();
            me.totalMass = p->at("massTotal")->get_double();
            me.cells = NULL;
        }
    }
    
    struct food *foodArr = (struct food *)malloc(foods->size() * sizeof(struct food));

    for (int i = 0; i < foods->size(); i++)
    {
        if (foods->at(i)->get_flag() != sio::message::flag_object) return;
        std::shared_ptr<sio::object_message> food = 
            std::static_pointer_cast<sio::object_message>(foods->at(i));
        //std::cout << i << ": " << food->at("x")->get_double() << "," << food->at("y")->get_double() << std::endl;
        foodArr[i].x = food->at("x")->get_double();
        foodArr[i].y = food->at("y")->get_double();
        foodArr[i].mass = food->at("mass")->get_double();
    }
    
    struct target t = playerMove(me, foodArr, foods->size());
    
    auto msg = std::static_pointer_cast<sio::object_message>(sio::object_message::create());
    msg->insert("x", sio::double_message::create(t.dx));
    msg->insert("y", sio::double_message::create(t.dy));
    
    h.socket()->emit("0", sio::message::list(msg));
    
    free(foodArr);
    
}

int main(int argc, char *argv[])
{

    std::map<std::string, std::string> query;
    query["type"] = "player";
    h.connect(URL, query);
    
    // Give time for connection to be established
    do {
        sleep(1);
    } while (!h.opened());
    
    h.socket()->on("welcome", &onWelcome);
    h.socket()->on("gameSetup", &onMessage);
    h.socket()->on("pong", &onMessage);
    h.socket()->on("connect_failed", &onMessage);
    h.socket()->on("disconnect", &onMessage);
    h.socket()->on("gameSetup", &onMessage);
    h.socket()->on("playerDied", &onMessage);
    h.socket()->on("playerDisconnect", &onMessage);
    h.socket()->on("playerJoin", &onMessage);
    h.socket()->on("leaderboard", &onMessage);
    h.socket()->on("serverMSG", &onMessage);
    h.socket()->on("serverSendPlayerChat", &onMessage);
    h.socket()->on("serverTellPlayerMove", &onTellPlayerMove);
    h.socket()->on("RIP", &onMessage);
    h.socket()->on("kick", &onMessage);
    h.socket()->on("virusSplit", &onMessage);
    
    //std::cerr << "ping\n";
    //h.socket()->emit("ping");
    
    h.socket()->emit("respawn");
    
    while(1)
        sleep(1);
}