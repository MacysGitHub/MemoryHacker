#include <Windows.h>
#include <iostream>
#include "OpenProcessByName.h"
//
//#define FILE_MENU_NEW 1
//#define FILE_MENU_OPTIONS 2
//#define FILE_MENU_EXIT 3
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK OptionWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//void AddMenus(HWND hwnd);
//int RegisterOptionClass(HINSTANCE hInstance);
//void DisplayOptionsDialog(HWND hWnd);
//
//HWND TextBox, SendButton, TextField, OptionshWndOpt;
//
//HINSTANCE hInstance;
//
//HMENU hMenu;
//
//const wchar_t OPTIONS_CLASS_NAME[] = L"Options Class";
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
//{
//	// Register the window class.
//	const wchar_t CLASS_NAME[] = L"Memory Hacker";
//
//
//	WNDCLASSW wc = { 0 };
//	
//
//	
//
//	wc.lpfnWndProc = WindowProc;
//	wc.hInstance = hInstance;
//	wc.lpszClassName = CLASS_NAME;
//
//	
//
//	if (!RegisterClassW(&wc))
//		return -1;
//
//	int regOpt;
//	regOpt = RegisterOptionClass(hInstance);
//
//	// Create the window.
//
//	HWND hwnd = CreateWindowExW(
//		0,                              // Optional window styles.
//		CLASS_NAME,                     // Window class
//		L"Learn to Program Windows",    // Window text
//		WS_OVERLAPPEDWINDOW,            // Window style
//
//		// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//		NULL,       // Parent window    
//		NULL,       // Menu
//		hInstance,  // Instance handle
//		NULL        // Additional application data
//	);
//
//
//	if (hwnd == NULL)
//	{
//		return 0;
//	}
//
//	ShowWindow(hwnd, nCmdShow);
//
//	// Run the message loop.
//
//	MSG msg = { };
//	while (GetMessage(&msg, NULL, 0, 0) > 0)
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//
//	return 0;
//}
//
//void AddMenus(HWND hwnd) {
//	hMenu = CreateMenu();
//	HMENU hFileMenu = CreateMenu();
//	AppendMenuA(hFileMenu, MF_STRING, FILE_MENU_NEW, "New");
//	AppendMenuA(hFileMenu, MF_STRING, FILE_MENU_OPTIONS, "Options");
//	AppendMenuA(hFileMenu, MF_STRING, FILE_MENU_EXIT, "Exit");
//	AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "FILE");
//	SetMenu(hwnd, hMenu);
//}
//
//LRESULT CALLBACK OptionWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	switch (uMsg)
//	{
//	
//	case WM_CREATE:
//		//MessageBoxA(hWnd, "Options", "Options", MB_OK);
//		CreateWindow(TEXT("button"), TEXT("Data Type DWORD"),
//			WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
//			20, 20, 185, 35,
//			hWnd, (HMENU)12, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
//		CheckDlgButton(hWnd, 12, BST_CHECKED);
//		break;
//
//	case WM_COMMAND:
//		BOOL checked;
//		checked = IsDlgButtonChecked(hWnd, 12);
//		if (checked) {
//			CheckDlgButton(hWnd, 12, BST_UNCHECKED);
//			SetWindowText(hWnd, TEXT(""));
//		}
//		else {
//			CheckDlgButton(hWnd, 12, BST_CHECKED);
//			SetWindowText(hWnd, "DWORD");
//		}
//		break;
//
//	
//	case WM_CLOSE:
//		DestroyWindow(hWnd);
//		break;
//	default:
//		return DefWindowProc(hWnd, uMsg, wParam, lParam);
//	}
//	
//
//	return 0;
//}
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//	case WM_CREATE:
//		AddMenus(hwnd);
//		TextBox = CreateWindow("EDIT",
//			"",
//			WS_BORDER | WS_CHILD | WS_VISIBLE,
//			10, 300, 390, 20,
//			hwnd, (HMENU)1, NULL, NULL);
//
//		SendButton = CreateWindow("BUTTON",
//			"Send",
//			WS_VISIBLE | WS_CHILD | WS_BORDER,
//			410, 300, 65, 20,
//			hwnd, (HMENU)2, NULL, NULL);
//
//		TextField = CreateWindow("EDIT",
//			"",
//			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY,
//			10, 90, 465, 200,
//			hwnd, (HMENU)3, NULL, NULL);
//		break;
//
//	case WM_COMMAND:
//		switch (LOWORD(wParam))
//		{
//		case FILE_MENU_OPTIONS:
//			//CreateWindow("Options Class", "Options", WS_CHILD | WS_OVERLAPPEDWINDOW, 200, 200, 200, 200, hwnd, NULL, hInstance, lParam);
//			DisplayOptionsDialog(hwnd);
//			MessageBeep(MB_OK);
//			break;
//
//		case FILE_MENU_EXIT:
//			DestroyWindow(hwnd);
//		}
//		break;
//
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		return 0;
//
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hwnd, &ps);
//
//		// All painting occurs here, between BeginPaint and EndPaint.
//
//		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
//
//		EndPaint(hwnd, &ps);
//	}
//	return 0;
//	default:
//		return DefWindowProc(hwnd, uMsg, wParam, lParam);
//	}
//
//}
//
//int RegisterOptionClass(HINSTANCE hInstance) {
//	WNDCLASSW wcOptions = { 0 };
//	wcOptions.lpfnWndProc = OptionWinProc;
//	wcOptions.hInstance = hInstance;
//	wcOptions.lpszClassName = OPTIONS_CLASS_NAME;
//
//	if (!RegisterClassW(&wcOptions))
//		return -1;
//	
//}
//
//void DisplayOptionsDialog(HWND hWnd) {
//	HWND hWndOpt;
//	hWndOpt = CreateWindowExW(
//		0,                              // Optional window styles.
//		OPTIONS_CLASS_NAME,                     // Window class
//		L"Options",    // Window text
//		WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style
//
//		// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//		hWnd,       // Parent window    
//		NULL,       // Menu
//		hInstance,  // Instance handle
//		NULL        // Additional application data
//	);
//	CreateWindow(0, "Option Data Type", WS_CHILD | BS_CHECKBOX, 20, 20, 10,10, hWnd, NULL, NULL, NULL);
//}

int main(int argc, char** argv) {
	wchar_t* dest = new wchar_t[strlen(argv[1]) + 1];
	mbstowcs_s(NULL, dest, strlen(argv[1]) + 1, argv[1], strlen(argv[1]));
	OpenProcessByName(dest);
}

//HANDLE openProcess(DWORD procId) {
//	HANDLE process;
//	process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
//}