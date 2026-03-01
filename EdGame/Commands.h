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
    seed_wheat, 
    seed_carrot,
    seed_pumpkin,
    seed_corn
} SeedType;

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

void set_seed(SeedType seed);            

CellType whattype();
int getwidthofgarden();
int getheightofgarden();
void resetposition();
void wait(int milliseconds);

#endif