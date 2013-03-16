// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------

#ifndef _common_h_
#define _common_h_

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define WIN32/PSP shortcut macros.
//#ifdef SN_TARGET_PSP_PRX 
//#define PSP
//#else
//#define WINDOWS
//#endif

#ifdef _WINDOWS
#define WINDOWS
#else
#define PSP
#endif

// =======================================================
// Some helpful macros / defines.
// =======================================================
#define bool int
#define false 0
#define true 1

#define GRID_WIDTH 129
#define GRID_HEIGHT 129

#define MAX_TREE_LEN 102400
#define PI 3.14159265

#define SUB_SEQUENCE_DELAY 10
#define MAX_GENERATIONS 4
#define LAST_GENERATION_SUB_SEQUENCES 150

#define START_COLOR 0xFF112299
#define END_COLOR   0xFF00FF99

#define MAX_SOUNDS 4
#define MAX_SOUND_CHANNELS_TO_USE 3
#define MAX_SOUNDS_IN_TRACK 8

#define MAX_CUBES_PER_TREE 5547

// =======================================================
// Pusedo Functions
// =======================================================
#define ClearGrid() memset(g_cubeGrid, 0, GRID_WIDTH * GRID_HEIGHT * sizeof(GridCube));
#define Sign(x) (x > 0) - (x < 0)
#define Swap(x,y) { x = x ^ y; y = x ^ y; x = x ^ y; }

// More efficient than macro.
#define ABS(x) (x >= 0 ? x : -x)

#define GetA(x) ((x&0xff000000)>>24)
#define GetR(x) ((x&0x00ff0000)>>16)
#define GetG(x) ((x&0x0000ff00)>>8)
#define GetB(x) (x&0x000000ff)

#define DegToRad(x) (x / 57.29578)
#define RadToDeg(x) (x * 57.29578)

#endif
