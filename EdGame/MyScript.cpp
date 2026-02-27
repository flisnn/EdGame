// MyScript.c
#include "Commands.h"

void ph() {
    if (whattype() == 0) {
        plant();
    }
    else {
        harvest();
        plant();
    }
}

void execute_script() {
    /*for (int row = 0; row < getheightofgarden(); row++) {
        if (row % 2 == 0) {
            for (int col = 0; col < getwidthofgarden() - 1; col++) {
                ph();
                move(right);
            }
        }
        else {
            for (int col = 0; col < getwidthofgarden() - 1; col++) {
                ph();
                move(left);
            }
        }
        if (row < getheightofgarden() - 1) {
            ph();
            move(down);
        }
    }
    ph();*/
    
    for (int row = 0; row < getheightofgarden(); row++) {
        for (int col = 0; col < getwidthofgarden(); col++) {
            droneto(row, col);
            ph();
        }
    }
    resetposition();
}