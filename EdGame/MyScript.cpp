// MyScript.c

#include "Commands.h"

SCRIPT_BEGIN();

    for (int row = 0; row < 10; row++) {
        if (row % 2 == 0) {
            for (int col = 0; col < 9; col++) {
                if (whattype() == 0) {
                    plant();
                }
                else {
                    harvest();
                    plant();
                }
                move(right);
            }
        }
        else {
            for (int col = 0; col < 9; col++) {
                if (whattype() == 0) {
                    plant();
                }
                else {
                    harvest();
                    plant();
                }
                move(left);
            }
        }
        if (row < 9) {
            if (whattype() == 0) {
                plant();
            }
            else {
                harvest();
                plant();
            }
            move(down);
        }
    }
    if (whattype() == 0) {
        plant();
    }
    else {
        harvest();
        plant();
    }
    ResetPosition();

SCRIPT_END();