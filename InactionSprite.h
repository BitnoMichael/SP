#pragma once
#include <windows.h>

const int SPRITE_VELOSITY_X = 2;
const int SPRITE_VELOSITY_Y = 1;

class InactionSprite
{
private:
	bool isVisible = false;

	int vx = SPRITE_VELOSITY_X;
	int vy = SPRITE_VELOSITY_Y;

	UINT_PTR idTimerInaction = 1;
	UINT_PTR idTimerAnimation = 2;
	int timerInactionValue = 0;

	HINSTANCE hInst;
	HWND hWnd;

	WNDCLASSEXW overlayClass;
	HWND hOverlay = NULL;
public:
	int x = 0;
	int y = 0;

	int w = 20;
	int h = 20;


	UINT_PTR GetIdTimerInaction() const;
	void SetIdTimerInaction(UINT_PTR value);

	int GetTimerInactionValue() const;
	void SetTimerInactionValue(int value);
	int IncTimerInactionValue();

	UINT_PTR GetIdTimerAnimation() const;

	bool GetIsVisible(); 

	void StartAnimation();
	void StopAnimation();
	void ValidateScreenInfo();
	void ShowNextFrame();

	InactionSprite(HINSTANCE hInst, HWND hWnd);
};

