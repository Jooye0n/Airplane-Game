// Win32Project1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
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

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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

	// TODO: 여기에 코드를 입력합니다.


	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	MSG msg;

	// 기본 메시지 루프입니다.
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
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
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
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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

// 정보 대화 상자의 메시지 처리기입니다.
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

