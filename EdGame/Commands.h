// Commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum {
    up,
    down,
    left,
    right
} Direction;

// Макросы для скриптов
#define SCRIPT_BEGIN() void execute_script() { 
#define SCRIPT_END() } 

void move(Direction dir); 
void plant();
void harvest();
int whattype();
void ResetPosition();
void wait(int milliseconds);
//void set_speed(int speed);

#endif