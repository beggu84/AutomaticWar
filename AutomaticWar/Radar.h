#pragma once

#include <Windows.h>
#include <vector>

class Radar
{
private:
	int _width{ 0 }, _height{ 0 };
	HWND _hWnd{ nullptr };

	std::vector<RECT> _permanentRectangles;
	std::vector<RECT> _temporaryRectangles;
	std::vector<RECT> _drawnRectangles;

public:
	static Radar& Instance();
private:
	Radar();
	~Radar();

public:
	void Initialize(int width, int height);
	void Update();

	void AddRectangle(int x, int reversedY, int size, bool permanent=true);
	void DrawRectangle(HDC hdc, RECT& rect);

private:
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, unsigned int msg, WPARAM wp, LPARAM lp);
};

