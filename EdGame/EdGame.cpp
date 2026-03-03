// EdGame.cpp : Определяет точку входа для приложения.
//

#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "EdGame.h"
#include "Commands.h"
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdbool.h>

#define MAX_LOADSTRING 100

bool app_closing = false;
bool info = false;
bool seeds = false;
bool shop = false;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND g_hWnd = NULL;

// Объявление функции из скрипта
extern void execute_script(void);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EDGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDGAME));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_EDGAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//

//РАЗМЕРЫ ПОЛЯ
int widthgarden = 10;//ширина поля
int heightgarden = 1;//высота поля
//РАЗМЕРЯ ПОЛЯ

int random = 1;
int score = 0;
CellType current_seed = wheat;

DWORD timer_start = 0;
DWORD timer_end = 0;
DWORD timer_value = 0;
bool timer_running = false;

const int widthbed = 100;
const int heightbed = 100;

int** garden;
bool gardenfill = false;

struct Drone {
    int x;
    int y;
};

struct Drone drone = { 0, 0 };

HBRUSH hwhiteBrush;
HBRUSH hyellowBrush;
HBRUSH hbrownBrush;
HBRUSH hblackBrush;
HBRUSH hredBrush;
HPEN hblackPen;
HPEN hwhitePen;

void create_garden(int height, int width) {
    garden = (int**)malloc(height * sizeof(int*));

    for (int i = 0; i < height; i++) {
        garden[i] = (int*)malloc(width * sizeof(int));
    }
}

void free_garden(int height) {
    if (garden != NULL) {
        for (int i = 0; i < height; i++) {
            if (garden[i] != NULL) {
                free(garden[i]);
            }
        }
        free(garden);
        garden = NULL;
    }
}

void fillgarden() {
    if (garden == NULL) {
        MessageBox(NULL, L"Сад не инициализирован!", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            //РАНДОМ
            if (random == 0) {
                garden[i][j] = 0;
            }
            else if (random == 1) {
                garden[i][j] = 1;
            }
            else {
                int type = rand() % 100;
                if (type <= 20) {
                    garden[i][j] = 0;
                }
                else if (type <= 55) {
                    garden[i][j] = 1;
                }
                else if (type <= 75) {
                    garden[i][j] = 2;
                }
                else if (type <= 90) {
                    garden[i][j] = 4;
                }
                else if (type <= 100) {
                    garden[i][j] = 3;
                }
            }
            //РАНДОМ
        }
    }
    gardenfill = true;
}

void update_window_size() {
    if (g_hWnd != NULL) {
        RECT rect;
        GetWindowRect(g_hWnd, &rect);

        int newWidth;
        if (info) {
            newWidth = (widthgarden + 6) * widthbed + 16;
        }
        else {
            newWidth = widthgarden * widthbed + 16;
        }

        int newHeight = heightgarden * heightbed + 59;

        SetWindowPos(g_hWnd, NULL, rect.left, rect.top,
            newWidth, newHeight,
            SWP_NOMOVE | SWP_NOZORDER);

        InvalidateRect(g_hWnd, NULL, TRUE);
    }
}

void set_garden_size(int new_height, int new_width) {
    if (new_height <= 0 || new_width <= 0 ||
        new_height > 50 || new_width > 50) {
        MessageBox(NULL, L"Недопустимый размер поля!", L"Ошибка", MB_OK | MB_ICONWARNING);
        return;
    }

    int old_height = heightgarden;
    int old_width = widthgarden;

    int** temp_garden = NULL;

    if (garden != NULL) {
        temp_garden = (int**)malloc(old_height * sizeof(int*));
        for (int i = 0; i < old_height; i++) {
            temp_garden[i] = (int*)malloc(old_width * sizeof(int));
            for (int j = 0; j < old_width; j++) {
                temp_garden[i][j] = garden[i][j];
            }
        }

        for (int i = 0; i < old_height; i++) {
            free(garden[i]);
        }
        free(garden);
    }

    heightgarden = new_height;
    widthgarden = new_width;

    garden = (int**)malloc(heightgarden * sizeof(int*));
    for (int i = 0; i < heightgarden; i++) {
        garden[i] = (int*)malloc(widthgarden * sizeof(int));
    }

    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            if (temp_garden != NULL && i < old_height && j < old_width) {
                garden[i][j] = temp_garden[i][j];
            }
            else {
                //РАНДОМ
                if (random == 0) {
                    garden[i][j] = 0;
                }
                else if (random == 1) {
                    garden[i][j] = 1;
                }
                else {
                    int type = rand() % 100;
                    if (type <= 20) {
                        garden[i][j] = 0;
                    }
                    else if (type <= 55) {
                        garden[i][j] = 1;
                    }
                    else if (type <= 75) {
                        garden[i][j] = 2;
                    }
                    else if (type <= 90) {
                        garden[i][j] = 4;
                    }
                    else {
                        garden[i][j] = 3;
                    }
                }
                //РАНДОМ
            }
        }
    }

    if (temp_garden != NULL) {
        for (int i = 0; i < min(old_height, new_height); i++) {
            for (int j = 0; j < min(old_width, new_width); j++) {
                garden[i][j] = temp_garden[i][j];
            }
        }

        for (int i = 0; i < old_height; i++) {
            free(temp_garden[i]);
        }
        free(temp_garden);
    }

    update_window_size();
}

void randomswitch(int value) {
    random = value;
    fillgarden();
}

int move_delay = 200;

bool wait_with_check(int milliseconds) {
    DWORD start = GetTickCount();
    while (GetTickCount() - start < milliseconds) {
        if (app_closing) {
            return false;
        }

        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                PostQuitMessage(msg.wParam);
                app_closing = true;
                return false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }
    return true;
}

void move(Direction dir) {
    switch (dir) {
    case up:
        if (drone.y > 0) drone.y--; else
            MessageBox(NULL, L"Не могу двигаться вверх - граница поля!", L"Предупреждение", MB_OK | MB_ICONWARNING);
        break;
    case down:
        if (drone.y < heightgarden - 1) drone.y++; else
            MessageBox(NULL, L"Не могу двигаться вниз - граница поля!", L"Предупреждение", MB_OK | MB_ICONWARNING);
        break;
    case left:
        if (drone.x > 0) drone.x--; else
            MessageBox(NULL, L"Не могу двигаться влево - граница поля!", L"Предупреждение", MB_OK | MB_ICONWARNING);
        break;
    case right:
        if (drone.x < widthgarden - 1) drone.x++; else
            MessageBox(NULL, L"Не могу двигаться вправо - граница поля!", L"Предупреждение", MB_OK | MB_ICONWARNING);

        break;
    }
    wait_with_check(move_delay);
    InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
}

void droneto(int y, int x) {
    if ((y < heightgarden && y >= 0) && (x < widthgarden && x >= 0)) {
        drone.y = y;
        drone.x = x;
    }
    else
        MessageBox(NULL, L"Не могу перейти на эту клетку - граница поля!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    wait_with_check(move_delay);
    InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
}

#define MAX_PLANTINGS 50

const int GROW_TIME = 15000;
const int GROW_STAGES = 5;

// Обновите структуру
typedef struct {
    bool active;
    int x;
    int y;
    int start_time;
    int stage;
    int result;
    int total_grow_time;
} Planting;

Planting plantings[MAX_PLANTINGS];

int find_free_planting() {
    for (int i = 0; i < MAX_PLANTINGS; i++) {
        if (!plantings[i].active) return i;
    }
    return -1;
}

void plant() {
    if (garden == NULL) {
        MessageBox(NULL, L"Сад не инициализирован!", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    if (garden[drone.y] == NULL) return;
    if (garden[drone.y][drone.x] == 0 && garden[drone.y][drone.x] < 10) {
        int slot = find_free_planting();
        if (slot != -1) {
            if (seeds) {
                plantings[slot].result = current_seed;
            }
            else {
                if (random == 0) {
                    plantings[slot].result = 0;
                }
                else if (random == 1) {
                    plantings[slot].result = 1;
                }
                else {
                    int type = rand() % 100;
                    if (type <= 20) {
                        plantings[slot].result = 0;
                    }
                    else if (type <= 55) {
                        plantings[slot].result = 1;
                    }
                    else if (type <= 75) {
                        plantings[slot].result = 2;
                    }
                    else if (type <= 90) {
                        plantings[slot].result = 4;
                    }
                    else if (type <= 100) {
                        plantings[slot].result = 3;
                    }
                }
            }
            plantings[slot].active = true;
            plantings[slot].x = drone.x;
            plantings[slot].y = drone.y;
            plantings[slot].start_time = GetTickCount();
            plantings[slot].stage = 0;
            if (plantings[slot].result == 1) {
                plantings[slot].total_grow_time = GROW_TIME;
            }
            else if (plantings[slot].result == 2) {
                plantings[slot].total_grow_time = GROW_TIME;
            }
            else if (plantings[slot].result == 3) {
                plantings[slot].total_grow_time = GROW_TIME;
            }
            else if (plantings[slot].result == 4) {
                plantings[slot].total_grow_time = GROW_TIME;
            }

            garden[drone.y][drone.x] = 10;
            InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
        }
    }
    else {
        MessageBox(NULL, L"Не могу посадить урожай - на клетке уже что-то посажено!",
            L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

void harvest() {
    if (app_closing) {
        return;
    }
    if (garden == NULL) {
        MessageBox(NULL, L"Сад не инициализирован!", L"Ошибка", MB_OK | MB_ICONERROR);
        return;
    }
    if (garden[drone.y] == NULL) return;
    if (garden[drone.y][drone.x] != 0 && garden[drone.y][drone.x] < 10) {
        wait_with_check(1000);
        if (app_closing) {
            return;
        }
        if (info) {
            switch (garden[drone.y][drone.x]) {
            case 1: score += 1; break;
            case 2: score += 2; break;
            case 3: score += 8; break;
            case 4: score += 5; break;
            }
        }
        if (!app_closing && garden != NULL && garden[drone.y] != NULL) {
            garden[drone.y][drone.x] = 0;
            InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
        }

        InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
    }
    else {
        MessageBox(NULL, L"Не могу собрать урожай - урожай и так собран!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

void infoswitch(bool value) {
    info = value;

    update_window_size();
}

void seedswitch(bool value){
    seeds = value;
}

void shopswitch(bool value) {
    shop = value;
}


void set_seed(CellType seed) {
    if (seeds) {
        if (seed >= wheat && seed <= corn) {
            current_seed = seed;
        }
        else {
            MessageBox(NULL, L"Не могу выбрать этот вид семян!", L"Предупреждение", MB_OK | MB_ICONWARNING);
        }
    }
    else {
        MessageBox(NULL, L"Не могу выбрать семена - функция не доступна!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}
void set_seed(int seed) {
    switch (seed) {
    case 1:
        set_seed(wheat);
        break;
    case 2:
        set_seed(carrot);
        break;
    case 3:
        set_seed(pumpkin);
        break;
    case 4:
        set_seed(corn);
        break;
    default:
        set_seed(empty);
        break;
    }
}

CellType whattype() {
    return whattype(drone.y, drone.x);
}
CellType whattype(int y, int x) {
    if (garden == NULL) {
        return empty;
    }
    if (garden[y][x] == 0) {
        return empty;
    }
    else if (garden[y][x] == 1) {
        return wheat;
    }
    else if (garden[y][x] == 2) {
        return carrot;
    }
    else if (garden[y][x] == 3) {
        return pumpkin;
    }
    else if (garden[y][x] == 4) {
        return corn;
    }
    else {
        return growing;
    }
}

int getheightofgarden() {
    return heightgarden;
}

int getwidthofgarden() {
    return widthgarden;
}

void resetposition() {
    drone.y = 0;
    drone.x = 0;
    InvalidateRect(g_hWnd, NULL, TRUE);
}

void wait(int milliseconds) {
    wait_with_check(milliseconds);
}

void start_timer() {
    if (info) {
        timer_start = GetTickCount();
        timer_running = true;
    }
    else {
        MessageBox(NULL, L"Не могу использовать эту функцию - функция не доступна!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

void stop_timer() {
    if (info) {
        if (timer_running) {
            timer_end = GetTickCount();
            timer_running = false;
        }
        else {
            MessageBox(NULL, L"Не могу остановить таймер - таймер не запущен!", L"Предупреждение", MB_OK | MB_ICONWARNING);
        }
    }
    else {
        MessageBox(NULL, L"Не могу использовать эту функцию - функция не доступна!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

void get_elapsed_time() {
    if (info) {
        if (timer_running) {
            timer_value = GetTickCount() - timer_start;
        }
        else {
            timer_value = timer_end - timer_start;
        }
    }
    else {
        MessageBox(NULL, L"Не могу использовать эту функцию - функция не доступна!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

void loadgame() {
    FILE* fin;

    fopen_s(&fin, "c:\\Windows\\Temp\\Garden.txt", "rt");

    if (fin == NULL) {
        return;
    }

    int savedWidth, savedHeight;
    fscanf_s(fin, "%d %d", &savedWidth, &savedHeight);
    create_garden(savedHeight, savedWidth);
    heightgarden = savedHeight;
    widthgarden = savedWidth;

    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            fscanf_s(fin, "%d", &garden[i][j]);
        }
    }
    fclose(fin);

    gardenfill = true;
    resetposition();
    update_window_size();
}

void savegame() {
    FILE* fout;

    fopen_s(&fout, "c:\\Windows\\Temp\\Garden.txt", "wt");

    if (fout == NULL) {
        return;
    }
    fprintf(fout, "%d %d\n", widthgarden, heightgarden);
    int slot = 0;

    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            if (garden[i][j] >= 10) {
                fprintf(fout, "%d ", plantings[slot].result);
                slot++;
            }
            else fprintf(fout, "%d ", garden[i][j]);

        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n\n");

    fclose(fout);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int windowWidth;
    if (info) windowWidth = (widthgarden + 6) * widthbed + 16;
    else windowWidth = widthgarden * widthbed + 16;
    int windowHeight = heightgarden * heightbed + 59;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hWnd = hWnd;  // Сохраняем дескриптор
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    execute_script();

    return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        srand((unsigned int)time(NULL));

        hwhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        hyellowBrush = CreateSolidBrush(RGB(253, 219, 109));
        hbrownBrush = CreateSolidBrush(RGB(144, 77, 48));
        hblackBrush = CreateSolidBrush(RGB(0, 0, 0));
        hredBrush = CreateSolidBrush(RGB(255, 0, 0));
        hblackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        hwhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

        SetTimer(hWnd, 1, 100, NULL);
        for (int i = 0; i < MAX_PLANTINGS; i++) {
            plantings[i].active = false;
            plantings[i].total_grow_time = GROW_TIME;
        }
        create_garden(heightgarden, widthgarden);
        if (!gardenfill) {
            fillgarden();
        }
    }
    break;
    case WM_TIMER:
    {
        int current_time = GetTickCount();
        bool need_redraw = false;

        for (int i = 0; i < MAX_PLANTINGS; i++) {
            if (plantings[i].active) {
                int elapsed = current_time - plantings[i].start_time;

                if (elapsed >= plantings[i].total_grow_time) {
                    garden[plantings[i].y][plantings[i].x] = plantings[i].result;
                    plantings[i].active = false;
                    need_redraw = true;
                }
                else {
                    int stage_duration = plantings[i].total_grow_time / GROW_STAGES;
                    int new_stage = elapsed / stage_duration;
                    if (new_stage >= GROW_STAGES) new_stage = GROW_STAGES - 1;

                    if (new_stage != plantings[i].stage) {
                        plantings[i].stage = new_stage;
                        garden[plantings[i].y][plantings[i].x] = 10 + new_stage;
                        need_redraw = true;
                    }
                }
            }
        }

        if (need_redraw) {
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
    break;
    case WM_PAINT:
    {
        if (garden == NULL) break;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

        HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdcMem, &clientRect, hWhiteBrush);
        DeleteObject(hWhiteBrush);

        for (int i = 0; i < heightgarden; i++) {
            for (int j = 0; j < widthgarden; j++) {
                if (garden[i][j] == 0) {
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hbrownBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);
                }
                else if (garden[i][j] == 1) {
                    HBRUSH hBrush = CreateSolidBrush(RGB(253, 219, 109));
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);
                    DeleteObject(hBrush);
                }
                else if (garden[i][j] == 2) {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 140, 0));
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);

                    HBRUSH hOrangeBrush = CreateSolidBrush(RGB(255, 140, 0));
                    SelectObject(hdcMem, hOrangeBrush);
                    SelectObject(hdcMem, hblackPen);

                    POINT carrot[4] = {
                        {j * widthbed + 0.3 * widthbed, i * heightbed + heightbed * 0.4},
                        {j * widthbed + 0.7 * widthbed,  i * heightbed + heightbed * 0.4},
                        {j * widthbed + 0.5 * widthbed, i * heightbed + heightbed * 0.9},
                        {j * widthbed + 0.3 * widthbed, i * heightbed + heightbed * 0.4}
                    };
                    Polygon(hdcMem, carrot, 4);

                    HPEN hGreenPen = CreatePen(PS_SOLID, 2, RGB(0, 150, 0));
                    SelectObject(hdcMem, hGreenPen);

                    for (int k = 0; k < 5; k++) {
                        MoveToEx(hdcMem, j* widthbed + 0.5 * widthbed, i* heightbed + heightbed * 0.4, NULL);
                        LineTo(hdcMem, j* widthbed + 0.25 * widthbed + k * 0.125 * widthbed, i* heightbed + heightbed * 0.2);
                    }

                    DeleteObject(hOrangeBrush);
                    DeleteObject(hGreenPen);
                }
                else if (garden[i][j] == 3) {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0));
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);
                    DeleteObject(hBrush);
                    Ellipse(hdcMem,
                        j* widthbed + 0.1 * widthbed, i* heightbed + 0.3 * heightbed,
                        j* widthbed + 0.9 * widthbed, i* heightbed + 0.9 * heightbed);
                    Ellipse(hdcMem,
                        j* widthbed + 0.25 * widthbed, i* heightbed + 0.3 * heightbed,
                        j* widthbed + 0.75 * widthbed, i* heightbed + 0.9 * heightbed);
                    Ellipse(hdcMem,
                        j* widthbed + 0.4 * widthbed, i* heightbed + 0.3 * heightbed,
                        j* widthbed + 0.6 * widthbed, i* heightbed + 0.9 * heightbed);
                    HPEN hGreenPen = CreatePen(PS_SOLID, 4, RGB(0, 150, 0));
                    SelectObject(hdcMem, hGreenPen);

                    MoveToEx(hdcMem, j * widthbed + 0.5 * widthbed, i * heightbed + heightbed * 0.3, NULL);
                    LineTo(hdcMem, j* widthbed + 0.5 * widthbed, i* heightbed + heightbed * 0.2);
                    DeleteObject(hGreenPen);

                }
                else if (garden[i][j] == 4) {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 215, 0));
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);
                    Ellipse(hdcMem,
                        j* widthbed + 0.35 * widthbed, i* heightbed + 0.2 * heightbed,
                        j* widthbed + 0.65 * widthbed, i* heightbed + 0.75 * heightbed);
                    hBrush = CreateSolidBrush(RGB(34, 177, 76));
                    SelectObject(hdcMem, hBrush);
                    Chord(hdcMem,
                        j* widthbed + 0.3 * widthbed, i* heightbed + 0.2 * heightbed,
                        j* widthbed + 0.7 * widthbed, i* heightbed + 0.8 * heightbed,
                        j * widthbed + 0.25 * widthbed, i * heightbed + 0.8 * heightbed,
                        j* widthbed + 0.8 * widthbed, i* heightbed + 0.3 * heightbed);
                    Chord(hdcMem,
                        j * widthbed + 0.3 * widthbed, i * heightbed + 0.2 * heightbed,
                        j * widthbed + 0.7 * widthbed, i * heightbed + 0.8 * heightbed,
                        j * widthbed + 0.2 * widthbed, i * heightbed + 0.3 * heightbed,
                        j * widthbed + 0.75 * widthbed, i * heightbed + 0.8 * heightbed);
                    DeleteObject(hBrush);
                }
                else if (garden[i][j] >= 10 && garden[i][j] <= 14) {
                    int stage = garden[i][j] - 10;
                    int green = 135 + stage * 20;
                    int red = 240 - stage * 10;
                    HBRUSH hStageBrush = CreateSolidBrush(RGB(red, green, 100));
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hStageBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);
                    DeleteObject(hStageBrush);
                }
            }
        }

        SelectObject(hdcMem, hblackPen);
        SelectObject(hdcMem, hblackBrush);
        Rectangle(hdcMem,
            drone.x * widthbed + widthbed * 0.35,
            drone.y * heightbed + heightbed * 0.25,
            drone.x * widthbed + widthbed - widthbed * 0.35,
            drone.y * heightbed + heightbed - heightbed * 0.25);

        SelectObject(hdcMem, GetStockObject(WHITE_BRUSH));
        Ellipse(hdcMem,
            drone.x * widthbed + 1,
            drone.y * heightbed + 1,
            drone.x * widthbed + widthbed * 0.45,
            drone.y * heightbed + heightbed * 0.45);

        Ellipse(hdcMem,
            drone.x * widthbed + widthbed * 0.55,
            drone.y * heightbed + 1,
            drone.x * widthbed + widthbed - 1,
            drone.y * heightbed + heightbed * 0.45);

        Ellipse(hdcMem,
            drone.x * widthbed + widthbed * 0.55,
            drone.y * heightbed + heightbed * 0.55,
            drone.x * widthbed + widthbed - 1,
            drone.y * heightbed + heightbed - 1);

        Ellipse(hdcMem,
            drone.x * widthbed + 1,
            drone.y * heightbed + heightbed * 0.55,
            drone.x * widthbed + widthbed * 0.45,
            drone.y * heightbed + heightbed - 1);

        if (info) {
            HBRUSH hBrush = CreateSolidBrush(RGB(253, 219, 109));
            SelectObject(hdcMem, hblackPen);
            SelectObject(hdcMem, hBrush);
            Rectangle(hdcMem, (widthgarden + 1) * widthbed, 1 * heightbed,
                (widthgarden + 1)* widthbed + widthbed, 1 * heightbed + heightbed);
            DeleteObject(hBrush);



            hBrush = CreateSolidBrush(RGB(255, 140, 0));
            SelectObject(hdcMem, hblackPen);
            SelectObject(hdcMem, hBrush);
            Rectangle(hdcMem, (widthgarden + 2)* widthbed, 1* heightbed,
                (widthgarden + 2)* widthbed + widthbed, 1* heightbed + heightbed);
            HBRUSH hOrangeBrush = CreateSolidBrush(RGB(255, 140, 0));
            SelectObject(hdcMem, hOrangeBrush);
            SelectObject(hdcMem, hblackPen);
            POINT carrot[4] = {
                {(widthgarden + 2) * widthbed + 0.3 * widthbed, 1 * heightbed + heightbed * 0.4},
                {(widthgarden + 2) * widthbed + 0.7 * widthbed,  1 * heightbed + heightbed * 0.4},
                {(widthgarden + 2) * widthbed + 0.5 * widthbed, 1 * heightbed + heightbed * 0.9},
                {(widthgarden + 2) * widthbed + 0.3 * widthbed, 1 * heightbed + heightbed * 0.4}
            };
            Polygon(hdcMem, carrot, 4);
            HPEN hGreenPen = CreatePen(PS_SOLID, 2, RGB(0, 150, 0));
            SelectObject(hdcMem, hGreenPen);
            for (int k = 0; k < 5; k++) {
                MoveToEx(hdcMem, (widthgarden + 2) * widthbed + 0.5 * widthbed, 1 * heightbed + heightbed * 0.4, NULL);
                LineTo(hdcMem, (widthgarden + 2) * widthbed + 0.25 * widthbed + k * 0.125 * widthbed, 1 * heightbed + heightbed * 0.2);
            }
            DeleteObject(hOrangeBrush);
            DeleteObject(hGreenPen);



            hBrush = CreateSolidBrush(RGB(255, 165, 0));
            SelectObject(hdcMem, hblackPen);
            SelectObject(hdcMem, hBrush);
            Rectangle(hdcMem, (widthgarden + 3) * widthbed, 1 * heightbed,
                (widthgarden + 3) * widthbed + widthbed, 1 * heightbed + heightbed);
            DeleteObject(hBrush);
            Ellipse(hdcMem,
                (widthgarden + 3) * widthbed + 0.1 * widthbed, 1 * heightbed + 0.3 * heightbed,
                (widthgarden + 3) * widthbed + 0.9 * widthbed, 1 * heightbed + 0.9 * heightbed);
            Ellipse(hdcMem,
                (widthgarden + 3) * widthbed + 0.25 * widthbed, 1 * heightbed + 0.3 * heightbed,
                (widthgarden + 3) * widthbed + 0.75 * widthbed, 1 * heightbed + 0.9 * heightbed);
            Ellipse(hdcMem,
                (widthgarden + 3) * widthbed + 0.4 * widthbed, 1 * heightbed + 0.3 * heightbed,
                (widthgarden + 3) * widthbed + 0.6 * widthbed, 1 * heightbed + 0.9 * heightbed);
            hGreenPen = CreatePen(PS_SOLID, 4, RGB(0, 150, 0));
            SelectObject(hdcMem, hGreenPen);

            MoveToEx(hdcMem, (widthgarden + 3) * widthbed + 0.5 * widthbed, 1 * heightbed + heightbed * 0.3, NULL);
            LineTo(hdcMem, (widthgarden + 3) * widthbed + 0.5 * widthbed, 1 * heightbed + heightbed * 0.2);
            DeleteObject(hGreenPen);



            hBrush = CreateSolidBrush(RGB(255, 215, 0));
            SelectObject(hdcMem, hblackPen);
            SelectObject(hdcMem, hBrush);
            Rectangle(hdcMem, (widthgarden + 4) * widthbed, 1 * heightbed,
                (widthgarden + 4) * widthbed + widthbed, 1 * heightbed + heightbed);
            Ellipse(hdcMem,
                (widthgarden + 4) * widthbed + 0.35 * widthbed, 1 * heightbed + 0.2 * heightbed,
                (widthgarden + 4) * widthbed + 0.65 * widthbed, 1 * heightbed + 0.75 * heightbed);
            hBrush = CreateSolidBrush(RGB(34, 177, 76));
            SelectObject(hdcMem, hBrush);
            Chord(hdcMem,
                (widthgarden + 4) * widthbed + 0.3 * widthbed, 1 * heightbed + 0.2 * heightbed,
                (widthgarden + 4) * widthbed + 0.7 * widthbed, 1 * heightbed + 0.8 * heightbed,
                (widthgarden + 4) * widthbed + 0.25 * widthbed, 1 * heightbed + 0.8 * heightbed,
                (widthgarden + 4) * widthbed + 0.8 * widthbed, 1 * heightbed + 0.3 * heightbed);
            Chord(hdcMem,
                (widthgarden + 4) * widthbed + 0.3 * widthbed, 1 * heightbed + 0.2 * heightbed,
                (widthgarden + 4) * widthbed + 0.7 * widthbed, 1 * heightbed + 0.8 * heightbed,
                (widthgarden + 4) * widthbed + 0.2 * widthbed, 1 * heightbed + 0.3 * heightbed,
                (widthgarden + 4) * widthbed + 0.75 * widthbed, 1 * heightbed + 0.8 * heightbed);
            DeleteObject(hBrush);

            hGreenPen = CreatePen(PS_SOLID, 4, RGB(0, 150, 0));
            SelectObject(hdcMem, hGreenPen);
            SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
            Rectangle(hdcMem, (widthgarden + current_seed) * widthbed, 1 * heightbed,
                (widthgarden + current_seed) * widthbed + widthbed, 1 * heightbed + heightbed);
            DeleteObject(hGreenPen);
        }

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);
        if (info) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));

            char buffer[100];
            sprintf(buffer, "Позиция дрона: X=%d Y=%d", drone.x, drone.y);
            TextOutA(hdc, (widthgarden + 1) * widthbed, heightbed * 0.1, buffer, strlen(buffer));
            sprintf(buffer, "Ваши Баллы: %d", score);
            TextOutA(hdc, (widthgarden + 1) * widthbed, heightbed * 0.3, buffer, strlen(buffer));
            int seconds = timer_value / 1000;
            int milliseconds = timer_value % 1000;
            int minutes = seconds / 60;
            seconds = seconds % 60;
            sprintf(buffer, "Конечное значение таймера: %02d:%02d.%03d (%d мс)",
                minutes, seconds, milliseconds, timer_value);
            TextOutA(hdc, (widthgarden + 1) * widthbed, heightbed * 0.5, buffer, strlen(buffer));
            if (seeds) {
                sprintf(buffer, "Ваши выбранные семена:");
                TextOutA(hdc, (widthgarden + 1) * widthbed, heightbed * 0.8, buffer, strlen(buffer));
            }

        }
        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        app_closing = true;
        savegame();
        free_garden(heightgarden);

        Sleep(100);

        KillTimer(hWnd, 1);
        DeleteObject(hwhiteBrush);
        DeleteObject(hyellowBrush);
        DeleteObject(hbrownBrush);
        DeleteObject(hblackBrush);
        DeleteObject(hblackPen);
        DeleteObject(hwhitePen);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}