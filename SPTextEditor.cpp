// SPTextEditor.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "SPTextEditor.h"
#include "InactionSprite.h"
#include <stdexcept>

#define MAX_LOADSTRING 100
#define BTN_OPEN_ID 1001
#define BTN_SAVE_ID 1002
#define BTN_CLOSE_ID 1003
#define BTN_CUT_ID 2001
#define BTN_COPY_ID 2002
#define BTN_PASTE_ID 2003
#define BTN_ABOUT_ID 3001

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

static InactionSprite* inactionSprite = NULL;

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
    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SPTEXTEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPTEXTEDITOR));

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

    return (int) msg.wParam;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPTEXTEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SPTEXTEDITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowEx(
       0,  
       szWindowClass,          // класс окна
       szTitle,        // заголовок
       WS_OVERLAPPEDWINDOW, // обычный стиль окна
       CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, // координаты и размеры
       NULL, NULL, hInstance, NULL
   );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (inactionSprite == NULL)
        inactionSprite = new InactionSprite(hInst, hWnd);

    switch (message)
    {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
    {
        inactionSprite->SetTimerInactionValue(0);
        inactionSprite->StopAnimation();
        break;
    }
    case WM_SIZE:
    {
        inactionSprite->ValidateScreenInfo();
        break;
    }
    case WM_TIMER:
    {
        if (inactionSprite->GetIsVisible())
            inactionSprite->ShowNextFrame();
        else if (inactionSprite->IncTimerInactionValue() >= 5)
            inactionSprite->StartAnimation();

        break;
    }
    case WM_CREATE:
    {
        // создаём главное меню
        SetTimer(hWnd, inactionSprite->GetIdTimerInaction(), 1000, NULL);
        HMENU hMainMenu = CreateMenu();

        HMENU hFileMenu = CreateMenu();
        AppendMenu(hFileMenu, MF_STRING, BTN_OPEN_ID, L"Открыть");
        AppendMenu(hFileMenu, MF_STRING, BTN_SAVE_ID, L"Сохранить");
        AppendMenu(hFileMenu, MF_STRING, BTN_CLOSE_ID, L"Выход");

        HMENU hEditMenu = CreateMenu();
        AppendMenu(hEditMenu, MF_STRING, BTN_CUT_ID, L"Вырезать");
        AppendMenu(hEditMenu, MF_STRING, BTN_COPY_ID, L"Копировать");
        AppendMenu(hEditMenu, MF_STRING, BTN_PASTE_ID, L"Вставить");

        // добавляем подменю в главное меню
        AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"&Файл");
        AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"&Правка");
        AppendMenu(hMainMenu, MF_STRING, BTN_ABOUT_ID, L"О программе");

        // прикрепляем меню к окну
        SetMenu(hWnd, hMainMenu);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case BTN_ABOUT_ID:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
    {
        if (inactionSprite)
            KillTimer(hWnd, inactionSprite->GetIdTimerInaction());
        PostQuitMessage(0);
        break;
    }
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
