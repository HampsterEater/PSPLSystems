// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------

#ifndef _platform_h_
#define _platform_h_

#include "Common.h"

// Platform independent structs and defs.
typedef struct 
{
	float x, y, z;
} P_Vector3;

#define SOUND_FORMAT_MONO 1
#define MAX_VOLUME 100
#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2
#define PRIM_TYPE_TRIANGLES 1
#define VERTEX_TYPE_FLOAT 1

// Define all functions that need to be implemented per platform.
void P_InitializeScreen();
void P_InitializeSound();

double P_Sin(double x);
double P_Cos(double x);

bool P_CloseRequested();
void P_FlipBuffers();
void P_StartFrame();
void P_FinishFrame();
void P_ClearBuffer(int color);
void P_SetColor(int color);
void P_DebugPrint(int x, int y, int color, const char* txt);
bool P_ButtonDown(int button);
void P_Translate(P_Vector3* val);
void P_Scale(P_Vector3* val);
void P_RotateY(double val);
void P_PushMatrix();
void P_PopMatrix();
void P_DrawCube();
void P_PlayPCM(int sampleRate, int format, int maxVolumeLeft, int maxVolumeRight, int channel, void* data, int dataSize);
unsigned long long P_GetMillisecs();
void P_Sleep(int val);

// Include correct platform header.
#ifdef WINDOWS
#include "Win32Platform.h"
#else
#include "PSPPlatform.h"
#endif

#endif