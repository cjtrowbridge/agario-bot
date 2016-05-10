bot: bot.o player.o
	clang++ -L ./lib bot.o player.o -o bot -l sioclient -l boost_system -l pthread -l m
	
bot.o: bot.cpp bot.h
	clang++ -c -std=c++11 -I ./include bot.cpp
	
player.o: player.c bot.h
	clang -c player.c -Wall

clean:
	rm -f *.o bot