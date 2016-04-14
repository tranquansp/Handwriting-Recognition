// DocAnh.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DocAnh.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>
#define MAX_LOADSTRING 100

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// MyGlobal Variables
typedef struct tagMYCOLOR {
	BYTE R;
	BYTE G;
	BYTE B;
} MYCOLOR;

MYCOLOR **data;

FILE *stream;
int  n, m,  i, j, k, k2;
//float **data, **symmat,
float **symmat2, *evals, *interm;
float *dis;
float in_value;
char option;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				ReadPPM(HWND hWnd);
MYCOLOR				**Alloc2d(int m, int n);
void				Free2d(MYCOLOR **aa);
void				ReadImage(HWND hWnd);
int KhongLaMat(float **dataSrc,float **eigenface,int n, int m,HDC hdc);

void xuly( float **dataN, HDC hdc,HWND hWnd, int n ,int m);
void xuat(float so,int x,int y,HDC hdc);
float **matrix(int n, int m);
void corcol(float **data, int n, int m, float **symmat);
void covcol(float **data, int n, int m, float **symmat,HDC hdc,HWND hWnd);
void scpcol(float **data, int n, int m, float **symmat);
//float *vector(int n);
void tred2(float **a, int n, float *d, float *e);
void tqli(float *d, float *e, int n, float **z,HWND hWnd);
void free_vector(float *v,int n);
void free_matrix(float **mat,int n,int m);
void erhand(char *err_msg,HWND hWnd);

void tinh_eigenface(float **dataSrc, float **symmat, int n,int m,float **eigenface);
void chieu(float **eigenface, float **dataSrc,float **proimage,int n,int m);
void chieu_anhtest(float **eigenface,float *ve_im_test,float **proimage,int n,int m);
float NguongMax(float  **proimage,int m);
int sosanh(float **proimage,int n,int m);
void sapxep_vector(float **symmat,int n,int m);
int AnhKhongLaMat(float **dataSrc,float **symmat,int n,int m,HDC hdc);




void ghifile2(FILE *fp,int m,float *dulieu);
void ghifile(FILE *fp,int n,int m,float **dulieu);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE,szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DOCANH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DOCANH);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DOCANH);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_DOCANH;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
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

   hWnd = CreateWindow(szWindowClass,TEXT ("Nhan Dang")/* szTitle*/, WS_OVERLAPPEDWINDOW|WS_MAXIMIZE,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_READPPM:
			ReadPPM(hWnd);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
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
		//if (*data != NULL)
		//	Free2d(data);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}



MYCOLOR **Alloc2d(int m, int n)
{
	MYCOLOR **aa;
	MYCOLOR *a;
	int i;

	aa = (MYCOLOR **)calloc(m,sizeof(MYCOLOR *));
	a  = (MYCOLOR *)calloc(m*n,sizeof(MYCOLOR));

	for (i=0; i<m; i++)
		aa[i] = a + i*n;
	return aa;
}

void Free2d(MYCOLOR **aa)
{ 
	free(*aa);
	free(aa);
	return;
}


BYTE **Alloc2dByte(int m, int n)
{
	BYTE **aa;
	BYTE *a;
	int i;

	aa = (BYTE **)calloc(m,sizeof(BYTE *));
	a  = (BYTE *)calloc(m*n,sizeof(BYTE));

	for (i=0; i<m; i++)
		aa[i] = a + i*n;
	return aa;
}

void Free2dByte(BYTE **aa)
{ 
	free(*aa);
	free(aa);
	return;
}


float **Alloc2dFloat(int m, int n)
{
	float **aa;
	float *a;
	int i;

	aa = (float **)calloc(m,sizeof(float *));
	a  = (float *)calloc(m*n,sizeof(float));

	for (i=0; i<m; i++)
		aa[i] = a + i*n;
	return aa;
}

void Free2dFloat(float **aa)
{ 
	free(*aa);
	free(aa);
	return;
}

float *vector(int n)
/* Allocates a float vector with range [1..n]. */
{

    float *v;

    /*v = (float *) malloc ((unsigned) n*sizeof(float));
    if (!v) erhand("Allocation failure in vector().");
    return v-1; */
	v  = (float *)calloc(n,sizeof(float));
	return v;
}

typedef struct tagMYHEADER {
	short int id;
	short int nx;
	short int ny;
	short int notused1;
	short int notused2;
} MYHEADER;



float **dataSrc,**luu;
float *mean;
float *ve_im_test;
int daianh;
void ReadPPM(HWND hWnd)
{
	HDC hdc;
	FILE *fp;
	char Id[4], cWidth[10], cHeight[10], cGray[10];
	int Width, Height, i, j;
	BYTE I;
	BYTE **data1;
	float chuyen;
	FILE *fpText;
	float *vectorimage;
	hdc = GetDC(hWnd);

	fp = fopen("C:\\ANH\\CSDL\\1.ppm","rb");//  "E:\\MINH\\TTD2.ppm","rb");//anh so 1

	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	data = Alloc2d(Height,Width);
	dataSrc=Alloc2dFloat(Height*Width,26);          //10 anh cho cac anh ban dau
	daianh=Width;
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);

	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			//SetPixel(hdc,j,i,RGB(data[i][j].R,data[i][j].G,data[i][j].B));
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][0]=(float)I;
			//SetPixel(hdc,j,i,RGB(I,I,I));
		}
	fclose(fp);

	fp = fopen("C:\\ANH\\CSDL\\2.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][1]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\3.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][2]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\4.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][3]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\5.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][4]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\6.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][5]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\7.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][6]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\8.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][7]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\9.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][8]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\10.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][9]=(float)I;
			}
	fclose(fp);

	fp = fopen("C:\\ANH\\CSDL\\11.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][10]=(float)I;
			}
	fclose(fp);

	fp = fopen("C:\\ANH\\CSDL\\12.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][11]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\13.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][12]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\14.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][13]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\15.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][14]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\16.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][16]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\17.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][16]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\18.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][17]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\19.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][18]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\20.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][19]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\21.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][20]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\22.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][21]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\23.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][22]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\24.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][23]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\25.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][24]=(float)I;
			}
	fclose(fp);
	fp = fopen("C:\\ANH\\CSDL\\26.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][25]=(float)I;
			}
	fclose(fp);


	//////////////////csdl cua Son/////////////
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\1.ppm","rb");//  "E:\\MINH\\TTD2.ppm","rb");//anh so 1

	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//data = Alloc2d(Height,Width);
	//dataSrc=Alloc2dFloat(Height*Width,10);          //10 anh cho cac anh ban dau
	//daianh=Width;
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);

	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		//SetPixel(hdc,j,i,RGB(data[i][j].R,data[i][j].G,data[i][j].B));
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][0]=(float)I;
	//		//SetPixel(hdc,j,i,RGB(I,I,I));
	//	}
	//fclose(fp);

	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\2.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][1]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\3.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][2]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\4.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][3]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\5.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][4]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\6.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][5]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\7.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][6]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\8.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][7]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\9.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][8]=(float)I;
	//		}
	//fclose(fp);
	//fp = fopen("C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\PhotoPPM\\10.ppm","rb");
	//fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	//Width = atoi(cWidth); Height = atoi(cHeight);
	//fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	//for (i=0; i<Height; i++)
	//	for (j=0; j<Width; j++) {
	//		I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
	//		dataSrc[j+i*Width][9]=(float)I;
	//		}
	//fclose(fp);


	///////////////////////////////////////////


/////////////////////////CSDL////////////////////////////////////////////////////////////////////////////////	

/*	fp = fopen("G:\\Database\\TrainSetDT\\1.ppm","rb");//  "E:\\MINH\\TTD2.ppm","rb");//anh so 1

	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	data = Alloc2d(Height,Width);
	dataSrc=Alloc2dFloat(Height*Width,20);          //20 anh cho cac anh ban dau
	daianh=Width;
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);

	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			//SetPixel(hdc,j,i,RGB(data[i][j].R,data[i][j].G,data[i][j].B));
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][0]=(float)I;
			//SetPixel(hdc,j,i,RGB(I,I,I));
		}
	fclose(fp);
	

	fp = fopen("G:\\Database\\TrainSetDT\\2.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][1]=(float)I;
			}
	fclose(fp);

	fp = fopen("G:\\Database\\TrainSetDT\\3.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][2]=(float)I;
			}
	fclose(fp);

	fp = fopen("G:\\Database\\TrainSetDT\\4.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][3]=(float)I;
			}
	fclose(fp);

	fp = fopen("G:\\Database\\TrainSetDT\\5.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][4]=(float)I;
			}
	fclose(fp);

	fp = fopen("G:\\Database\\TrainSetDT\\6.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][5]=(float)I;
			}
	fclose(fp);

	//---------------
	fp = fopen("G:\\Database\\TrainSetDT\\7.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][6]=(float)I;
			}
	fclose(fp);
	fp = fopen("G:\\Database\\TrainSetDT\\8.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][7]=(float)I;
			}
	fclose(fp);
	//////////////////////////////////////////////////-------------------
	fp = fopen("G:\\Database\\TrainSetDT\\9.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][8]=(float)I;
			}
	fclose(fp);
	fp = fopen("G:\\Database\\TrainSetDT\\10.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][9]=(float)I;
			}
	fclose(fp);
	fp = fopen("G:\\Database\\TrainSetDT\\11.ppm","rb");         //////////////////////////////
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][10]=(float)I;
			}
	fclose(fp);
	fp = fopen("G:\\Database\\TrainSetDT\\12.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][11]=(float)I;
			}
	fclose(fp);
	fp = fopen("G:\\Database\\TrainSetDT\\13.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][12]=(float)I;
			}
	fclose(fp);
	fp = fopen("G:\\Database\\TrainSetDT\\14.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][13]=(float)I;
			}
	fclose(fp);fp = fopen("G:\\Database\\TrainSetDT\\15.ppm","rb"); //////////////////////////
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][14]=(float)I;
			}
	fclose(fp);fp = fopen("G:\\Database\\TrainSetDT\\16.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][15]=(float)I;
			}
	fclose(fp);fp = fopen("G:\\Database\\TrainSetDT\\17.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][16]=(float)I;
			}
	fclose(fp);fp = fopen("G:\\Database\\TrainSetDT\\18.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][17]=(float)I;
			}
	fclose(fp);fp = fopen("G:\\Database\\TrainSetDT\\19.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][18]=(float)I;
			}
	fclose(fp);fp = fopen("G:\\Database\\TrainSetDT\\20.ppm","rb");
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			dataSrc[j+i*Width][19]=(float)I;
			}
	fclose(fp);
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////anh test do tim
	ve_im_test=vector(Height*Width);
	fp = fopen("C:\\ANH\\CSDL_test\\XaBong.ppm","rb");//C:\\Users\\Mr.Minh\\Desktop\\New Folder\\Application\\PCA\\PCA\\QueryPPM\\XaBong2.ppm","rb");  //G:\\Database\\TestSetDT\\XaBong.ppm","rb");   //
	fscanf(fp,"%s %s %s %s ",Id,cWidth,cHeight,cGray);
	Width = atoi(cWidth); Height = atoi(cHeight);
	fread(*data,sizeof(MYCOLOR),Height*Width,fp);
	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = (data[i][j].R+data[i][j].G+data[i][j].B)/3;
			ve_im_test[j+i*Width]=(float)I;
			SetPixel(hdc,j,i,RGB(I,I,I));
			}
	fclose(fp);
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////			
	/////set anh test
	//j=0;              //test
	//for(i=0;i<Height*Width;i++)
	//{
	//	if(i%Width==0)
	//		j++;
	//	SetPixel(hdc,i-(j-1)*Width,j+300,RGB(dataSrc[i][2],dataSrc[i][2],dataSrc[i][2]));
	//}
	//j=0;              //test
	//for(i=0;i<Height*Width;i++)
	//{
	//	if(i%Width==0)
	//		j++;
	//	SetPixel(hdc,i-(j-1)*Width+300,j+300,RGB(dataSrc[i][4],dataSrc[i][4],dataSrc[i][4]));
	//}
	xuly(dataSrc,hdc,hWnd,Height*Width,26);//10);
	ReleaseDC(hWnd, hdc);
	return;
}

void xuat(float so,int x,int y,HDC hdc)
{
	int     decimal,   sign,n;
    char    *buff;
	if(so>100)
		n=3;
	else 
		if(so>=10)
			n=2;
		else 
			if(so<10)
				n=1;

	buff = _ecvt( (double)so, 3, &decimal, &sign );
	//_itoa((double)so,buff,3);
	TextOut (hdc, x,y, buff, n);
}

void ReadImage(HWND hWnd)
{
	HDC hdc;
	FILE *fp;
	char Id[4], cWidth[10], cHeight[10], cGray[10];
	int Width, Height, i, j;
	BYTE I;

	MYHEADER header;
	BYTE **data1;

	hdc = GetDC(hWnd);
	
	fp = fopen("d:\\HatGao\\hi_map","rb");

	fread(&header,sizeof(MYHEADER),1,fp);
	Width = header.nx; Height = header.ny;
	data1 = Alloc2dByte(Height,Width);
	fread(*data1,sizeof(BYTE),Height*Width,fp);

	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = data1[i][j];
			SetPixel(hdc,j,i,RGB(I,I,I));
		}
	
	fclose(fp);

	fp = fopen("d:\\HatGao\\hi_map.pgm","wb");

	fprintf(fp,"P5\n%d %d\n255\n",Width,Height);

	fwrite(*data1,sizeof(BYTE),Height*Width,fp);

	for (i=0; i<Height; i++)
		for (j=0; j<Width; j++) {
			I = data1[i][j];
			SetPixel(hdc,j,i,RGB(I,I,I));
		}
	
	fclose(fp);

	ReleaseDC(hWnd, hdc);

	Free2dByte(data1);
	return;
}



float temp1=0;
void xuly( float **dataN, HDC hdc,HWND hWnd, int n ,int m)
{
	
	FILE * fpText;
	float **symmat;//, **symmat2, *evals, *inter;
	
	HDC hdc1;
	hdc1=GetDC(hWnd);
	symmat=Alloc2dFloat(m,m);
	
	covcol(dataN,n,m,symmat,hdc,hWnd);

	evals = vector(m+1);     /* Storage alloc. for vector of eigenvalues */
    interm = vector(m+1);    /* Storage alloc. for 'intermediate' vector */
    //symmat2 = Alloc2dFloat(m+1, m+1);  /* Duplicate of correlation (etc.) matrix */
    //for (i = 1; i <= m; i++) {
    // for (j = 1; j <= m; j++) {
    //  symmat2[i][j] = symmat[i][j]; /* Needed below for col. projections */
    //                          }
    //     

	float ** a1;
	a1=Alloc2dFloat(m+1,m+1);
	for (i=1;i<=m;i++)
	for(j=1;j<=m;j++)
		a1[i][j]=symmat[i-1][j-1]; // a1 chinh la covarian symmat
	tred2(a1, m, evals, interm);  /* Triangular decomposition */
    tqli(evals, interm, m, a1,hWnd);   /* Reduction of sym. trid. matrix */

	
	for (i=0;i<m;i++)
	for(j=0;j<m;j++)
		symmat[i][j]=a1[i+1][j+1]; 
	for( i=0;i<m;i++)
		evals[i]=evals[i+1];
	///////sap xep vector rieng giam dan theo tri rieng
	//fpText = fopen("E:\\1.txt","wt");
	//ghifile(fpText,m,m,symmat);
	sapxep_vector(symmat,n,m);
	//fpText = fopen("E:\\2.txt","wt");
	//ghifile(fpText,m,m,symmat);
		
	//fpText = fopen("E:\\mean.txt","wt");
	//ghifile2(fpText,n,mean);

	////////data cac mat tru mat trung binh
	for(i=0;i<m;i++)
		for(j=0;j<n;j++)
			dataSrc[j][i]-=mean[j];
	
	//fpText = fopen("E:\\evals.txt","wt");
	//ghifile2(fpText,m,evals);
	
	int j1,j2;
	float **eigenface;
	eigenface=Alloc2dFloat(n,m);
	tinh_eigenface(dataSrc,symmat,n,m,eigenface);
	

	float **proimage;
	proimage=Alloc2dFloat(m,m+1);
	chieu(eigenface,dataSrc,proimage,n,m);
	//----------------------------------------test
	//float ** test;
	//test=Alloc2dFloat(n,m);
	//double min,max;
	//float *tem;
	//tem = vector(n+1); 
	//for(k=0;k<6;k++){
	//j=0;
	////====tim min
	//	min=eigenface[0][k];
	//	for(i=0;i<n;i++)
	//		if( eigenface[i][k]<min)
	//			min=eigenface[i][k];

	////----min
	////=====tru min
	//	for(i=0;i<n;i++)
	//		tem[i]=eigenface[i][k]-min;
	////max
	//	max=eigenface[0][k];
	//	for(i=0;i<n;i++)
	//		if( eigenface[i][k]>max)
	//			max=eigenface[i][k];
	////---max
	////chuan hoa
	//	for(i=0;i<n;i++)
	//		tem[i]=tem[i]*255/max;
	//for(i=0;i<n;i++)
	//{
	//	if(i%daianh==0)
	//		j++;
	//	SetPixel(hdc,i-(j-1)*daianh+k*200,j+450,RGB((BYTE)tem[i],(BYTE)tem[i],(BYTE)tem[i]));//eigenface[i][k],eigenface[i][k],eigenface[i][k]));
	//}
	//}
	

j=0;
////////		mat trung bnh
	/*for(i=0;i<n;i++)
	{
		if(i%daianh==0)
			j++;
		SetPixel(hdc,i-(j-1)*daianh,j+240,RGB((int)(mean[i]+0.5),(int)(mean[i]+0.5),(int)(mean[i])+0.5));
	}*/


	//------------------------------------------
	//khac biet cua anh test voi trung binh cac khuon mat nguon
	for(i=0;i<n;i++)
		ve_im_test[i]-=mean[i];
	//FILE *fpText2;
	//fpText2 = fopen("E:\\pro3.txt","wt");
	//ghifile(fpText2,m,m,proimage);
	
	//tinh do tuong dong 
	//int tuongdong=AnhKhongLaMat(dataSrc,symmat,n,m,hdc);

	int test= KhongLaMat(dataSrc,eigenface, n, m,hdc);

	//khac biet cua anh test voi trung binh cac khuon mat nguon
	/*for(i=0;i<n;i++)
		ve_im_test[i]-=mean[i];*/
	//if(tuongdong==0)
	/*if(test==0)
	{
		TextOut(hdc,400,100,"Anh Khong La Mat", ARRAYSIZE("Anh Khong La Mat"));
		return;
	}*/
   
	

	////chieu vector anh test sai biet voi trung binh cac khuon mat nguon len ko gian vetor khuon mat nguon
	chieu_anhtest(eigenface,ve_im_test,proimage,n,m);
	//fpText = fopen("E:\\prom.txt","wt");
	//ghifile(fpText,m,m,proimage);
	
	///////////////////////khoi phuc anh test ,xem co fai la anh test nay la khuon mat?
	float *re_test_image;
	re_test_image=vector(n);
	
	for(i=0;i<n;i++)
	{
		re_test_image[i]=0.0;
		for(j=0;j<m;j++)
			re_test_image[i]+=eigenface[i][j]*proimage[j][m];
	}
	j=0; 
	for(i=0;i<n;i++)
		re_test_image[i]=re_test_image[i]+mean[i];
	
	
	////tim anh thu may trong csdl giong voi anh test , bang cach tinh khoang cach Eulicde
	int similar_im= sosanh(proimage, n, m);

	////tinh sai so  theta  -->xd anh test co hay ko trong csdl
	//fpText2 = fopen("E:\\pro66.txt","wt");
	//ghifile(fpText2,m,m,proimage);
	float thetaMax=NguongMax(proimage,m);
	/*for(i=0;i<m;i++)
		theta+=(proimage[i][0]-proimage[i][1])*(proimage[i][0]-proimage[i][1]);
	thetaMax=theta*10;*/
	////khoang cach anh khoi fuc voi anh ban dau
	float kq=0;
	for(i=0;i<n;i++)
		kq+=(ve_im_test[i]-re_test_image[i])*(ve_im_test[i]-re_test_image[i]);
	double a= sqrt(kq);
		if(temp1>65000000)//thetaMax)          //temp khoang cach be nhat anh test->csdl,
	{       
		TextOut(hdc,400,100,"Anh Khong Co Trong CSDL", ARRAYSIZE("Anh Khong Co Trong CSDL"));

		return;
	}
	////
	//if( temp>
	/////
	//double esilon=0.0;
	//for(i=0;i<n;i++)
	//	esilon+=(ve_im_test[i]-re_test_image[i])*(ve_im_test[i]-re_test_image[i]);
	///////
	//if(esilon>(double)thetaMax)
	//	return;
		
	/*if(temp>thetaMax)         //temp chinh la khoang cach be nhat tu anh test den lan luot anh in csdl
		return;*/
	////hien thi anh ket qua
	FILE *fpText2;
	double min,max;
	float *tem;
	tem = vector(n); 
	/*fpText2 = fopen("E:\\khoi.txt","wt");
	ghifile2(fpText2,n,re_test_image);*/
	//====tim min
		min=re_test_image[0];
		for(i=0;i<n;i++)
			if( re_test_image[i]<min)
				min=re_test_image[i];

	//----min
	//=====tru min
		for(i=0;i<n;i++)
			tem[i]=re_test_image[i]-min;
	//max
		max=re_test_image[0];
		for(i=0;i<n;i++)
			if( tem[i]>max)
				max=tem[i];
	//---max
	//chuan hoa
		for(i=0;i<n;i++)
			tem[i]=(tem[i]/max)*255;
	
	fpText2 = fopen("E:\\khoiphuc.txt","wt");
	ghifile2(fpText2,n,tem);
	j=0;
	for(i=0;i<n;i++)
	{
		if(i%daianh==0)
			j++;
		SetPixel(hdc,i-(j-1)*daianh+600,j,RGB((BYTE)tem[i],(BYTE)tem[i],(BYTE)tem[i]));//re_test_image[i],re_test_image[i],re_test_image[i]));
	}
	///////////
	j=0; 
	for(i=0;i<n;i++)
	{
		if(i%daianh==0)
			j++;
		SetPixel(hdc,i-(j-1)*daianh+400,j,RGB(dataSrc[i][similar_im]+mean[i],dataSrc[i][similar_im]+mean[i],dataSrc[i][similar_im]+mean[i]));
	}
	Free2dFloat(dataSrc);
	Free2dFloat(symmat);
	Free2dFloat(a1);
	Free2dFloat(proimage);
	Free2dFloat(eigenface);
	free(evals);
	free(interm);
	ReleaseDC(hWnd,hdc1);
}
//////////// lam lai anh ko la mat
int KhongLaMat(float **dataSrc,float **eigenface,int n, int m,HDC hdc)
{
	float x=0,y=0, detax=0,detay=0,xx=0,yy=0,xy=0,co=0,a=0,b=0;
	for(i=0;i<n;i++)
	{
		x+=ve_im_test[i]+mean[i];
		y+=mean[i];//eigenface[0][i];
		//y+=dataSrc[i][0]+mean[i];
	}
	x/=n; y/=n;
	for(i=0;i<n;i++)
	{
		detax=ve_im_test[i]+mean[i]-x;
		detay=mean[i]-y;//eigenface[0][i]-y;
		a=a+detax;
		b=b+detay;
		xx+=detax*detax;
		yy+=detay*detay;
		xy+=detax*detay;
	}
	
	
	co=xy/(sqrt(xx)*sqrt(yy));
	if(co<=0.25)
		return 0;
	else
		return 1;
	//{
	//	x=0,y=0, detax=0,detay=0,xx=0,yy=0,xy=0,co=0,a=0,b=0;
	//	for(i=0;i<n;i++)
	//	{
	//		x+=ve_im_test[i];//+mean[i];
	//		y+=eigenface[0][i];
	//	}
	//	x/=n; y/=n;
	//	for(i=0;i<n;i++)
	//	{
	//		detax=ve_im_test[i]-x;
	//		detay=eigenface[0][i]-y;
	//		a=a+detax;
	//		b=b+detay;
	//		xx+=detax*detax;
	//		yy+=detay*detay;
	//		xy+=detax*detay;
	//	}
	//
	//	co=xy/(sqrt(xx)*sqrt(yy));
	//	if(co<=0)
	//		return 0;
	//	else
	//		return 1;
	//}
}

///////////
int AnhKhongLaMat(float **dataSrc,float **symmat,int n,int m,HDC hdc)
{
	float * interm1;
	interm1=vector(m);
	float ** phuchoi;
	
	phuchoi=Alloc2dFloat(n,m);      //phuc hoi qua su chuyen doi anh chieu ban dau, co cho anh test thay 1 anh trong csdl de chieu phuc hoi roi so sanh voi anh ban dau
	
	for(i=0;i<m;i++)
		for(j=0;j<n;j++)
			phuchoi[j][i]=dataSrc[j][i];
	for(i=0;i<n;i++)
		phuchoi[i][5]=ve_im_test[i];
	// chieu anh len dac trung mat
	 for (i = 0; i < n; i++) {
      for (j = 0; j <m; j++) {
        interm1[j] = phuchoi[i][j]; }   /* data[i][j] will be overwritten */
         for (int k = 0; k < m; k++) {
          phuchoi[i][k] = 0.0;
          for (k2 = 0; k2 < m; k2++) {
            phuchoi[i][k] += interm1[k2] * symmat[k2][k];//[m-k+1];
		  }
		  
        }
     }

	 j=0; 
	for(i=0;i<n;i++)
	{
		if(i%daianh==0)
			j++;
		SetPixel(hdc,i-(j-1)*daianh+400,j+300,RGB(ve_im_test[i],ve_im_test[i],ve_im_test[i]));
	}
	j=0; 
	for(i=0;i<n;i++)
	{
		if(i%daianh==0)
			j++;
		SetPixel(hdc,i-(j-1)*daianh+600,j+300,RGB(phuchoi[i][5]+mean[i],phuchoi[i][5]+mean[i],phuchoi[i][5]+mean[i]));
	}
	 //phuc hoi
	 //for (i = 0; i < n; i++){ 
  //    for (j = 0; j <m; j++)
	 // {
		//  interm1[j] = phuchoi[i][j];}
	 // for (int k = 0; k < m; k++) {
		//	  phuchoi[i][k] = 0.0;
		//	  for (k2 = 0; k2 < m; k2++) {
		//		phuchoi[i][k] += interm1[k2] * symmat[k][k2]; }
		//	//  phuchoi[i][k]+=mean[k];
	 // }
	 //}
	
	 ///////tuong quan: corr
	//trung binh
	for(i=0;i<n;i++)
	{
		ve_im_test[i]=ve_im_test[i]+mean[i];
		phuchoi[i][5]=phuchoi[i][5]+mean[i];
	}
	double t=0.0;
	for( i=0;i<n;i++)
		t+=(ve_im_test[i]-phuchoi[i][5])*(ve_im_test[i]-phuchoi[i][5]);
	float x=0,y=0, detax=0,detay=0,xx=0,yy=0,xy=0,co=0,a=0,b=0;

	x=sqrt(t);
	if(x>18000.0)
		return 0;
	else
		return 1;;
	/*for(i=0;i<n;i++)
	{
		x+=ve_im_test[i];
		y+=phuchoi[i][5];
	}
	x/=n; y/=n;
	for(i=0;i<n;i++)
	{
		detax=ve_im_test[i]-x;
		detay=phuchoi[i][5]-y;
		a=a+detax;
		b=b+detay;
		xx+=detax*detax;
		yy+=detay*detay;
		xy+=detax*detay;
	}
	
	
	co=xy/(sqrt(xx)*sqrt(yy));
	if(co<0.4)
		return 0;
	else
		return 1;*/
}
//////
//////////
void tinh_eigenface(float **dataSrc, float **symmat, int n,int m,float **eigenface)
{
	int j1,j2;
	
	for (j1 = 0; j1 < n; j1++)
    {
    for (j2 = 0; j2 < m; j2++)
        {
        eigenface[j1][j2] = 0.0;
        for (i = 0; i < m; i++)
            {
            eigenface[j1][j2] += dataSrc[j1][i] * symmat[i][j2];
            }
		}
    }
	/*fpText = fopen("E:\\data2.txt","wt");
	ghifile(fpText,m,m,dataSrc);
	*/
	/*FILE *fpText;
	fpText = fopen("E:\\eigenface.txt","wt");
	ghifile(fpText,n,m,eigenface);
	*/
}
///////
///////// chieu data len eigenface
void chieu(float **eigenface, float **dataSrc,float **proimage,int n,int m)
{
	
int j1,j2;
	for (j1 = 0; j1 < m/*-10*/; j1++)
    {
    for (j2 = 0; j2 < m; j2++)
        {
        proimage[j1][j2] = 0.0;
        for (i = 0; i < n; i++)
            {
            proimage[j1][j2] += eigenface[i][j1] * dataSrc[i][j2];
            }
		}
    }
	FILE *fpText;
	fpText = fopen("E:\\pro.txt","wt");
	ghifile(fpText,m,m,proimage);
	
}
////////

///////
void chieu_anhtest(float **eigenface,float *ve_im_test,float **proimage,int n,int m)
{
	int j2;
	for (j2 = 0; j2 < m/*-10*/; j2++)
        {
        proimage[j2][m] = 0.0;
        for (i = 0; i < n; i++)
            {
            proimage[j2][m] += eigenface[i][j2] * ve_im_test[i];
            }
		}
	FILE *fpText;
	fpText = fopen("E:\\pro5.txt","wt");
	ghifile(fpText,m,m+1,proimage);
}	

//////

//////
float NguongMax(float  **proimage,int m)
{
	float theta=0,maxtheta=0;
	float kq=0;
	for(i=0;i<m;i++)
		theta+=(proimage[i][0]-proimage[i][1])*(proimage[i][0]-proimage[i][1]);
	//kq=theta*5;
	
	/*for(i=0;i<m-1;i++)
		for(j=i+1;j<m;j++)
		{
			theta=0;
			for(int k=0;k<m;k++)
				theta+=(proimage[k][i]-proimage[k][j])*(proimage[k][i]-proimage[k][j]);
			if(maxtheta<theta)
				maxtheta=theta;
		}
	float a= sqrt(maxtheta)/2;*/
	double k=sqrt(theta);
	return k;
}
///////////
int sosanh(float **proimage,int n,int m)
{
	dis=vector(m);
	for(i=0;i<m;i++)
	{
		for(j=0;j<m/*-10*/;j++)
		{
			dis[i]+=(proimage[j][m]-proimage[j][i])*(proimage[j][m]-proimage[j][i]);   //khoang cach Euclicde
			//dis[i]+=abs(proimage[j][i]-proimage[j][m])*sqrt(evals[i]);          //kcach Mahalobis
			
		}
		//dis[i]=sqrt(dis[i]);
		
	}
	FILE * fpText1;
	fpText1 = fopen("E:\\dis.txt","wt");
	ghifile2(fpText1,m,dis);
	float be=dis[0];
	
	int similar_im=0;
	for(i=0;i<m;i++)
		if(dis[i]<be)
		{
			be=dis[i];
			
			similar_im=i;
		}
	temp1=sqrt(be);
	return similar_im;
}
////
////
void sapxep_vector(float **symmat,int n,int m)
{
	float * temp, t_evalue;
	int k=0;
	temp=vector(m);i=j=0;
	for(i=0;i<m-1;i++)
		for(j=i+1;j<m;j++)
			if(evals[i]<evals[j])
			{
				for(k=0;k<m;k++)
					temp[k]=symmat[k][i];  
				t_evalue=evals[i];
				for(k=0;k<m;k++)
					symmat[k][i]=symmat[k][j]; 
				evals[i]=evals[j];
				for(k=0;k<m;k++)
					symmat[k][j]=temp[k]; 
				evals[j]=t_evalue;

			}
			FILE * fpText2;
	fpText2 = fopen("E:\\sy.txt","wt");
	ghifile(fpText2,m,m,symmat);
	fpText2 = fopen("E:\\ev.txt","wt");
	ghifile2(fpText2,m,evals);
}
/////
/**  Variance-covariance matrix: creation  *****************************/
void ghifile(FILE *fp,int n,int m,float **dulieu)
{
	 for (i = 0; i < n; i++) 
      for (j = 0; j < m; j++) 
	  {
		  if(j==0)
		  {fprintf(fp,"   \n   "); }
		  fprintf(fp,"    %10.3f              ",dulieu[i][j]);
		  
	  }

}
void ghifile2(FILE *fp,int m,float *dulieu)
{
	  for (j = 0; j < m; j++) 
	  {
		  fprintf(fp,"%10.20f\n",dulieu[j]);
	  }

}

void covcol(float **dataS, int n, int m, float **symmat,HDC hdc,HWND hWnd)
/* Create m * m covariance matrix from given n * m data matrix. */
{

int i, j, j1, j2;
HDC hdc1;
hdc1=GetDC(hWnd);
int x=200;
float tong=0;
FILE * fpText;
mean=vector(n);
	for(i=0;i<n;i++)
	{
		mean[i]=0.0;
		tong=0;
		for(j=0;j<m;j++)
			tong+=dataS[i][j];
		mean[i]=tong/m;
	}


fpText = fopen("E:\\mean.txt","wt");
	ghifile2(fpText,n,mean);

/* Calculate the m * m covariance matrix. */
for (j1 = 0; j1 < m; j1++)
    {
    for (j2 = j1; j2 < m; j2++)
        {
        symmat[j1][j2] = 0.0;
        for (i = 0; i < n; i++)
            {
            symmat[j1][j2] += (dataS[i][j1]-mean[i]) * (dataS[i][j2]-mean[i]) ;
            }
		//symmat[j1][j2]/=n;
        symmat[j2][j1] = symmat[j1][j2];
        }
    }

	fpText = fopen("E:\\cov.txt","wt");
ghifile(fpText,m,m,symmat);
return;

}



/**  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */

//void tred2(float **a1, int n, float *d, float *e)
///* Householder reduction of matrix a to tridiagonal form.
//   Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
//   Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
//        Springer-Verlag, 1976, pp. 489-494.
//        W H Press et al., Numerical Recipes in C, Cambridge U P,
//        1988, pp. 373-374.  */
//{
//int l, k, j, i;
//float scale, hh, h, g, f;
//
//
//for (i = n; i >= 2; i--)
//    {
//    l = i - 1;
//    h = scale = 0.0;
//    if (l > 1)
//       {
//       for (k = 1; k <= l; k++)
//           scale += fabs(a1[i][k]);
//       if (scale == 0.0)
//          e[i] = a1[i][l];
//       else
//          {
//          for (k = 1; k <= l; k++)
//              {
//              a1[i][k] /= scale;
//              h += a1[i][k] * a1[i][k];
//              }
//          f = a1[i][l];
//          g = f>0 ? -sqrt(h) : sqrt(h);
//          e[i] = scale * g;
//          h -= f * g;
//          a1[i][l] = f - g;
//          f = 0.0;
//          for (j = 1; j <= l; j++)
//              {
//              a1[j][i] = a1[i][j]/h;
//              g = 0.0;
//              for (k = 1; k <= j; k++)
//                  g += a1[j][k] * a1[i][k];
//              for (k = j+1; k <= l; k++)
//                  g += a1[k][j] * a1[i][k];
//              e[j] = g / h;
//              f += e[j] * a1[i][j];
//              }
//          hh = f / (h + h);
//          for (j = 1; j <= l; j++)
//              {
//              f = a1[i][j];
//              e[j] = g = e[j] - hh * f;
//              for (k = 1; k <= j; k++)
//                  a1[j][k] -= (f * e[k] + g * a1[i][k]);
//              }
//         }
//    }
//    else
//        e[i] = a1[i][l];
//    d[i] = h;
//    }
//d[1] = 0.0;
//e[1] = 0.0;
//for (i = 1; i <= n; i++)
//    {
//    l = i - 1;
//    if (d[i])
//       {
//       for (j = 1; j <= l; j++)
//           {
//           g = 0.0;
//           for (k = 1; k <= l; k++)
//               g += a1[i][k] * a1[k][j];
//           for (k = 1; k <= l; k++)
//               a1[k][j] -= g * a1[k][i];
//           }
//       }
//       d[i] = a1[i][i];
//       a1[i][i] = 1.0;
//       for (j = 1; j <= l; j++)
//           a1[j][i] = a1[i][j] = 0.0;
//    }
//
//
//}
//
///**  Tridiagonal QL algorithm -- Implicit  **********************/
//
//void tqli(float *d, float *e, int n, float **z, HWND hWnd)
//{
//int m, l, iter, i, k;
//float s, r, p, g, f, dd, c, b;
//
//for (i = 2; i <= n; i++)
//    e[i-1] = e[i];
//e[n] = 0.0;
//for (l = 1; l <= n; l++)
//    {
//    iter = 0;
//    do
//      {
//      for (m = l; m <= n-1; m++)
//          {
//          dd = fabs(d[m]) + fabs(d[m+1]);
//          if (fabs(e[m]) + dd == dd) break;
//          }
//          if (m != l)
//             {
//            // if (iter++ == 30) erhand("No convergence in TLQI.",hWnd);
//             g = (d[l+1] - d[l]) / (2.0 * e[l]);
//             r = sqrt((g * g) + 1.0);
//             g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
//             s = c = 1.0;
//             p = 0.0;
//             for (i = m-1; i >= l; i--)
//                 {
//                 f = s * e[i];
//                 b = c * e[i];
//                 if (fabs(f) >= fabs(g))
//                    {
//                    c = g / f;
//                    r = sqrt((c * c) + 1.0);
//                    e[i+1] = f * r;
//                    c *= (s = 1.0/r);
//                    }
//                 else
//                    {
//                    s = f / g;
//                    r = sqrt((s * s) + 1.0);
//                    e[i+1] = g * r;
//                    s *= (c = 1.0/r);
//                    }
//                 g = d[i+1] - p;
//                 r = (d[i] - g) * s + 2.0 * c * b;
//                 p = s * r;
//                 d[i+1] = g + p;
//                 g = c * r - b;
//                 for (k = 1; k <= n; k++)
//                     {
//                     f = z[k][i+1];
//                     z[k][i+1] = s * z[k][i] + c * f;
//                     z[k][i] = c * z[k][i] - s * f;
//                     }
//                 }
//                 d[l] = d[l] - p;
//                 e[l] = g;
//                 e[m] = 0.0;
//             }
//          }  while (m != l);
//      }
// } 
//
//void erhand(char *err_msg,HWND hWnd)
///* Error handler */
//{
//	MessageBox(hWnd,err_msg,"Error",TRUE);
//   // fprintf(stderr,"Run-time error:\n");
//    //fprintf(stderr,"%s\n", err_msg);
//    //fprintf(stderr,"Exiting to system.\n");
//    exit(1);
//}
//
//
void tred2(float **a, int n, float *d, float *e)
/* Householder reduction of matrix a to tridiagonal form.
   Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
   Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
        Springer-Verlag, 1976, pp. 489-494.
        W H Press et al., Numerical Recipes in C, Cambridge U P,
        1988, pp. 373-374.  */
{
int l, k, j, i;
float scale, hh, h, g, f;

for (i = n; i >= 2; i--)
    {
    l = i - 1;
    h = scale = 0.0;
    if (l > 1)
       {
       for (k = 1; k <= l; k++)
           scale += fabs(a[i][k]);
       if (scale == 0.0)
          e[i] = a[i][l];
       else
          {
          for (k = 1; k <= l; k++)
              {
              a[i][k] /= scale;
              h += a[i][k] * a[i][k];
              }
          f = a[i][l];
          g = f>0 ? -sqrt(h) : sqrt(h);
          e[i] = scale * g;
          h -= f * g;
          a[i][l] = f - g;
          f = 0.0;
          for (j = 1; j <= l; j++)
              {
              a[j][i] = a[i][j]/h;
              g = 0.0;
              for (k = 1; k <= j; k++)
                  g += a[j][k] * a[i][k];
              for (k = j+1; k <= l; k++)
                  g += a[k][j] * a[i][k];
              e[j] = g / h;
              f += e[j] * a[i][j];
              }
          hh = f / (h + h);
          for (j = 1; j <= l; j++)
              {
              f = a[i][j];
              e[j] = g = e[j] - hh * f;
              for (k = 1; k <= j; k++)
                  a[j][k] -= (f * e[k] + g * a[i][k]);
              }
         }
    }
    else
        e[i] = a[i][l];
    d[i] = h;
    }
d[1] = 0.0;
e[1] = 0.0;
for (i = 1; i <= n; i++)
    {
    l = i - 1;
    if (d[i])
       {
       for (j = 1; j <= l; j++)
           {
           g = 0.0;
           for (k = 1; k <= l; k++)
               g += a[i][k] * a[k][j];
           for (k = 1; k <= l; k++)
               a[k][j] -= g * a[k][i];
           }
       }
       d[i] = a[i][i];
       a[i][i] = 1.0;
       for (j = 1; j <= l; j++)
           a[j][i] = a[i][j] = 0.0;
    }
}

/**  Tridiagonal QL algorithm -- Implicit  **********************/

void tqli(float *d, float *e, int n, float **z, HWND hWnd)
{
int m, l, iter, i, k;
float s, r, p, g, f, dd, c, b;

for (i = 2; i <= n; i++)
    e[i-1] = e[i];
e[n] = 0.0;
for (l = 1; l <= n; l++)
    {
    iter = 0;
    do
      {
      for (m = l; m <= n-1; m++)
          {
          dd = fabs(d[m]) + fabs(d[m+1]);
          if (fabs(e[m]) + dd == dd) break;
          }
          if (m != l)
             {
            // if (iter++ == 30) erhand("No convergence in TLQI.",hWnd);
             g = (d[l+1] - d[l]) / (2.0 * e[l]);
             r = sqrt((g * g) + 1.0);
             g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
             s = c = 1.0;
             p = 0.0;
             for (i = m-1; i >= l; i--)
                 {
                 f = s * e[i];
                 b = c * e[i];
                 if (fabs(f) >= fabs(g))
                    {
                    c = g / f;
                    r = sqrt((c * c) + 1.0);
                    e[i+1] = f * r;
                    c *= (s = 1.0/r);
                    }
                 else
                    {
                    s = f / g;
                    r = sqrt((s * s) + 1.0);
                    e[i+1] = g * r;
                    s *= (c = 1.0/r);
                    }
                 g = d[i+1] - p;
                 r = (d[i] - g) * s + 2.0 * c * b;
                 p = s * r;
                 d[i+1] = g + p;
                 g = c * r - b;
                 for (k = 1; k <= n; k++)
                     {
                     f = z[k][i+1];
                     z[k][i+1] = s * z[k][i] + c * f;
                     z[k][i] = c * z[k][i] - s * f;
                     }
                 }
                 d[l] = d[l] - p;
                 e[l] = g;
                 e[m] = 0.0;
             }
          }  while (m != l);
      }
 } 

