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
    empty,    
    wheat,     
    carrot,    
    pumpkin,   
    corn,     
    growing    
} CellType;

extern bool info;
extern bool seeds;
extern bool shop;

void move(Direction dir);
void droneto(int y, int x);
void plant();                 
void harvest();

void infoswitch(bool value);
void seedswitch(bool value);
void shopswitch(bool value);

void set_seed(CellType seed);            

CellType whattype();
int getwidthofgarden();
int getheightofgarden();
void resetposition();
void wait(int milliseconds);

void start_timer();
void stop_timer();
void get_elapsed_time();

#endif