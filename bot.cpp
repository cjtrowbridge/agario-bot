#include <iostream>
#include "sio_client.h"
#include "sio_message.h"
#include "sio_socket.h"
#include <unistd.h>
#include <cstring>

#include "bot.h"

std::string BOTNAME = "bot";
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
        
        player->insert("name", sio::string_message::create(BOTNAME));
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
            std::strcpy(me.id, "");
            me.x = p->at("x")->get_double();
            me.y = p->at("y")->get_double();
            me.totalMass = p->at("massTotal")->get_double();

            std::shared_ptr<sio::array_message> cells =
                std::static_pointer_cast<sio::array_message>(p->at("cells"));
            me.cells = (struct cell *)malloc(cells->size() * sizeof(struct cell));
            for (int c = 0; c < cells->size(); c++)
            {
                std::shared_ptr<sio::object_message> cell =
                    std::static_pointer_cast<sio::object_message>(cells->at(c));
                me.cells[c].x = cell->at("x")->get_double();
                me.cells[c].y = cell->at("y")->get_double();
                me.cells[c].mass = cell->at("mass")->get_double();
                me.cells[c].radius = cell->at("radius")->get_double();
            }
            me.ncells = cells->size();
        }
        else
        {
            std::strcpy(playerArr[pa_count].id, p->at("id")->get_string().c_str());
            playerArr[pa_count].x = p->at("x")->get_double();
            playerArr[pa_count].y = p->at("y")->get_double();
            playerArr[pa_count].totalMass = p->at("massTotal")->get_double();
            
            std::shared_ptr<sio::array_message> cells =
                std::static_pointer_cast<sio::array_message>(p->at("cells"));
            playerArr[pa_count].cells = (struct cell *)malloc(cells->size() * sizeof(struct cell));
            for (int c = 0; c < cells->size(); c++)
            {
                std::shared_ptr<sio::object_message> cell =
                    std::static_pointer_cast<sio::object_message>(cells->at(c));
                playerArr[pa_count].cells[c].x = cell->at("x")->get_double();
                playerArr[pa_count].cells[c].y = cell->at("y")->get_double();
                playerArr[pa_count].cells[c].mass = cell->at("mass")->get_double();
                playerArr[pa_count].cells[c].radius = cell->at("radius")->get_double();
            }
            playerArr[pa_count].ncells = cells->size();
            
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
        massArr[i].mass = m->at("masa")->get_double();
    }
    
    // Call playerMove from player.c
    struct action a = playerMove(me,
                                 playerArr, pa_count, 
                                 foodArr, foods->size(),
                                 virusArr, virii->size(),
                                 massArr, mass->size());
    
    auto msg = std::static_pointer_cast<sio::object_message>(sio::object_message::create());
    msg->insert("x", sio::double_message::create(a.dx));
    msg->insert("y", sio::double_message::create(a.dy));
    
    h.socket()->emit("0", sio::message::list(msg));
    
    if (a.fire) h.socket()->emit("1");
    if (a.split) h.socket()->emit("2");
    
    // Free malloc'd memory
    if (me.cells) free (me.cells);
    for (int i = 0; i < pa_count; i++)
        if (playerArr[i].cells) free (playerArr[i].cells);
    if (foodArr) free(foodArr);
    if (playerArr) free(playerArr);
    if (massArr) free(massArr);
    if (virusArr) free(virusArr);
}

void onVirusSplit(sio::event &e)
{
    std::shared_ptr<sio::int_message> v = 
        std::static_pointer_cast<sio::int_message>(e.get_message());
    std::cout << "virusSplit " << v->get_int() << std::endl;
    h.socket()->emit("2", e.get_message());
}

void onRIP(sio::event &e)
{
    std::cout << "Dead!" << std::endl;
    h.socket()->close();
    exit(0);
}

void onKick(sio::event &e)
{
    std::cout << "You've been kicked off!" << std::endl;
    h.socket()->close();
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        BOTNAME = argv[1];
    }
    
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
    //h.socket()->on("playerDied", &onMessage);
    //h.socket()->on("playerDisconnect", &onMessage);
    //h.socket()->on("playerJoin", &onMessage);
    //h.socket()->on("leaderboard", &onMessage);
    //h.socket()->on("serverMSG", &onMessage);
    //h.socket()->on("serverSendPlayerChat", &onMessage);
    h.socket()->on("serverTellPlayerMove", &onTellPlayerMove);
    h.socket()->on("RIP", &onRIP);
    h.socket()->on("kick", &onKick);
    h.socket()->on("virusSplit", &onVirusSplit);
    
    //std::cerr << "ping\n";
    //h.socket()->emit("ping");
    
    h.socket()->emit("respawn");
    
    while(1)
        sleep(1);
}