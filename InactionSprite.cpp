#include "InactionSprite.h"
#include <stdexcept>

const LPCWSTR OVERLAY_CLASS_NAME = L"SPOverlayClass";

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

InactionSprite::InactionSprite(HINSTANCE hInst, HWND hWnd)
{
	if (!hInst || !hWnd)
		throw new std::invalid_argument("values must not be null");
	this->hInst = hInst;
	this->hWnd = hWnd;

    overlayClass.cbSize = sizeof(overlayClass);
    overlayClass.style = CS_HREDRAW | CS_VREDRAW;
    overlayClass.lpfnWndProc = OverlayWndProc;
    overlayClass.hInstance = hInst;
    overlayClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    overlayClass.hbrBackground = NULL; // мы сами рисуем
    overlayClass.lpszClassName = OVERLAY_CLASS_NAME;

    RegisterClassExW(&overlayClass);
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InactionSprite* inactionSprite;

    if (message == WM_NCCREATE) {
        // достаём указатель на объект из lpCreateParams
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        inactionSprite = (InactionSprite*)cs->lpCreateParams;

        // сохраняем указатель в данных окна
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)inactionSprite);
    }
    else {
        // достаём указатель на объект из данных окна
        inactionSprite = (InactionSprite*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }
    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
    {
        inactionSprite->SetTimerInactionValue(0);
        inactionSprite->StopAnimation();
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT canvasRectangle;
        GetClientRect(hWnd, &canvasRectangle);

        // двойная буферизация
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, canvasRectangle.right, canvasRectangle.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);

        // фон
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(mem, &canvasRectangle, brush);
        DeleteObject(brush);

        // спрайт
        HBRUSH spr = CreateSolidBrush(RGB(255, 100, 0));
        RECT rSprite = { inactionSprite->x, inactionSprite->y, inactionSprite->x + inactionSprite->w, inactionSprite->y + inactionSprite->h };
        FillRect(mem, &rSprite, spr);
        DeleteObject(spr);

        // копирование в окно
        BitBlt(hdc, 0, 0, canvasRectangle.right, canvasRectangle.bottom, mem, 0, 0, SRCCOPY);

        // очистка
        SelectObject(mem, oldBmp);
        DeleteObject(bmp);
        DeleteDC(mem);

        EndPaint(hWnd, &ps);
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return 0;
}

void InactionSprite::ValidateScreenInfo() {
    RECT rc;
    GetClientRect(hWnd, &rc);
    POINT pt = { rc.left, rc.top };
    ClientToScreen(hWnd, &pt);

    MoveWindow(hOverlay,
        pt.x, pt.y,
        rc.right - rc.left,
        rc.bottom - rc.top,
        TRUE);
}

UINT_PTR InactionSprite::GetIdTimerInaction() const
{
	return idTimerInaction;
}

void InactionSprite::SetIdTimerInaction(UINT_PTR value) 
{
	idTimerInaction = value;
}

int InactionSprite::GetTimerInactionValue() const
{
	return timerInactionValue;
}

void InactionSprite::SetTimerInactionValue(int value)
{
	timerInactionValue = value;
}

int InactionSprite::IncTimerInactionValue()
{
	timerInactionValue++;
	return timerInactionValue;
}

UINT_PTR InactionSprite::GetIdTimerAnimation() const
{
    return this->idTimerAnimation;
}

bool InactionSprite::GetIsVisible()
{
	return isVisible;
}

void InactionSprite::StartAnimation()
{
	if (this->isVisible) return;

	this->isVisible = true;
	this->x = 0;
	this->y = 0;
	this->vx = SPRITE_VELOSITY_X;
	this->vy = SPRITE_VELOSITY_Y;

    // показать оверлей

    RECT rc;
    GetClientRect(this->hWnd, &rc);
    POINT pt = { rc.left, rc.top };
    ClientToScreen(this->hWnd, &pt);

    hOverlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        OVERLAY_CLASS_NAME,
        L"",
        WS_POPUP,
        pt.x, pt.y,
        rc.right - rc.left, rc.bottom - rc.top,
        this->hWnd, 
        NULL, hInst, this);

    ShowWindow(hOverlay, SW_SHOW);
    UpdateWindow(hOverlay);

    // спрятать системный курсор (опционально)
    ShowCursor(FALSE);

	SetTimer(hWnd, idTimerAnimation, 2, NULL);
}

void InactionSprite::StopAnimation()
{
	if (!this->isVisible) return;
	this->isVisible = false;
    if (hOverlay)
    {
        DestroyWindow(hOverlay);
        hOverlay = NULL;
    }
    ShowCursor(TRUE);
	KillTimer(hWnd, idTimerAnimation);
}

void InactionSprite::ShowNextFrame()
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    int maxX = rc.right - w;
    int maxY = rc.bottom - h;

    x += vx;
    y += vy;

    if (x < 0) 
    { 
        x = 0; 
        vx = -vx;
    }
    if (y < 0) 
    { 
        y = 0; 
        vy = -vy; 
    }
    if (x > maxX) 
    { 
        x = maxX; 
        vx = -vx; 
    }
    if (y > maxY) 
    { 
        y = maxY; 
        vy = -vy; 
    }
    InvalidateRect(hOverlay, NULL, TRUE);
}