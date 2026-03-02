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
int score = 0;
CellType current_seed = wheat;
const int widthgarden = 10;
const int heightgarden = 10;

DWORD timer_start = 0;
DWORD timer_end = 0;
DWORD timer_value = 0;
bool timer_running = false;

const int widthbed = 100;
const int heightbed = 100;

int garden[heightgarden][widthgarden];

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

void drawwheat(HDC hdc, int fy, int fx) {
    SelectObject(hdc, hblackPen);
    SelectObject(hdc, hyellowBrush);
    Rectangle(hdc, fx * widthbed, fy * heightbed, fx * widthbed + widthbed, fy * heightbed + heightbed);
}

void drawempty(HDC hdc, int fy, int fx) {
    SelectObject(hdc, hblackPen);
    SelectObject(hdc, hbrownBrush);
    Rectangle(hdc, fx * widthbed, fy * heightbed, fx * widthbed + widthbed, fy * heightbed + heightbed);
}

void drawdrone(HDC hdc, int fy, int fx) {
    SelectObject(hdc, hwhitePen);
    SelectObject(hdc, hblackBrush);
    Rectangle(hdc, fx * widthbed + widthbed * 0.35, fy * heightbed + heightbed * 0.25, fx * widthbed + widthbed - widthbed * 0.35, fy * heightbed + heightbed - heightbed * 0.25);
    Ellipse(hdc, fx * widthbed + 1, fy * heightbed + 1, fx * widthbed + widthbed * 0.45, fy * heightbed + heightbed * 0.45);
    Ellipse(hdc, fx * widthbed + widthbed * 0.55, fy * heightbed + 1, fx * widthbed + widthbed - 1, fy * heightbed + heightbed * 0.45);
    Ellipse(hdc, fx * widthbed + widthbed * 0.55, fy * heightbed + heightbed * 0.55, fx * widthbed + widthbed - 1, fy * heightbed + heightbed - 1);
    Ellipse(hdc, fx * widthbed + 1, fy * heightbed + heightbed * 0.55, fx * widthbed + widthbed * 0.45, fy * heightbed + heightbed - 1);
}

void fillgarden() {
    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
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
    }
    gardenfill = true;
}

void drawgarden(HDC hdc) {
    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            if (garden[i][j] == 0) {
                drawempty(hdc, i, j);
            }
            else if (garden[i][j] == 1) {
                drawwheat(hdc, i, j);
            }
        }
    }
    drawdrone(hdc, drone.y, drone.x);
}

int move_delay = 500;

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
    if (garden[drone.y][drone.x] == 0 && garden[drone.y][drone.x] < 10) {
        int slot = find_free_planting();
        if (slot != -1) {
            if (seeds) {
                plantings[slot].result = current_seed;
            }
            else {
                int type = rand() % 100;
                if (type <= 50) {
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
            plantings[slot].active = true;
            plantings[slot].x = drone.x;
            plantings[slot].y = drone.y;
            plantings[slot].start_time = GetTickCount();
            plantings[slot].stage = 0;
            if (plantings[slot].result == 1) {
                plantings[slot].total_grow_time = GROW_TIME - 15000;
            }
            else if (plantings[slot].result == 2) {
                plantings[slot].total_grow_time = GROW_TIME - 5000;
            }
            else if (plantings[slot].result == 3) {
                plantings[slot].total_grow_time = GROW_TIME;
            }
            else if (plantings[slot].result == 4) {
                plantings[slot].total_grow_time = GROW_TIME + 15000;
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
    if (garden[drone.y][drone.x] != 0 && garden[drone.y][drone.x] < 10) {
        wait_with_check(1000);
        if (info) {
            if (garden[drone.y][drone.x] == 1) {
                score += 1;
            }
            else if (garden[drone.y][drone.x] == 2) {
                score += 2;
            }
            else if (garden[drone.y][drone.x] == 3) {
                score += 8;
            }
            else if (garden[drone.y][drone.x] == 4) {
                score += 5;
            }
        } else wait_with_check(1000);
        garden[drone.y][drone.x] = 0;
        InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
    }
    else {
        MessageBox(NULL, L"Не могу собрать урожай - урожай и так собран!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

void infoswitch(bool value) {
    info = value;

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

        SetWindowPos(g_hWnd, NULL, rect.left, rect.top,
            newWidth, rect.bottom - rect.top,
            SWP_NOMOVE | SWP_NOZORDER);

        InvalidateRect(g_hWnd, NULL, TRUE);
    }
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

CellType whattype() {
    if (garden[drone.y][drone.x] == 0) {
        return empty;
    }
    else if (garden[drone.y][drone.x] == 1) {
        return wheat;
    }
    else if (garden[drone.y][drone.x] == 2) {
        return carrot;
    }
    else if (garden[drone.y][drone.x] == 3) {
        return pumpkin;
    }
    else if (garden[drone.y][drone.x] == 4) {
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

/*void set_speed(int speed) {
    move_delay = 500 / speed;
    if (move_delay < 30) move_delay = 30;
    if (move_delay > 500) move_delay = 500;
}*/

void run_script() {
    execute_script();
}
/*void loadgame() {
    FILE* fin;

    fopen_s(&fin, "c:\\Windows\\Temp\\Garden.txt", "rt");

    if (fin == NULL) {
        return;
    }

    int savedWidth, savedHeight;
    fscanf_s(fin, "%d %d", &savedWidth, &savedHeight);

    if (savedWidth == widthgarden && savedHeight == heightgarden) {
        for (int i = 0; i < heightgarden; i++) {
            for (int j = 0; j < widthgarden; j++) {
                fscanf_s(fin, "%d", &garden[i][j]);
            }
        }
    }
    fclose(fin);

    gardenfill = true;
}*/

void savegame() {
    FILE* fout;

    fopen_s(&fout, "c:\\Windows\\Temp\\Garden.txt", "wt");

    if (fout == NULL) {
        return;
    }
    fprintf(fout, "%d %d\n", widthgarden, heightgarden);

    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            fprintf(fout, "%d ", garden[i][j]);
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
            sprintf(buffer, "Конечное значение таймера: %d", timer_value);
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