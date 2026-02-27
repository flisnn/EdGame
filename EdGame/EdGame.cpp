// EdGame.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "EdGame.h"
#include "Commands.h"
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdbool.h>

#define MAX_LOADSTRING 100

bool app_closing = false;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

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

const int widthgarden = 5;
const int heightgarden = 5;

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

void wheat(HDC hdc, int fy, int fx) {
    SelectObject(hdc, hblackPen);
    SelectObject(hdc, hyellowBrush);
    Rectangle(hdc, fx * widthbed, fy * heightbed, fx * widthbed + widthbed, fy * heightbed + heightbed);
}

void empty(HDC hdc, int fy, int fx) {
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
            garden[i][j] = rand() % 2;
        }
    }
    gardenfill = true;
}

void drawgarden(HDC hdc) {
    for (int i = 0; i < heightgarden; i++) {
        for (int j = 0; j < widthgarden; j++) {
            if (garden[i][j] == 0) {
                empty(hdc, i, j);
            }
            else if (garden[i][j] == 1) {
                wheat(hdc, i, j);
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

const int GROW_TIME = 30000;
const int GROW_STAGES = 5;

// Обновите структуру
typedef struct {
    bool active;
    int x;
    int y;
    int start_time;
    int stage;
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
    if (garden[drone.y][drone.x] == 0) {
        int slot = find_free_planting();
        if (slot != -1) {
            plantings[slot].active = true;
            plantings[slot].x = drone.x;
            plantings[slot].y = drone.y;
            plantings[slot].start_time = GetTickCount();
            plantings[slot].stage = 0;
            plantings[slot].total_grow_time = GROW_TIME;

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
    if (garden[drone.y][drone.x] != 0) {
        wait_with_check(1000);
        garden[drone.y][drone.x] = 0;
        InvalidateRect(FindWindow(szWindowClass, NULL), NULL, FALSE);
    }
    else {
        MessageBox(NULL, L"Не могу собрать урожай - урожай и так собран!", L"Предупреждение", MB_OK | MB_ICONWARNING);
    }
}

int whattype() {
    return garden[drone.y][drone.x];
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

    int windowWidth = widthgarden * widthbed + 16;
    int windowHeight = heightgarden * heightbed + 60;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Запускаем скрипт
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
                    // Завершаем рост
                    garden[plantings[i].y][plantings[i].x] = 1;
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
                    SelectObject(hdcMem, hblackPen);
                    SelectObject(hdcMem, hyellowBrush);
                    Rectangle(hdcMem, j * widthbed, i * heightbed,
                        j * widthbed + widthbed, i * heightbed + heightbed);
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

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);

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