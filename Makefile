LIBS=./lib
# Uncomment the line below for 32-bit
# LIBS=./lib32

bot: bot.o player.o
	clang++ -L $(LIBS) bot.o player.o -o bot -l sioclient -l boost_system -l pthread -l m && ./bot
	
bot.o: bot.cpp bot.h
	clang++ -c -std=c++11 -I ./include bot.cpp
	
player.o: player.c bot.h
	clang -c player.c -Wall

clean:
	rm -f *.o bot
