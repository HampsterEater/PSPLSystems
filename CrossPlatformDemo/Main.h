// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------

#ifndef _main_h_
#define _main_h_

#include "Common.h"
#include "Platform.h"

// =======================================================
// Grid cube structure.
// =======================================================
typedef struct 
{
	int Visible;
	int Color;
	int Sequence;
	int SubSequence;
} GridCube;

// =======================================================
// Function Prototypes.
// =======================================================
void CrossPlatformEntryPoint();
void Update();
void Render();
void Initialize();
void Deinitialize();

int InterpColor(int start, int end, float delta);

void SetCubePixel(int x, int y, int color, int sequence, int subSequence);
int DrawLineBetweenPoints(int x1, int y1, int x2, int y2, int offset);
int PaintLSystem(int offset, double cx, double cy, double ca, int depth, int sequence);
void IterateLSystem(unsigned int maxGenerations);

void GenerateSounds();
void GenerateTree();

void PlayPCMSound(int index);

#endif