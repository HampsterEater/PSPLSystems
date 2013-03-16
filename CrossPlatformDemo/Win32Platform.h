// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------

#ifndef _win32platform_h_
#define _win32platform_h_

#include "main.h"
#include "Common.h"
#include "Cube.h"

#ifdef WINDOWS

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include <math.h>

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
#endif