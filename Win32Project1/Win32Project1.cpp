// Win32Project1.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Win32Project1.h"
#include "vg\openvg.h"
#include "egl\egl.h"
#include "vgLoadImage.h"
#include "DXUTsound.h"

#define KEY_DOWN(code) (GetAsyncKeyState(code)&0x8000)

static EGLDisplay currentDisplay;
static EGLSurface currentSurface;
static EGLContext currentContext;


VGImage planeImages;
VGImage back;
VGImage missileImage;

CSoundManager* sm = NULL;
CSound* shot_bgm;
CSound* missle;

struct PLANE {
	int x, y;
	int count;
	int throwing;
}plane;

struct {
	int shoot, x, y;
	int direction;
}missile[10];





void timerproc();
void draw();

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

												// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.


	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	MSG msg;

	// �⺻ �޽��� �����Դϴ�.
	DWORD lastTime = GetTickCount();
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, 1))
		{
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT) break;
		}
		DWORD curTime = GetTickCount();
		if (curTime - lastTime>32) // 30 frame per second
		{
			lastTime = lastTime + 33;
			timerproc();
		}
	}



	return (int)msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);


	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
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
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CREATE:
	{
		currentDisplay = eglGetDisplay(GetDC(hWnd));
		eglInitialize(currentDisplay, NULL, NULL);
		eglBindAPI(EGL_OPENVG_API);
		EGLint conf_list[] = { EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_NONE };
		EGLConfig config;
		EGLint    num_config;
		eglChooseConfig(currentDisplay, conf_list, &config, 1, &num_config);
		currentSurface = eglCreateWindowSurface(currentDisplay, config, hWnd, NULL);
		currentContext = eglCreateContext(currentDisplay, 0, NULL, NULL);
		eglMakeCurrent(currentDisplay, currentSurface, currentSurface, currentContext);

		planeImages = vgLoadImage(TEXT("plane.png"));
		back = vgLoadImage(TEXT("background.png"));
		missileImage = vgLoadImage(TEXT("missile.png"));

		plane.y = 50;
		plane.x = 30;
		plane.count = 0;
		plane.throwing = false;

		sm = new CSoundManager();
		sm->Initialize(hWnd, DSSCL_NORMAL);
		sm->Create(&shot_bgm, TEXT("background_sound.wav"), 0, GUID_NULL);
		sm->Create(&missle, TEXT("missile_sound.wav"), 0, GUID_NULL);
		shot_bgm->Play(0, DSBPLAY_LOOPING);

		for (int i = 0; i<10; i++)
			missile[i].shoot = 0;
	}
	break;
	case WM_KEYDOWN: {
		if (wParam == VK_SPACE) {
				missle->Reset();
				missle->Play(0);
			for (int i = 0; i < 100; i++) {
				if (missile[i].shoot == false) {
					missile[i].shoot = true;
					missile[i].x = plane.x + 5;
					missile[i].y = plane.y + 5;
					break;
				}
			}
		}
		break;

	} break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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

void timerproc()
{
	if (KEY_DOWN(VK_UP) && plane.y <= 500) plane.y += 10;
	if (KEY_DOWN(VK_DOWN) && plane.y >= 5) plane.y -= 10;
	if (KEY_DOWN(VK_LEFT) && plane.x >= 5) plane.x -= 10;
	if (KEY_DOWN(VK_RIGHT) && plane.x <= 700) plane.x += 10;

	if (plane.throwing) plane.count++;



	for (int i = 0; i<10; i++) {
		if (missile[i].shoot) {
			missile[i].x += 50;

			if (missile[i].x>1000) missile[i].shoot = false;
		}
	}
	draw();
}


void draw()
{
	eglMakeCurrent(currentDisplay, currentSurface, currentSurface, currentContext);
	VGfloat clearColor[4] = { 1,0.5,0,1 };
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	vgClear(0, 0, 640, 480);
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

	vgLoadIdentity();

	vgDrawImage(back);


	vgLoadIdentity();
	vgTranslate(plane.x, plane.y);


	vgDrawImage(planeImages);


	for (int i = 0; i<10; i++) {
		if (missile[i].shoot) {
			vgLoadIdentity();
			vgTranslate(missile[i].x, missile[i].y);
			vgDrawImage(missileImage);
		}
	}

	eglSwapBuffers(currentDisplay, currentSurface);
}

