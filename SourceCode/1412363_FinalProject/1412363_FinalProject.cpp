// 1412363_FinalProject.cpp : Defines the entry point for the application.
//
//#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "1412363_FinalProject.h"
#include <windowsX.h>
#include <commctrl.h>
#include <stdio.h>
#include <time.h>
#include<iostream>
#include <fstream>
#include<string>
#include <ctime>
#include <cstdlib>
#include <shellapi.h>
#include <locale>
#include <codecvt>
#include <vector>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
using namespace std;
#define MAX_CONTENT_SIZE 300
#define MAX_TAGS_SIZE 200
#define MAX_LOADSTRING 100
#define WM_MYMESSAGE (WM_USER+ 1)
#define TIMETOSHOW 5
int remain_Time = TIMETOSHOW;
bool isClicked = false;
HHOOK hHook = NULL;
HHOOK hkCreateNew;
HINSTANCE hinstLib;
HWND g_hWnd;
bool isAutoStatistic = false;
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
struct Note{
	wstring content;
	wstring time;
	vector<wstring> tags;
};

vector<int> sort;

UINT EXIT = 100;
UINT VIEW_NOTE = 200;
UINT VIEW_STATITISTIC = 300;

static HBRUSH testBrush;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogCreateNew(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void writeListItemToFile(wstring path);
void readListItemFromFile(wstring path);
void addToList(Note note);
void loadToTreeView(HWND m_hTreeView);
int loadToListView(wstring name, HWND listview);
void preloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel, HWND m_hTreeView);
void initColumn(HWND m_hListView, int width);
void setValueForCombobox(HWND cb);
INT_PTR CALLBACK Dialog_ViewNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Dialog_ViewStatitistic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void draw(HWND hDlg, HDC hdc, int nX, int nY, DWORD dwRadius, float xStartAngle, float xSweepAngle);
void setNote(HWND staticX, int i, vector<int>sort,int sum);
INT_PTR CALLBACK Dialog_ShowDetailNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void CALLBACK timeToshowStatitistic(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);
bool CompareTag(wstring tag1, wstring tag2);
wstring getCurrentTime();
void _doInstallHook(HWND hWnd);
LRESULT CALLBACK MyHookProc(int nCode, WPARAM wParam, LPARAM lParam);
void deleteTag(wstring tag);
void deleteNote(Note note, wstring tag);
	void _doRemoveHook(HWND hWnd);
	void searchTag(wstring keyword);
	void searchNote(wstring tag, wstring keyword);

long lStyle = LVS_REPORT | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
HFONT hFont, hFont1, hFont2;
RECT rcClient;
HWND g_hTreeView, g_hListView, staticDetailNote, staticViewNote;
int selectedItemIndex = 0;
vector<Note> ListNote; // list chua danh sach cac note ma chua chia theo tag
vector<wstring>Tag;//list all tag
vector<vector<Note>>List;
vector<vector<Note>>ListToShow;

Note g_note;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	readListItemFromFile(L"test.txt");
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY1412363_FINALPROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1412363_FINALPROJECT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY1412363_FINALPROJECT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MY1412363_FINALPROJECT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		50, 50, 599, 600, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

bool KeyPressed(int a)
{
	if (GetAsyncKeyState(a))
	{
		return true;
	}
	else
	{
		return false;
	}
}

HWND btnHamburger = NULL, page;
NOTIFYICONDATA icon;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_hWnd = hWnd;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	LOGFONT lf;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	hFont = CreateFont(17, 7, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"italic");
	GetClientRect(hWnd, &rcClient);
	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;


	switch (message)
	{
	case WM_CREATE:{

					   HBITMAP hImage = (HBITMAP)LoadImage(NULL, L"images.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
					   HWND hImageView = CreateWindowEx(NULL, L"STATIC", NULL, SS_BITMAP | WS_VISIBLE | WS_CHILD, 0, 00, 900, 800, hWnd, NULL, GetModuleHandle(NULL), NULL);
					   SendMessage(hImageView, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);

					   _doInstallHook(hWnd);

					   //add icon to notification area
					   ZeroMemory(&icon, sizeof(NOTIFYICONDATA));
					   icon.cbSize = sizeof(NOTIFYICONDATA);
					   icon.uID = IDI_ICON1;					   
					   icon.hBalloonIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
					   icon.hIcon = (HICON)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
					   icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
					   icon.hWnd = hWnd;
					   icon.uCallbackMessage = WM_MYMESSAGE;
					   wcscpy(icon.szTip, L"Quick Note");
					   Shell_NotifyIcon(NIM_MODIFY, &icon); //modify name in szTip
					   Shell_NotifyIcon(NIM_ADD, &icon); //Add to notification area
					  }
		break;
 
	case WM_MYMESSAGE:{

						  switch (lParam)
						  {
						  case WM_RBUTTONDOWN:
						  {

												 MENUITEMINFO separatorBtn = { 0 };
												 separatorBtn.cbSize = sizeof(MENUITEMINFO);
												 separatorBtn.fMask = MIIM_FTYPE;
												 separatorBtn.fType = MFT_SEPARATOR;

												 HMENU hMenu = CreatePopupMenu();

												 if (hMenu) {
													 InsertMenu(hMenu, -1, MF_BYPOSITION, EXIT, L"Exit");
													 InsertMenuItem(hMenu, -1, FALSE, &separatorBtn);
													 InsertMenu(hMenu, -1, MF_BYPOSITION, VIEW_STATITISTIC, L"View statitistic");
													 InsertMenuItem(hMenu, -1, FALSE, &separatorBtn);
													 InsertMenu(hMenu, -1, MF_BYPOSITION, VIEW_NOTE, L"View notes");
													 InsertMenuItem(hMenu, -1, FALSE, &separatorBtn);
													 POINT pt;
													 GetCursorPos(&pt);
													 SetForegroundWindow(hWnd);
													 UINT clicked = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);
													 
													 if (clicked == VIEW_NOTE){
														 int result = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3),
															 hWnd, Dialog_ViewNote);
													 }
													 if (clicked == VIEW_STATITISTIC){
														 int result = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3),
															 hWnd, Dialog_ViewStatitistic);
													 }
													 if (clicked == EXIT){
														 DestroyWindow(hWnd);
													 }
													 PostMessage(hWnd, WM_NULL, 0, 0);
													 DestroyMenu(hMenu);
												 }

									  }
						  default:
							  break;
						  }
	}

	


	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		

		switch (wmId)
		{

		case ID_TOOLS_AUTOSTATISTIC:
		{
									   if (!isAutoStatistic){
										   remain_Time = TIMETOSHOW;
										   CheckMenuItem(GetMenu(hWnd), ID_TOOLS_AUTOSTATISTIC, MF_BYCOMMAND |
											   MF_CHECKED);
										   //set time tim create dialog
										   SetTimer(hWnd, IDT_TIME_RUN, 1000, (TIMERPROC)timeToshowStatitistic);
										   isAutoStatistic = true;
									   }
									   else
									   {
										   CheckMenuItem(GetMenu(hWnd), ID_TOOLS_AUTOSTATISTIC, MF_BYCOMMAND |
											   MF_UNCHECKED);
										   KillTimer(hWnd, IDT_TIME_RUN);
										   isAutoStatistic = false;
									   }
		}
		case IDM_ABOUT:
		{}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		writeListItemToFile(L"test.txt");
		_doRemoveHook(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
HWND static1, static2, static3, static4, static5, static6, static7;

HWND statictag, staticcontent,editatgs;
INT_PTR CALLBACK Dialog_ShowDetailNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	hFont2 = CreateFont(17, 7, 0, 0, FW_EXTRALIGHT, TRUE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Times New Roman"));

	switch (message)
	{
	case WM_INITDIALOG:
	{
					statictag = GetDlgItem(hDlg, IDC_STATIC);
					SendMessage(statictag, WM_SETFONT, (WPARAM)hFont2, TRUE);
					staticcontent = GetDlgItem(hDlg, IDC_EDIT1);
					SendMessage(staticcontent, WM_SETFONT, (WPARAM)hFont2, TRUE);
					editatgs = GetDlgItem(hDlg, IDC_EDIT2);
					SendMessage(editatgs, WM_SETFONT, (WPARAM)hFont2, TRUE);
					wstring tags = L"Tag: ";
					for (int i = 0; i < g_note.tags.size(); i++){
						tags += g_note.tags[i];
						if (i < g_note.tags.size() - 1){
							tags += L",";
						}
					}
					tags += L"\r\nTime:" + g_note.time;
					wstring content = L"Content: \r\n" + g_note.content;
					SetWindowText(editatgs, tags.c_str());
					SetWindowText(staticcontent, content.c_str());

					testBrush = CreateSolidBrush(RGB(255, 153, 153));

					return (INT_PTR)TRUE;

	}break;

	case WM_CTLCOLORSTATIC:
	{
		DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkMode(hdcStatic, TRANSPARENT);
	}
	case WM_CTLCOLORDLG:
	{
						   return (INT_PTR)(testBrush); 
	}break;
	case WM_COMMAND:
		int id = LOWORD(wParam);
		switch (id) {
		case IDOK:
			EndDialog(hDlg, TRUE);
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return (INT_PTR)TRUE;
			break;		
		}
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK Dialog_ViewStatitistic(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	GetClientRect(hDlg, &rcClient);
	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;
	HFONT hFont = CreateFont(17, 7, 0, 0, FW_EXTRALIGHT, TRUE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Times New Roman"));	GetClientRect(hDlg, &rcClient);
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:	
		static1 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height/10, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static1, WM_SETFONT, (WPARAM)hFont, TRUE);
		static2 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height/5, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static2, WM_SETFONT, (WPARAM)hFont, TRUE);
		static3 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height*3/10, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static3, WM_SETFONT, (WPARAM)hFont, TRUE);
		static4 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height*4/10, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static4, WM_SETFONT, (WPARAM)hFont, TRUE);
		static5 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height/2, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static5, WM_SETFONT, (WPARAM)hFont, TRUE);
		static6 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height*6/10, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static6, WM_SETFONT, (WPARAM)hFont, TRUE);
		static7 = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 580, height*7/10, 300, 20, hDlg, NULL, NULL, NULL);
		SendMessage(static7, WM_SETFONT, (WPARAM)hFont, TRUE);
		ShowWindow(static1, SW_HIDE);
		ShowWindow(static2, SW_HIDE);
		ShowWindow(static3, SW_HIDE);
		ShowWindow(static4, SW_HIDE);
		ShowWindow(static5, SW_HIDE);
		ShowWindow(static6, SW_HIDE);
		ShowWindow(static7, SW_HIDE);
		testBrush = CreateSolidBrush(RGB(255, 153, 153));

		return (INT_PTR)TRUE;
	
	case WM_CTLCOLORSTATIC:
	{
							  DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
							  HDC hdcStatic = (HDC)wParam;
							  SetTextColor(hdcStatic, RGB(0, 0, 0));
							  SetBkMode(hdcStatic, TRANSPARENT);
	}
	case WM_CTLCOLORDLG:
		return (INT_PTR)(testBrush);
	case WM_COMMAND:
	{
					   int id = LOWORD(wParam);
					   switch (id)
					   {
					   case IDOK:						  
						   EndDialog(hDlg, FALSE);
						   return (INT_PTR)TRUE;
					   case IDCANCEL:
						   EndDialog(hDlg, FALSE);
						   return (INT_PTR)TRUE;
					   default:
						   EndDialog(hDlg, FALSE);
						   return (INT_PTR)TRUE;
						   break;
					   }
	}break;
	case WM_PAINT:
	{
					 if (List.size() > 0){
						 hdc = BeginPaint(hDlg, &ps);
						 int nX = 250;
						 int nY = 200;
						 DWORD dwRadius = 140;
						 float xStartAngle = 90;
						 float xSweepAngle = 0;
						 int sum = 0;
						 hdc = GetDC(hDlg);
						 BeginPath(hdc);

						 //tinh tong so note co trong list
						 for (int i = 0; i < List.size(); i++){
							 sum += List[i].size() - 1;
						 }

						 //tong cua nhung tag khac khi so tag vuot qua 6
						 int sumOther = 0;

						 //dua danh sach cac index cua List vao
						 for (int i = 0; i < List.size(); i++){
							 sort.push_back(i);
						 }

						 //sap xep theo index
						 for (int i = 0; i < List.size() - 1; i++){
							 for (int j = i + 1; j < List.size(); j++){
								 if (List[sort[i]].size() < List[sort[j]].size()){
									 int temp = sort[i];
									 sort[i] = sort[j];
									 sort[j] = temp;
								 }
							 }
						 }


						 //bat dau ve					
						 SelectObject(hdc, GetStockObject(DC_BRUSH));
						 for (int i = 0; i < List.size(); i++){
							 if (i < 6){
								 //tinh goc quay
								 xSweepAngle = 360 * (List[sort[i]].size() - 1) / sum;
								 BeginPath(hdc);
								 if (i == 0)
								 {
									 if (i == List.size() - 1){
										 xSweepAngle = 270 + xStartAngle;
									 }
									 SetDCBrushColor(hdc, RGB(51, 51, 255));
									 setNote(static1, i, sort, sum);
								 }
								 if (i == 1)
								 {
									 if (i == List.size() - 1){
										 xSweepAngle = 270 + xStartAngle;
									 }
									 SetDCBrushColor(hdc, RGB(255, 0, 127));
									 setNote(static2, i, sort, sum);
								 }
								 if (i == 2)
								 {
									 if (i == List.size() - 1){
										 xSweepAngle = 270 + xStartAngle;
									 }
									 SetDCBrushColor(hdc, RGB(0, 255, 255));
									 setNote(static3, i, sort, sum);
								 }
								 if (i == 3)
								 {
									 if (i == List.size() - 1){
										 xSweepAngle = 270 + xStartAngle;
									 }
									 SetDCBrushColor(hdc, RGB(255, 255, 0));
									 setNote(static4, i, sort, sum);
								 }
								 if (i == 4)
								 {
									 if (i == List.size() - 1){
										 xSweepAngle = 270 + xStartAngle;
									 }
									 SetDCBrushColor(hdc, RGB(153, 0, 153));
									 setNote(static5, i, sort, sum);
								 }
								 if (i == 5)
								 {
									 if (i == List.size() - 1){
										 xSweepAngle = 270 + xStartAngle;
									 }
									 SetDCBrushColor(hdc, RGB(0, 153, 0));
									 setNote(static6, i, sort, sum);
								 }
							 }
							 else
							 {
								 //goc quay cuoi cung = 360 - tong so goc quay truoc do
								 xSweepAngle = 270 + xStartAngle;
								 BeginPath(hdc);
								 SetDCBrushColor(hdc, RGB(255, 128, 0));
								 draw(hDlg, hdc, nX, nY, dwRadius, xStartAngle, -xSweepAngle);

								 //hien chu tich
								 Rectangle(hdc, width * 2 / 3, height * (i + 1) / 10, width * 23 / 30, height * (i + 1) / 10 + height / 20);
								 ShowWindow(static7, SW_SHOWNOACTIVATE);
								 wstring temp = L"Khác: " + to_wstring((int)(100 * xSweepAngle / 360)) + L"\%";
								 SetWindowText(static7, temp.c_str());
								 break;
							 }

							 draw(hDlg, hdc, nX, nY, dwRadius, xStartAngle, -xSweepAngle);
							 Rectangle(hdc, width * 2 / 3, height * (i + 1) / 10, width * 23 / 30, height * (i + 1) / 10 + height / 20);

							 //dich chuyen duong noi tam voi bien theo chieu goc quay
							 xStartAngle -= xSweepAngle;
						 }

					 }
					
			}break;
	}
	return (INT_PTR)FALSE;
}

HWND btnDeleteTag , btnDeleteNote, edt1, edt2;
TVITEM item;


INT_PTR CALLBACK Dialog_ViewNote(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmEvent;
	UNREFERENCED_PARAMETER(lParam);
	HFONT hFont = CreateFont(17, 7, 0, 0, FW_EXTRALIGHT, TRUE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Times New Roman"));	GetClientRect(hDlg, &rcClient);
	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;
	switch (message)
	{
	case WM_INITDIALOG:
		edt1 = CreateWindowEx(NULL, L"EDIT", L"", WS_CHILD | WS_VISIBLE, width / 20, 10, width / 3, 20, hDlg, NULL, NULL, NULL);
		SendMessage(edt1, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(edt1, EM_SETCUEBANNER, TRUE, LPARAM(L"Search for Tag.."));

		edt2 = CreateWindowEx(NULL, L"EDIT", L"", WS_CHILD | WS_VISIBLE, width * 23 / 60, 10, width * 17 / 30, 20, hDlg, NULL, NULL, NULL);
		SendMessage(edt2, WM_SETFONT, WPARAM(hFont), TRUE);
		SendMessage(edt2, EM_SETCUEBANNER, TRUE, LPARAM(L"Search for note..."));

		btnDeleteTag = CreateWindowEx(NULL, L"BUTTON", L"deleteTag", WS_CHILD | WS_VISIBLE, 360, 385, 100, 35, hDlg, (HMENU)IDC_DELETE, NULL, NULL);
		SendMessage(btnDeleteTag, WM_SETFONT, WPARAM(hFont), TRUE);
		btnDeleteNote = CreateWindowEx(NULL, L"BUTTON", L"deleteNote", WS_CHILD | WS_VISIBLE, 220, 385, 100, 35, hDlg, (HMENU)IDC_DELETENOTE, NULL, NULL);
		SendMessage(btnDeleteNote, WM_SETFONT, WPARAM(hFont), TRUE);

		
		g_hTreeView = CreateWindowEx(0, WC_TREEVIEW, _T("Tree View"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | BS_BITMAP| WS_VSCROLL | WS_HSCROLL, width / 20, height / 10, width / 3, height * 3 / 4, hDlg, NULL, hInst, NULL);
		ListToShow = List;
		loadToTreeView(g_hTreeView);
		SendMessage(g_hTreeView, WM_SETFONT, WPARAM(hFont), TRUE);

		g_hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, _T("List View"),
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | WS_TABSTOP | lStyle, width * 23 / 60, height / 10, width * 17 / 30, height * 3 / 4, hDlg, NULL, hInst, NULL);
		initColumn(g_hListView, 500);
		SendMessage(g_hListView, WM_SETFONT, WPARAM(hFont), TRUE);
		testBrush = CreateSolidBrush(RGB(255, 153, 153));
		return (INT_PTR)TRUE;

	case WM_CTLCOLORSTATIC:
	{
							  DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
							  HDC hdcStatic = (HDC)wParam;
							  SetTextColor(hdcStatic, RGB(0, 0, 0));
							  SetBkMode(hdcStatic, TRANSPARENT);
	}break;

	case WM_CTLCOLORDLG:
	{return (INT_PTR)(testBrush);
	}break;
	
	case WM_NOTIFY:
	{

					  HWND m_hTreeView = g_hTreeView;
					  HWND m_hListView = g_hListView;
					  NMHDR* pnm = (NMHDR*)lParam;
					  LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;
					  switch (pnm->code)
					  {
					  case TVN_ITEMEXPANDING:

						  //To show child of item or folder of each Item
						  preloadExpanding(lpnmTree->itemOld.hItem, lpnmTree->itemNew.hItem, m_hTreeView);
						  break;
					  case TVN_SELCHANGED:
					  {

											 // Get a handle to the selected item
											 HTREEITEM hSelectedItem = TreeView_GetSelection(m_hTreeView);
											 // Now get the text of the selected item
											  TCHAR buffer[128];
											 ListView_DeleteAllItems(m_hListView); //Xóa sạch List View để nạp cái mới
											 item.hItem = hSelectedItem;
											 item.mask = TVIF_TEXT;
											 item.cchTextMax = 128;
											 item.pszText = buffer;
											 if (TreeView_GetItem(m_hTreeView, &item))
											 {
												 selectedItemIndex = loadToListView((LPWSTR)item.lParam, m_hListView);
											 }
											 TreeView_Expand(m_hTreeView, TreeView_GetNextItem(m_hTreeView, NULL, TVGN_CARET), TVE_EXPAND);

					  }
						  break;

					  case NM_CLICK:
					  {
									   int seletedIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
									   if (seletedIndex != -1)
									   {
										   int len = List[selectedItemIndex].size();
										   g_note = List[selectedItemIndex][len - seletedIndex - 1];
									   }
					  }break;
					  case NM_DBLCLK:
					  {

										int seletedIndex = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
										if (seletedIndex != -1)
										{
											int len = List[selectedItemIndex].size();
											g_note = List[selectedItemIndex][len - seletedIndex - 1];
											DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG4),
												hDlg, Dialog_ShowDetailNote);
											
										}


					  }
						  break;
					  }
	}break;
	case WM_COMMAND:
		wmEvent = HIWORD(wParam);

		if ((HWND)lParam == edt1) {
		if (wmEvent == EN_CHANGE) {
		//triggered on keypress
		WCHAR keyword[255];

		GetWindowText(edt1, keyword, 255);
		searchTag(keyword);
		
		}
		}

		if ((HWND)lParam == edt2) {
			if (wmEvent == EN_CHANGE) {
				//triggered on keypress
				WCHAR keyword2[255];		
				GetWindowText(edt2, keyword2, 255);
				searchNote((LPWSTR)item.lParam, keyword2);
			}
		}
		int id = LOWORD(wParam);
		switch (id) {
		case IDC_DELETE:
		{
						   
						   deleteTag((LPWSTR)item.lParam);
		}
			break;

		case IDC_DELETENOTE:
		{
							   deleteNote(g_note,(LPWSTR) item.lParam);
		}break;
		case IDOK:
			EndDialog(hDlg, TRUE);
			return (INT_PTR)TRUE;
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return (INT_PTR)TRUE;
		case ID_DELETE:
		{
						
		}
			break;
		}
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DialogCreateNew(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);

	hFont1 = CreateFont(19, 8, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"italic");
	hFont2 = CreateFont(17, 7, 0, 0, FW_EXTRALIGHT, TRUE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Times New Roman"));	GetClientRect(hDlg, &rcClient);
	switch (message)
	{
	case WM_INITDIALOG:
	{
						  HWND stat = CreateWindowEx(0, L"STATIC", L"Create a new note", WS_CHILD | WS_VISIBLE, 180, 20, 200, 20, hDlg, NULL, NULL, NULL);
						  SendMessage(stat, WM_SETFONT, (WPARAM)hFont1, TRUE);
						  HWND static1 = GetDlgItem(hDlg, IDC_STATIC2);
						  SendMessage(static1, WM_SETFONT, (WPARAM)hFont2, TRUE);
						  HWND edt1 = GetDlgItem(hDlg, IDC_EDIT1);
						  SendMessage(edt1, WM_SETFONT, (WPARAM)hFont2, TRUE);
						  HWND combo = GetDlgItem(hDlg, IDC_COMBO2);
						  SendMessage(combo, WM_SETFONT, (WPARAM)hFont2, TRUE);
						  HWND static2 = GetDlgItem(hDlg, IDC_STATIC3);
						  SendMessage(static2, WM_SETFONT, (WPARAM)hFont2, TRUE);
						  setValueForCombobox(combo);
						  testBrush = CreateSolidBrush(RGB(255, 153, 153));

	return (INT_PTR)TRUE; }

	case WM_CTLCOLORSTATIC:
	{
							  DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
							  HDC hdcStatic = (HDC)wParam;
							  SetTextColor(hdcStatic, RGB(0, 0, 0));
							  SetBkMode(hdcStatic, TRANSPARENT);
	}
	case WM_CTLCOLORDLG:
		return (INT_PTR)(testBrush);

	case WM_COMMAND:
		int id = LOWORD(wParam);
		switch (id) {
		case IDOK:
		{
					 //tao note moi
					 
					 WCHAR temp1[255];
					 GetDlgItemText(hDlg, IDC_EDIT1,temp1, MAX_CONTENT_SIZE);
					 wstring temp2 = temp1;
					 if (temp2.size())
					 {

						 Note note;
						 note.content = temp1;

						 note.time = getCurrentTime();
						 WCHAR temp[MAX_TAGS_SIZE];
						 GetDlgItemText(hDlg, IDC_COMBO2, temp, MAX_TAGS_SIZE);

						 //cat thanh nhieu tag rieng biet
						 WCHAR *pwc = wcstok(temp, L",");
						 while (pwc != NULL)
						 {
							 wprintf(L"%ls\n", pwc);
							 note.tags.push_back(pwc);
							 pwc = wcstok(NULL, L",");
						 }

						 ListNote.push_back(note);
						 addToList(note);
						 TreeView_DeleteAllItems(g_hTreeView);
						 ListToShow = List;
						 loadToTreeView(g_hTreeView);
					 }
					 
					 EndDialog(hDlg, TRUE);
					return (INT_PTR)TRUE; 

		}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return (INT_PTR)TRUE;
			break;
		}

	
	}
	return (INT_PTR)FALSE;
}

void writeListItemToFile(wstring path)
{
	const locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());
	wofstream f(path);
	f.imbue(utf8_locale);
	//ghi số Note
	f << ListNote.size() << endl;

	for (int i = 0; i < ListNote.size(); i++)
	{
		f << wstring(ListNote[i].content) << endl;
		f << wstring(ListNote[i].time) << endl;
		f << ListNote[i].tags.size() << endl;

		for (int j = 0; j < ListNote[i].tags.size(); j++){
			f << wstring(ListNote[i].tags[j]) << endl;
		}		
	}

	f.close();
}

void readListItemFromFile(wstring path)
{
	const locale utf8_locale = locale(locale(), new codecvt_utf8<wchar_t>());
	std::wfstream f;
	f.imbue(utf8_locale);
	f.open(path, ios::in);
	wstring buffer, numbertags;
	if (f.is_open())
	{
		while (getline(f, buffer))
		{

			int sum = _wtoi(buffer.c_str());
			for (int i = 0; i < sum; i++){
				Note note;
				getline(f, note.content);
				getline(f, note.time);
				getline(f, numbertags);
				int number = _wtoi(numbertags.c_str());
				for (int j = 0; j < number; j++){
					wstring tag;
					getline(f, tag);
					note.tags.push_back(tag);
				}
				ListNote.push_back(note);
				addToList(note);
			}			
		}
	}

	
	f.close();
}

void addToList(Note note){
	//them note vao list
	for (int j = 0; j < note.tags.size(); j++){
		int check = 0; // chua ton tai tag nay
		wstring temp = note.tags[j];
		for (int i = 0; i < List.size(); i++){
			wstring temp2 = List[i][0].tags[0];
			if (CompareTag(List[i][0].tags[0],note.tags[j])){
				check = 1; //co ton tai
				List[i].push_back(note);
			}
		}
		if (check == 0){
			Note tempNote;
			tempNote.content = L"";
			tempNote.tags.push_back(note.tags[j]);
			Tag.push_back(note.tags[j]);
			vector<Note> tempList;
			tempList.push_back(tempNote);
			tempList.push_back(note);
			List.push_back(tempList);

		
		}
	}
}

void loadToTreeView(HWND m_hTreeView)
{

	TV_INSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	for (int i = 0; i < ListToShow.size(); i++){
		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = TVI_LAST;
		wstring name = ListToShow[i][0].tags[0] + L" (" + to_wstring(ListToShow[i].size() - 1) + L")";
		tvInsert.item.pszText = (LPWSTR)name.c_str();
		tvInsert.item.lParam = (LPARAM)ListToShow[i][0].tags[0].c_str();
		HTREEITEM hDesktop = TreeView_InsertItem(m_hTreeView, &tvInsert);
	}
}

void initColumn(HWND m_hListView, int width)
{
	InitCommonControls();

	LVCOLUMN lvCol;

	lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = width/2;
	lvCol.pszText = _T("Content");
	ListView_InsertColumn(m_hListView, 1, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = width/2;
	lvCol.pszText = _T("Time");
	ListView_InsertColumn(m_hListView, 2, &lvCol);
}

int loadToListView(wstring name, HWND listview){

	int index = 0;
	for (int i = 0; i < List.size(); i++){
		if (List[i][0].tags[0] == name){
			index = i;
			for (int j = 0; j < List[i].size(); j++){
				LV_ITEM lv;
				lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
				lv.iItem = 0;
				lv.iSubItem = 0;
				wstring temp;
				if (List[i][j].content.length() <= 50)
				{
					temp = List[i][j].content;
				}
				else
				{
					temp = List[i][j].content.substr(0,49);
					temp += L"...";

				}
				lv.pszText = (LPWSTR)temp.c_str();
				ListView_InsertItem(listview, &lv);

				lv.mask = LVIF_TEXT;
				lv.iSubItem = 1;
				lv.pszText = (LPWSTR)List[i][j].time.c_str();
				ListView_SetItem(listview, &lv);

				
			}		
		}		
	}
	return index;
}

void loadChildOfTreeItem(HTREEITEM &hParent, wstring tag, BOOL bShowHiddenSystem, HWND m_hTreeView)
{
	TV_INSERTSTRUCT tvInsert;
	for (int i = 0; i < List.size();i++){
		if (tag == List[i][0].tags[0])
		{
			for (int j = 0; j < List[i].size(); j++){
				tvInsert.hParent = hParent;
				tvInsert.hInsertAfter = TVI_LAST;
				tvInsert.item.pszText = (LPWSTR)List[i][j].content.c_str();
				tvInsert.item.lParam = (LPARAM)List[i][j].content.c_str();
				HTREEITEM hItem = TreeView_InsertItem(m_hTreeView, &tvInsert);
			}
			
		}
		
	}
}

void preloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel, HWND m_hTreeView)
{
	if (hCurSel == TreeView_GetRoot(m_hTreeView))
	{
		return;
	}
	else
	{
		
		// Now get the text of the selected item
		TCHAR buffer[128];

		
		HTREEITEM currentTreeChild = TreeView_GetChild(m_hTreeView, hCurSel);	
		TVITEM item;
		item.hItem = currentTreeChild;
		item.mask = TVIF_TEXT;
		item.cchTextMax = 128;
		item.pszText = buffer;
		
		do{
			if (TreeView_GetChild(m_hTreeView, currentTreeChild) == NULL)
			{				

				loadChildOfTreeItem(currentTreeChild, item.pszText, FALSE, m_hTreeView);
			}
			currentTreeChild = TreeView_GetNextSibling(m_hTreeView, currentTreeChild);
		} while (currentTreeChild);
	}
}

void setValueForCombobox(HWND cb)
{
	sort.clear();
	//dua danh sach cac index cua List vao
	for (int i = 0; i < List.size(); i++){
		sort.push_back(i);
	}

	//sap xep theo index

	if (List.size()>0)
	{
		for (int i = 0; i < List.size() - 1; i++){
			for (int j = i + 1; j < List.size(); j++){
				if (List[sort[i]].size() < List[sort[j]].size()){
					int temp = sort[i];
					sort[i] = sort[j];
					sort[j] = temp;
				}
			}
		}
	}
	

	if (sort.size()>5)
	{
		for (int i = 0; i < 5; i += 1)
		{
			// Add string to combobox.
			SendMessage(cb, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)Tag[i].c_str());
		}
	}
	else
	{
		for (int i = 0; i < sort.size(); i += 1)
		{
			// Add string to combobox.
			SendMessage(cb, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)Tag[i].c_str());
		}
	}

	//  in the selection field  
	SendMessage(cb, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

}

void draw(HWND hDlg,HDC hdc, int nX, int nY, DWORD dwRadius, float xStartAngle, float xSweepAngle){
	MoveToEx(hdc, nX, nY, (LPPOINT)NULL);
	AngleArc(hdc, nX, nY, dwRadius, xStartAngle, xSweepAngle);
	LineTo(hdc, nX, nY);
	EndPath(hdc);
	FillPath(hdc);
	//ReleaseDC(hDlg, hdc);
}

void setNote(HWND staticX, int i, vector<int>sort,int sum){
	ShowWindow(staticX, SW_SHOWNOACTIVATE);
	wstring temp = List[sort[i]][0].tags[0] + L": " + to_wstring(100 * (List[sort[i]].size() - 1) / sum) + L"\%";
	SetWindowText(staticX, temp.c_str());
}

void CALLBACK timeToshowStatitistic(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime)
{
	remain_Time--;
	if (remain_Time == -1)
	{		
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3),
			hwnd, Dialog_ViewStatitistic);
		remain_Time = TIMETOSHOW;
	}

	return;
}

bool CompareTag(wstring tag1, wstring tag2){
	
	if (_wcsnicmp(tag1.c_str(), tag2.c_str(), 20) == 0)
		return true;
	return false;
	
}
 

wstring getCurrentTime(){
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	string temp = asctime(timeinfo);
	string temp2 = "";
	for (int i = 11; i < temp.size() - 5; i++){
		temp2 += temp[i];
	}

	string year = "";
	for (int i = 20; i < temp.size()-1; i++){
		year += temp[i];
	}

	string day = "";
	for (int i = 0; i < 3; i++){
		day += temp[i];
	}

	string month = "";
	for (int i = 4; i < 7; i++){
		month += temp[i];
	}

	string date = "";
	for (int i = 8; i < 10; i++){
		date += temp[i];
	}

	if (day.compare("Mon") == 0){
		temp2 += " Thu 2";
	}
	else if (day.compare("Tue") == 0){
		temp2 += " Thu 3";
	}
	else if (day.compare("Wed") == 0){
		temp2 += " Thu 4";
	}
	else if (day.compare("Thu") == 0){
		temp2 += " Thu 5";
	}
	else if (day.compare("Fri") == 0){
		temp2 += " Thu 6";
	}
	else if (day.compare("sat") == 0){
		temp2 += " Thu 7";
	}
	else if (day.compare("Sun") == 0){
		temp2 += " Chu nhat";
	}

	temp2 += " Ngay "+ date;

	if (month.compare("Jan") == 0){
		temp2 += "/01";
	}
	else if (month.compare("Feb") == 0){
		temp2 += "/02";
	}
	else if (month.compare("Mar") == 0){
		temp2 += "/03";
	}
	else if (month.compare("Apr") == 0){
		temp2 += "/04";
	}
	else if (month.compare("May") == 0){
		temp2 += "/05";
	}
	else if (month.compare("Jun") == 0){
		temp2 += "/06";
	}
	else if (month.compare("Jul") == 0){
		temp2 += "/07";
	}
	else if (month.compare("Aug") == 0){
		temp2 += "/08";
	}
	//else if (month.compare("Sep") == 0){
		//temp2 += "/09";
	//}
	else if (month.compare("Oct") == 0){
		temp2 += "/10";
	}
	else if (month.compare("Nov") == 0){
		temp2 += "/11";
	}
	else if (month.compare("Dec") == 0){
		temp2 += "/12";
	}

	temp2 += "/" + year;

	wstring wsTmp(temp2.begin(), temp2.end());

	return  wsTmp;
}

LRESULT CALLBACK MyHookProc(int nCode, WPARAM wParam, LPARAM lParam)    
{                 

	if (nCode < 0) 
	{
		return CallNextHookEx(hkCreateNew, nCode, wParam, lParam);
	}

	if ((GetAsyncKeyState(VK_LWIN)<0) && (GetAsyncKeyState('P') <0))
	{
		if (!isClicked)
		{

			isClicked = true;
		}	

		if (isClicked){
			int result = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2),
				NULL, DialogCreateNew);
		}
		
	}

	return CallNextHookEx(hkCreateNew, nCode, wParam, lParam);

}

void _doInstallHook(HWND hWnd)
{
	if (hkCreateNew != NULL) return;
	hkCreateNew = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)MyHookProc, hinstLib, 0);
	int k = 10;
	k = 8;
}

void _doRemoveHook(HWND hWnd)
{
	if (hkCreateNew == NULL) return;
	UnhookWindowsHookEx(hkCreateNew);
	hkCreateNew = NULL;
	MessageBox(hWnd, L"Remove hook successfully", L"Result", MB_OK);
}

void deleteNote(Note note, wstring tag){
	for (int i = 0; i < ListNote.size(); i++){
		if (ListNote[i].content.compare(note.content) == 0){
			for (int k = 0; k < ListNote[i].tags.size(); k++){
				if (ListNote[i].tags[k].compare(tag) == 0)
				{
					ListNote[i].tags.erase(ListNote[i].tags.begin() + k);
				}
			}
		}
	}
	ListView_DeleteAllItems(g_hListView);
	TreeView_DeleteAllItems(g_hTreeView);
	List.clear();
	for (int i = 0; i < ListNote.size(); i++){
		addToList(ListNote[i]);
	}

	ListToShow.clear();
	ListToShow = List;
	loadToTreeView(g_hTreeView);
	loadToListView((LPWSTR)item.lParam, g_hListView);
}

void deleteTag(wstring tag){
	for (int i = 0; i < ListNote.size(); i++){
		for (int j = 0; j < ListNote[i].tags.size(); j++){
			if (ListNote[i].tags[j].compare(tag) == 0){
				ListNote[i].tags.erase(ListNote[i].tags.begin()+j);
			}

		}
	}
	ListView_DeleteAllItems(g_hListView);

	TreeView_DeleteAllItems(g_hTreeView);
	List.clear();
	for (int i = 0; i < ListNote.size(); i++){
		addToList(ListNote[i]);
	}

	ListToShow.clear();
	ListToShow = List;

	loadToTreeView(g_hTreeView);
	loadToListView((LPWSTR)item.lParam, g_hListView);

}
 
void searchNote(wstring tag, wstring keyword){

	ListView_DeleteAllItems(g_hListView);

	int index = 0;
	for (int i = 0; i < List.size(); i++){
		if (List[i][0].tags[0] == tag){
			index = i;
			for (int j = 0; j < List[i].size(); j++){

				if (List[i][j].content.find(keyword) != -1){
					LV_ITEM lv;
					lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
					lv.iItem = 0;
					lv.iSubItem = 0;
					wstring temp;
					if (List[i][j].content.length() <= 50)
					{
						temp = List[i][j].content;
					}
					else
					{
						temp = List[i][j].content.substr(0, 49);
						temp += L"...";

					}
					lv.pszText = (LPWSTR)temp.c_str();
					ListView_InsertItem(g_hListView, &lv);

					lv.mask = LVIF_TEXT;
					lv.iSubItem = 1;
					lv.pszText = (LPWSTR)List[i][j].time.c_str();
					ListView_SetItem(g_hListView, &lv);

				}

			}
		}
	}
}

void searchTag(wstring keyword)
{
	ListToShow.clear();
	for (int i = 0; i < List.size(); i++)
	{
		if (List[i][0].tags[0].find(keyword) != -1)
		{
			ListToShow.push_back(List.at(i));
		}
	}

	TreeView_DeleteAllItems(g_hTreeView);
	loadToTreeView(g_hTreeView);
	
}

   