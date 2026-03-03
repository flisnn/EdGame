// MyScript.cpp
#include "Commands.h"


//void execute_script() {
//
//    //Ваш код
//
//}

//LVL 0
//x=5
//y=5
//void execute_script() {
//    set_garden_size(5, 5);
//    move(right);
//    move(down);
//    move(left);
//    move(up);
//    if(whattype() == wheat ) {
//        harvest();
//    } else {
//        plant();
//    }
//    //в целом тут могут быть любые функции из первого блока команд что пользователь просто познакомился что может эта игра
//}


//LVL 1
//x=10
//y=1
//void execute_script() {
//    /*int y = 0;
//    cycle:
//    if (y < getwidthofgarden() - 1) {
//        move(right);
//        y++;
//        goto cycle;
//    }*/
//    /*int y = 0;
//    while (y < getwidthofgarden() - 1) {
//        move(right);
//        y++;
//    };*/
//    /*int y = 0;
//    do {
//        move(right);
//        y++;
//    } while (y < getwidthofgarden()-1);*/
//
//    //вместо getwidthofgarden() может быть и 10 не будет ошибкой
//}

//LVL 2
//можно объеденить LVL 1 и LVL 2
//void execute_script() {
//    //теперь усложняем предыдущую задачу можно сделать много вариантов по типу 
//    //сделать все грядки пустым, засадить все грядки, чередование и так далее
//    int y = 0;
//    while (y < getwidthofgarden() - 1) {
//        move(right);
//        if (whattype() == 1) {
//            harvest();
//        } else {
//            plant();
//        }
//        /*switch (whattype()) {
//        case empty:
//            plant();
//            break;
//        case wheat:
//            harvest;
//            break;
//        }*/
//        y++;
//    };
//}

//LVL 3
//x=5
//y=5
//bool doharvest() {
//    if (whattype() == 1)
//        return true;
//    else
//        return false;
//}
//
//int doleft(int x) {
//    if (x % 2 == 0)
//        return true;
//    else
//        return false;
//}
//
//void gotodown() {
//    int x = 0;
//    while (x < getheightofgarden() - 1) {
//        move(down);
//        if (doleft(x)) move(left);
//        x++;
//    }
//}
//void gotoright() {
//    int y = 0;
//    while (y < getwidthofgarden() - 1) {
//        if (doharvest())harvest();
//        move(right);
//        y++;
//    }
//}
//
//void execute_script() {
//    set_garden_size(5, 5);
//    gotoright();
//    gotodown();
//}

//LVL 4.1
//void execute_script() {
//    for (int i = 0; i < getwidthofgarden() - 1; i++) {
//        move(right);
//    }
//}
//LVL 4.2.1
//void execute_script() {
//    set_garden_size(5, 5);
//    for (int row = 0; row < getheightofgarden(); row++) {
//        for (int col = 0; col < getwidthofgarden(); col++) {
//            droneto(row, col);
//        }
//    }
//}
//LVL 4.2.2
//void execute_script() {
//    set_garden_size(5, 5);
//    for (int row = 0; row < getheightofgarden(); row++) {
//        if (row % 2 == 0) {
//            for (int col = 0; col < getwidthofgarden() - 1; col++) {
//                move(right);
//            }
//        }
//        else {
//            for (int col = 0; col < getwidthofgarden() - 1; col++) {
//                move(left);
//            }
//        }
//        if (row < getheightofgarden() - 1) {
//            move(down);
//        }
//    }
//    //показываем как обычно ходим по массиву и не оптимальный вариант хождения по массиву 
//}
//LVL 4.3
//можно сделать много задач на узоры вариантов очень много 
//как например сделать только с одним видом семян так и сразу с четырьмя
//void execute_script() {
//    set_garden_size(5, 5);
//    seedswitch(true);
//    for (int row = 0; row < getheightofgarden(); row++) {
//        for (int col = 0; col < getwidthofgarden(); col++) {
//            droneto(row, col);
//            if (whattype() != 0) {
//                harvest();
//                if (col != 0 && col < 5) {
//                    set_seed(col);
//                    plant();
//                }
//            }
//        }
//    }
//}
//это один из примеров засадки можно сделать это же по другому или другие виды засадки

//LVL 5
//сами прописывают функцию для сохранения или загрузки файлов в папку и из папки эти функция
//сохранение и загрузка уже есть в EDGame.cpp но им нужно прописать сохранение и загрузку в/из других файлов и поиграться с ней
//void execute_script() {
//
//}

//LVL 6
//x=8
//y=8
//тут мы уже показываем визуально как эработает рекурсия
//или они сами напишут его взяв из лекции midashand()
//bool visited[8][8] = { false };
//
//bool find_exit(int x, int y) {
//    if (x < 0 || x >= 8 || y < 0 || y >= 8) return false;
//    if (whattype(y, x) == 0 || visited[y][x]) return false;
//
//    visited[y][x] = true;
//    droneto(y, x);
//
//    if (whattype(y, x) == 2) {
//        return true;
//    }
//
//    if (find_exit(x + 1, y)) return true;
//    if (find_exit(x - 1, y)) return true;
//    if (find_exit(x, y + 1)) return true;
//    if (find_exit(x, y - 1)) return true;
//
//    return false;
//}
//
//void execute_script() {
//    loadgame();
//    find_exit(0, 0);
//}

//LVL 7
//void execute_script() {
//    //тут уже работа со стеком а именно магазин
//}

//void ph() {
//    if (whattype() == empty) {
//        plant();
//    }
//    else {
//        harvest();
//        plant();
//    }
//}
//
//void execute_script() {
//    randomswitch(2);
//    set_garden_size(5,5);
//    infoswitch(true);
//    seedswitch(true);
//    
//    set_seed(pumpkin);
//    start_timer();
//    /*for (int row = 0; row < getheightofgarden(); row++) {
//        if (row % 2 == 0) {
//            for (int col = 0; col < getwidthofgarden() - 1; col++) {
//                ph();
//                move(right);
//            }
//        }
//        else {
//            for (int col = 0; col < getwidthofgarden() - 1; col++) {
//                ph();
//                move(left);
//            }
//        }
//        if (row < getheightofgarden() - 1) {
//            ph();
//            move(down);
//        }
//    }
//    ph();*/
//    
//    for (int row = 0; row < getheightofgarden(); row++) {
//        for (int col = 0; col < getwidthofgarden(); col++) {
//            droneto(row, col);
//            ph();
//        }
//    }
//    stop_timer();
//    get_elapsed_time();
//    resetposition();
//}