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
    sio::message::list events = e.get_messages();
    
    // 0 - visible players
    // 1 - visible food
    // 2 - visible mass
    // 3 - visible virus
    
    std::shared_ptr<sio::array_message> foods = 
        std::static_pointer_cast<sio::array_message>(events[1]);
    std::shared_ptr<sio::array_message> players =
        std::static_pointer_cast<sio::array_message>(events[0]);
    std::shared_ptr<sio::array_message> mass =
        std::static_pointer_cast<sio::array_message>(events[2]);
    std::shared_ptr<sio::array_message> virii =
        std::static_pointer_cast<sio::array_message>(events[3]);

    //std::shared_ptr<sio::array_message> foodList = 
    //    std::static_pointer_cast<sio::array_message>(dataList->at(1));

    // Get current player and other players
    struct player me;
    struct player *playerArr = (struct player *)malloc((players->size() - 1) * sizeof(struct player));
    int pa_count = 0;
    
    for (int i = 0; i < players->size(); i++)
    {
        std::shared_ptr<sio::object_message> p =
            std::static_pointer_cast<sio::object_message>(players->at(i));
        if (! p->has("id"))
        {
            me.x = p->at("x")->get_double();
            me.y = p->at("y")->get_double();
            me.totalMass = p->at("massTotal")->get_double();
            me.cells = NULL;  // TODO
        }
        else
        {
            playerArr[pa_count].x = p->at("x")->get_double();
            playerArr[pa_count].y = p->at("y")->get_double();
            playerArr[pa_count].totalMass = p->at("massTotal")->get_double();
            playerArr[pa_count].cells = NULL; // TODO
            pa_count++;
        }
    }
    
    // Get food list
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
    
    // Get virus list
    struct cell *virusArr = (struct cell *)malloc(virii->size() * sizeof(struct cell));

    for (int i = 0; i < virii->size(); i++)
    {
        if (virii->at(i)->get_flag() != sio::message::flag_object) return;
        std::shared_ptr<sio::object_message> virus = 
            std::static_pointer_cast<sio::object_message>(virii->at(i));
        //std::cout << i << ": " << virus->at("x")->get_double() << "," << virus->at("y")->get_double() << std::endl;
        virusArr[i].x = virus->at("x")->get_double();
        virusArr[i].y = virus->at("y")->get_double();
        virusArr[i].mass = virus->at("mass")->get_double();
    }
    
    // Get mass list
    struct cell *massArr = (struct cell *)malloc(mass->size() * sizeof(struct cell));

    for (int i = 0; i < mass->size(); i++)
    {
        if (mass->at(i)->get_flag() != sio::message::flag_object) return;
        std::shared_ptr<sio::object_message> m = 
            std::static_pointer_cast<sio::object_message>(mass->at(i));
        //std::cout << i << ": " << m->at("x")->get_double() << "," << m->at("y")->get_double() << std::endl;
        massArr[i].x = m->at("x")->get_double();
        massArr[i].y = m->at("y")->get_double();
        massArr[i].mass = m->at("mass")->get_double();
    }
    
    
    struct target t = playerMove(me,
                                 playerArr, pa_count, 
                                 foodArr, foods->size(),
                                 virusArr, virii->size(),
                                 massArr, mass->size());
    
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
    //h.socket()->on("playerDied", &onMessage);
    //h.socket()->on("playerDisconnect", &onMessage);
    //h.socket()->on("playerJoin", &onMessage);
    //h.socket()->on("leaderboard", &onMessage);
    //h.socket()->on("serverMSG", &onMessage);
    //h.socket()->on("serverSendPlayerChat", &onMessage);
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