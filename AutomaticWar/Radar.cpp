#include "Radar.h"

#include <iostream>

using namespace std;

Radar& Radar::Instance()
{
	static Radar radar;
	return radar;
}

Radar::Radar()
{
}

Radar::~Radar()
{
}

void Radar::Initialize(int width, int height)
{
	_width = width;
	_height = height;

	const wchar_t* const myclass = L"myclass";
	WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
		0, 0, GetModuleHandle(0), LoadIcon(0,IDI_APPLICATION),
		LoadCursor(0,IDC_ARROW), HBRUSH(COLOR_WINDOW + 1),
		0, myclass, LoadIcon(0,IDI_APPLICATION) };

	if (RegisterClassEx(&wndclass))
	{
		_hWnd = CreateWindowEx(0, myclass, L"Radar",
			WS_OVERLAPPEDWINDOW, 1000, 0,
			width, height, 0, 0, GetModuleHandle(0), 0);

		if (_hWnd)
			ShowWindow(_hWnd, SW_SHOWDEFAULT);
	}
}

void Radar::Update()
{
	if (!_hWnd)
		return;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(_hWnd, &ps);

	//// double buffering
	//{
	//	HDC hdcMem = CreateCompatibleDC(hdc);
	//	HBITMAP hBmpMem = CreateCompatibleBitmap(hdc, _width, _height);

	//	HANDLE hOld = SelectObject(hdcMem, hBmpMem);

	//	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	//	Rectangle(hdcMem, 0, 0, _width, _height);

	//	SelectObject(hdc, GetStockObject(GRAY_BRUSH));
	//	SelectObject(hdcMem, GetStockObject(WHITE_PEN));
	//	for (RECT& rect : _drawnRectangles)
	//		DrawRectangle(hdcMem, rect);

	//	SelectObject(hdcMem, GetStockObject(BLACK_PEN));
	//	for (RECT& rect : _permanentRectangles)
	//		DrawRectangle(hdcMem, rect);

	//	for (RECT& rect : _temporaryRectangles)
	//		DrawRectangle(hdcMem, rect);

	//	BitBlt(hdc, 0, 0, _width, _height, hdcMem, 0, 0, SRCCOPY);

	//	SelectObject(hdcMem, hOld);
	//	DeleteObject(hBmpMem);
	//	DeleteDC(hdcMem);
	//}

	SelectObject(hdc, GetStockObject(GRAY_BRUSH));

	SelectObject(hdc, GetStockObject(WHITE_PEN));
	for (RECT& rect : _drawnRectangles)
		DrawRectangle(hdc, rect);

	SelectObject(hdc, GetStockObject(BLACK_PEN));
	for (RECT& rect : _permanentRectangles)
		DrawRectangle(hdc, rect);

	for (RECT& rect : _temporaryRectangles)
		DrawRectangle(hdc, rect);

	EndPaint(_hWnd, &ps);

	InvalidateRect(_hWnd, nullptr, FALSE);

	_drawnRectangles = _temporaryRectangles;
	_temporaryRectangles.clear();

	MSG msg;
	if (GetMessage(&msg, 0, 0, 0))
		DispatchMessage(&msg);
}

void Radar::AddRectangle(int x, int y, int size, bool permanent)
{
	RECT rect;
	rect.left = x - size;
	rect.top = y - size;
	rect.right = x + size;
	rect.bottom = y + size;

	if (permanent)
		_permanentRectangles.push_back(rect);
	else
		_temporaryRectangles.push_back(rect);
}

void Radar::DrawRectangle(HDC hdc, RECT& rect)
{
	int top = _height - rect.top;
	int bottom = _height - rect.bottom;
	Rectangle(hdc, rect.left, top, rect.right, bottom);
}

LRESULT Radar::WindowProcedure(HWND window, unsigned int msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		std::cout << "\ndestroying window\n";
		PostQuitMessage(0);
		return 0L;

	case WM_LBUTTONDOWN:
		//std::cout << "\nmouse left button down at (" << LOWORD(lp) << ',' << HIWORD(lp) << ")\n";
		// fall thru
		break;

	case WM_PAINT:
		break;

	default:
		//std::cout << '.';
		return DefWindowProc(window, msg, wp, lp);
	}

	return 0;
}