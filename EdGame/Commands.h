// Commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    up,
    down,
    left,
    right
} Direction;

typedef enum {
    empty = 0,    
    wheat = 1,     
    carrot = 2,    
    pumpkin = 3,   
    corn = 4,     
    growing    
} CellType;


void randomswitch(int value);
void set_garden_size(int new_height, int new_width);

void move(Direction dir);
void plant();                 
void harvest();
CellType whattype();
CellType whattype(int x, int y);
int getwidthofgarden();
int getheightofgarden();
void droneto(int y, int x);
void resetposition();
void wait(int milliseconds);
void loadgame();
void savegame();



extern bool seeds;
void set_seed(CellType seed);
void set_seed(int seed);
void seedswitch(bool value);



extern bool info;
void infoswitch(bool value);
void start_timer();
void stop_timer();
void get_elapsed_time();



extern bool shop;
void shopswitch(bool value);


#endif