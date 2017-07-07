//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2014/06/19
// File: basic_demo.cpp
#include "stdafx.h"
#include "basic_demo.h"
#include "hello_sample/hello_sample.h"

HINSTANCE							hInst;
TCHAR								szTitle[100] = _T("enn sample");
TCHAR								szWindowClass[100] = _T("enn sample");
enn::SampleFrame*					g_sample_ = 0;
int									W = 1024, H = 768;
HWND								g_hwnd = 0;
bool								g_running = true;


ATOM								MyRegisterClass(HINSTANCE hInstance);
BOOL								InitInstance(HINSTANCE, int);
LRESULT CALLBACK					WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK					About(HWND, UINT, WPARAM, LPARAM);
void								centerWindow();
void								createSample();
void								initSample();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) return FALSE;

	centerWindow();

	createSample();
	initSample();

	// run msg idle
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (g_running)
		{
			if (!g_sample_->idle())
			{
				DestroyWindow(g_hwnd);
			}
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BASIC_DEMO));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hwnd = hWnd;

	return TRUE;
}

void getDeviceSize(int& w, int& h)
{
	RECT rc;
	GetClientRect(g_hwnd, &rc);
	w = rc.right - rc.left;
	h = rc.bottom - rc.top;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		g_running = false;
		g_sample_->destroy();
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		if (SIZE_MAXIMIZED == wParam || SIZE_RESTORED == wParam)
		{
			int w, h;
			getDeviceSize(w, h);
			if (g_sample_) g_sample_->resize(w, h);
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

inline int getRectWidth(const RECT& rc)
{
	return rc.right - rc.left;
}

inline int getRectHeight(const RECT& rc)
{
	return rc.bottom - rc.top;
}

void centerWindow()
{
	RECT rectClient = { 0, 0, W, H };
	AdjustWindowRect(&rectClient, GetWindowLongPtr(g_hwnd, GWL_STYLE), FALSE);

	int width = getRectWidth(rectClient);
	int height = getRectHeight(rectClient);

	RECT rectWork = { 0 };
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWork, 0);

	int rectWorkHeight = getRectHeight(rectWork);
	if (height > rectWorkHeight)
	{
		width = width * rectWorkHeight / height;
		height = rectWorkHeight;
	}

	rectClient.left = rectWork.left + (getRectWidth(rectWork) - width) / 2;
	rectClient.top = rectWork.top + (rectWorkHeight - height) / 2;

	MoveWindow(g_hwnd, rectClient.left, rectClient.top, width, height, FALSE);
}

void createSample()
{
	g_sample_ = ENN_NEW enn::HelloSample();
}

void initSample()
{
	char exeName[MAX_PATH];
	GetModuleFileNameA(hInst, exeName, sizeof(exeName));

	static char exePath[MAX_PATH];
	char* filePart;
	GetFullPathNameA(exeName, MAX_PATH, exePath, &filePart);
	*filePart = 0;

	enn::SampleCreateContext ctx;
	ctx.windowWidth = W;
	ctx.windowHeight = H;
	ctx.handleWindow = g_hwnd;
	ctx.exe_path = exePath;

	g_sample_->create(ctx);
}
