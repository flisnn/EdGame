// Commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    up,
    down,
    left,
    right
} Direction;

//#define SCRIPT_BEGIN() void execute_script() {
//#define SCRIPT_END() } 

void move(Direction dir);
void droneto(int y, int x);
void plant();
void harvest();
int whattype();
int getwidthofgarden();
int getheightofgarden();
void resetposition();
void wait(int milliseconds);
//void set_speed(int speed);

#endif