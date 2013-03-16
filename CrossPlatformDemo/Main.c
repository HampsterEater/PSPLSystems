// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------

#include "Main.h"
#include "Cube.h"

// =======================================================
// Global variables.
// =======================================================

// Cube grid variables.
GridCube g_cubeGrid[GRID_WIDTH][GRID_HEIGHT];

// FPS tracking variables.
long long g_fpsTimer;
int g_currentFps;
int g_fpsTicks;

#define TARGET_FPS 30
int g_renderTime;

// L-System variables.
// --------------------------
// F=A  = Forward
// +=B  = Angle Plus
// -=C  = Angle Minus
// [=D  = Save Position
// ]=E  = Restore Position
// --------------------------
char* g_rule[6] = { "AACDCABACAEBDBACACAE", "B", "C", "D", "E" }; // FF-[-F+F+F]+[+F-F-F]
char* g_axiom = "A";
char g_tree[MAX_TREE_LEN];

// Tree rendering variables.
double g_treeAngle = 30.0 * (PI / 180.0); // 22.0
double g_treeSize = 5.0; // 4.0
double g_treeRotationAngle = 0.0f;

// Sequence rendering variables.
int g_lSystemSequence;
int g_currentLSystemSequence;
int g_currentSubSequence;

int g_totalSubSequences[MAX_GENERATIONS + 1];
long long g_subSequenceTimer;
int g_lSystemTotalSubSequences;
int g_lSystemSubSequenceCounter;
int g_lSystemSubSequenceColor;

// These variables just keep track of the start cubes position.
#define g_startCubeX GRID_WIDTH / 2
#define g_startCubeY 20

// These variables store the sound effects.
short* g_soundData[MAX_SOUNDS];
int g_soundDataSize[MAX_SOUNDS];
int g_sampleRate;

// These variables store our simple crappy music track.
int g_musicTrack[MAX_SOUNDS_IN_TRACK] = { 1, 1, 2, 1, 1, 2, 3, 2 };
int g_musicTrackDelay[MAX_SOUNDS_IN_TRACK] = { 300, 300, 300, 300, 300, 300, 300,  300 };
int g_musicTrackCursor = 0;
int g_musicTrackTimer = 0;

// If set this stops the loading bar from being shown when
// trees are being generated.
bool g_loadingOff = false;

// =======================================================
// Function definitions.
// =======================================================

// =======================================================
// This is invoked by the platform specific entry point (the actual
// main function). This is where the actual code should take place.
// =======================================================
void CrossPlatformEntryPoint()
{
	// Initialize the graphics system.
	Initialize();
	
	// Generate all sound effects.
	GenerateSounds();

	// Initialize tree.
	GenerateTree();
		
	// Turn off loading bar.
	g_loadingOff = true;
		
	// Run the main loop.
	while (!P_CloseRequested())
	{
		Update();
		Render();
	
		P_FlipBuffers();
	}

	// Deallocate resources.
	// TODO
}


// =======================================================
// Draws the loading bar to the screen.
// =======================================================
void DrawLoadingBar(char* phrase, float progress)
{	
	char convertBuffer[256];
	
	// If we don't want to show the loading bar, then return.
	if (g_loadingOff == true)
		return;
	
	// Clear the screen	
	P_StartFrame();
	P_ClearBuffer(0x00000000);

	// Draw loading text.
	P_SetColor(0xFFFFFFFF);
	sprintf(((char*)&convertBuffer), "%s - %f%%", phrase, progress);
	P_DebugPrint(35, 272 - 35, 0xFFFFFFFF, convertBuffer);
	
	// Finish rendering and sync.
	P_FinishFrame();

	// Swap buffers.
	P_FlipBuffers();
}

// =======================================================
// Generates all the sound effects. 
// =======================================================
void GenerateSounds()
{
	// Work out size and duration of pcm waveform data.
	float duration = 100.0f; // 100.0f
	int sampleCount = 256;
	int bytesPerSample = sizeof(short);
	int waveFormSize = duration * (bytesPerSample * sampleCount);
	int samples = waveFormSize / bytesPerSample;
	int sound = 0;
	int i = 0;
	float fractionPerSound = 0;
	float f = 0;
	float fractionOf2PI = 0;
	double sine = 0;
	double si = 0;

	// Update progress.
	DrawLoadingBar("Generating Sounds", 0);

	// Fill memory with simple sine wave.
	sound = 0;
	i = 0;
	fractionPerSound = 100.0f / MAX_SOUNDS;
	
	for (sound = 0; sound < MAX_SOUNDS; sound++)
	{
		// Allocate memory for sound.
		g_soundDataSize[sound] = waveFormSize;
		g_soundData[sound] = (short*)malloc(g_soundDataSize[sound]);

		// Generate sample data.
		for (i = 0; i < samples; i++)
		{	
			// Update loading bar.
			if (i % (samples / 10) == 0)
			{
				f = (fractionPerSound * sound) + (fractionPerSound * ((float)i / (float)samples));
				DrawLoadingBar("Generating Sounds", f);
			}

			// Work out the pcm value for this sample.
			// Make increasing pitchs for each successive sound.
			fractionOf2PI = (samples / (200.0f + (200.0f * sound)));
			sine = P_Sin(i * (2 * PI / fractionOf2PI));
			si = (32767 * 0.5f) * sine;
			g_soundData[sound][i] = si;
		}
	}
	
	// Set sample, format and volume for each channel.
	g_sampleRate = sampleCount * duration;
}

// =======================================================
// Generates a new randomised lsystem tree.
// =======================================================
void GenerateTree()
{
	// Update progress.
	DrawLoadingBar("Generating Trees", 0);
	
	// Clear L System.
	ClearGrid();
	
	// Generate the first L-System.
	IterateLSystem(MAX_GENERATIONS);
	
	// Update progress.
	DrawLoadingBar("Generating Trees", 50);
	
	// Draw the L-System
	g_totalSubSequences[MAX_GENERATIONS] = LAST_GENERATION_SUB_SEQUENCES;
	PaintLSystem(0, g_startCubeX, g_startCubeY, 90.0 * (PI / 180.0), 1, 0);
	
	// Reset sequence counters.
	g_currentLSystemSequence = 0;
	g_lSystemSubSequenceCounter = 0;
			
	// Update progress.
	DrawLoadingBar("Generating Trees", 100);
}

// =======================================================
// Intializes the demo.
// =======================================================
void Initialize()
{
	// Initialize graphics system.
	P_InitializeScreen();

	// Initialize sound.
	P_InitializeSound();
}

// =======================================================
// Updates the demo.
// =======================================================
void Update()
{
	// Find the max sub sequence for this generation
	int maxSubSeq = g_totalSubSequences[g_currentLSystemSequence];

	// Increment sub sequence.
	if (P_GetMillisecs() - g_subSequenceTimer > SUB_SEQUENCE_DELAY)
	{
		g_currentSubSequence++;
		g_lSystemSubSequenceCounter++;
		g_subSequenceTimer = P_GetMillisecs();
	}
	
	// Work out color for current sub sequence.
	g_lSystemSubSequenceColor = InterpColor(START_COLOR, END_COLOR, (float)g_lSystemSubSequenceCounter / (float)g_lSystemTotalSubSequences); 
	
	// Move to next generation?
	if (g_currentSubSequence > maxSubSeq)
	{
		g_currentSubSequence = 0;
		g_currentLSystemSequence++;
		
		// Gone through all generations?
		if (g_currentLSystemSequence > MAX_GENERATIONS)
		{
			// Regenerate tree.
			GenerateTree();
		}
	}
	
	// Play our crappy background music.
	if (P_GetMillisecs() >= g_musicTrackTimer)
	{
		// Play sound at the current cursor in the music track.
		PlayPCMSound(g_musicTrack[g_musicTrackCursor]);
		
		// Move cursor on and loop if we get to the end.
		if (++g_musicTrackCursor >= MAX_SOUNDS_IN_TRACK)
			g_musicTrackCursor = 0;
		
		// Reset timer for music.
		g_musicTrackTimer = P_GetMillisecs() + g_musicTrackDelay[g_musicTrackCursor];
	}
	
	// Rotate the tree if left or right buttons are pressed.
	if (P_ButtonDown(BUTTON_LEFT) == true) 
		g_treeRotationAngle -= 0.1f;
	if (P_ButtonDown(BUTTON_RIGHT) == true)  
		g_treeRotationAngle += 0.1f;

	// TODO: REPLACE WITH BUILT IN SYSTEM
	// Update the FPS display.
	if (P_GetMillisecs() - g_fpsTimer >= 1000)
	{
		g_currentFps = g_fpsTicks + 1;
		g_fpsTimer = P_GetMillisecs();
		g_fpsTicks = 0;
	}
	else
		g_fpsTicks += 1;

	// Slow down the rendering if its going to fast.
	if (g_renderTime < (1000.0f / TARGET_FPS))
	{
		float freeTime = (1000.0f / TARGET_FPS) - g_renderTime;
		P_Sleep(freeTime);
	}
}

// =======================================================
// Renders the demo.
// =======================================================
void Render()
{
	int startTime = P_GetMillisecs();

	P_Vector3 trans;
	P_Vector3 scale;
	int x, y;
	int renderCounter = 0;
	float s = 0;
	char convertBuffer[256];

	// Clear the screen	
	P_StartFrame();
	P_ClearBuffer(0xFFAAAA00);

	// Render the cube grid.
	P_PushMatrix();

	// Calculate camera translation.
	trans.x = 0.0f;
	trans.y = 0.0f;
	trans.z = -100.0f;
	scale.x = 1.0f;
	scale.y = 1.0f;
	scale.z = 1.0f;

	// Camera translation.
	P_Translate(&trans);
	
	// Render the ground.
	P_PushMatrix();
	P_SetColor(0xFF00AA00);
	
	trans.x = 0.0f;
	trans.y = -40.0f;
	trans.z = 0.0f;
	scale.x = 100.0f;
	scale.y = 20.0f;
	scale.z = 20.0f;
	P_Translate(&trans);
	P_Scale(&scale);
	
	P_DrawCube();
	P_PopMatrix();

	// Camera rotation.
	P_RotateY(g_treeRotationAngle);
	
	// Draw cube grid.
	for (x = 0; x < GRID_WIDTH; x++)
	{
		for (y = 0; y < GRID_HEIGHT; y++)
		{	
			// Check cube is visible.
			if (g_cubeGrid[x][y].Visible == 0)
				continue;
				
			// Check we are rendering this generation.
			if (g_cubeGrid[x][y].Sequence > g_currentLSystemSequence)
				continue;
				
			// Check we are on the right sub-sequence for this generation.
			if (g_cubeGrid[x][y].SubSequence > g_currentSubSequence && g_cubeGrid[x][y].Sequence == g_currentLSystemSequence)
				continue;
				
			// Update cube color.
			if (g_cubeGrid[x][y].SubSequence == g_currentSubSequence && g_cubeGrid[x][y].Sequence == g_currentLSystemSequence)
				g_cubeGrid[x][y].Color = g_lSystemSubSequenceColor;

			P_PushMatrix();
			
			// Translate cube.
			s = 0.6f - (0.15f * g_cubeGrid[x][y].Sequence);
			trans.x = ((x - (GRID_WIDTH / 2)) * s);
			trans.y = ((y - (GRID_WIDTH / 2)) * s);
			trans.z = 0.0f;
			scale.x = s;
			scale.y = s;
			scale.z = s;
			P_Translate(&trans);
			P_Scale(&scale);
			
			// Set cube colour.
			P_SetColor(g_cubeGrid[x][y].Color);

			// Draw cube.
			//P_DrawArray(PRIM_TYPE_TRIANGLES, VERTEX_TYPE_FLOAT, 36, 0, (void*)g_cubeData);
			P_DrawCube();

			P_PopMatrix();
			
			// Count this cube as rendered and break out if over the limit.
			if (++renderCounter >  MAX_CUBES_PER_TREE)
				break;			
		}
	}
	
	// Pop camera translation.
	P_PopMatrix();
	
	// Render the FPS display.
	P_DebugPrint(10, 10, 0xFFFFFFFF, "Timothy Leonard - Randomized \"Cubism\" L-System Tree's");	

#ifdef _WINDOWS
	P_DebugPrint(10, 20, 0xFFFFFFFF, "Arrow Keys - Move Camera");	
#else
	P_DebugPrint(10, 20, 0xFFFFFFFF, "DPad - Move Camera");	
#endif

	sprintf(((char*)&convertBuffer), "FPS - %i", g_currentFps);
	P_DebugPrint(10, 30, 0xFFFFFFFF, convertBuffer);	

	// Finish rendering and sync.
	P_FinishFrame();

	g_renderTime = P_GetMillisecs() - startTime;
}

// =======================================================
// Interpolates a color between 2 points.
// =======================================================
int InterpColor(int start, int end, float delta)
{
	int sr = start >> 16;
	int sg = (start >> 8) & 0xFF;
	int sb = start & 0xFF;
	
	int er = end >> 16;
	int eg = (end >> 8) & 0xFF;
	int eb = end & 0xFF;
	
	int fr = sr + ((er - sr) * delta);
	int fg = sg + ((eg - sg) * delta);
	int fb = sb + ((eb - sb) * delta);
	
	return 255 << 24 | fr << 16 | fg << 8 | fb;
}

// =======================================================
// Sets the colour and visibility of a cube in our 
// cube grid.
// =======================================================
void SetCubePixel(int x, int y, int color, int sequence, int subSequence)
{
	if (subSequence > g_totalSubSequences[sequence])
		g_totalSubSequences[sequence] = subSequence;
	if (x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_WIDTH)
		return;

	g_cubeGrid[x][y].Color = color;
	g_cubeGrid[x][y].Visible = true;
	g_cubeGrid[x][y].Sequence = sequence;
	g_cubeGrid[x][y].SubSequence = subSequence;
}

// =======================================================
// Draws a line between the current points within our 
// "cube grid".
// =======================================================
int DrawLineBetweenPoints(int x1, int y1, int x2, int y2, int off)
{
	int deltaX = 0;
	int deltaY = 0;
	int x = 0;
	int y = 0;
	int xStep = 0;
	int yStep = 0;
	int error = 0;
	bool steep = ABS(y2 - y1) > ABS(x2 - x1);

	if (steep == true)
	{
		Swap(x1, y1);
		Swap(x2, y2);
	}
	
	deltaX = ABS(x2 - x1);
	deltaY = ABS(y2 - y1);
	x = x1;
	y = y1;
	xStep = Sign(x2 - x1);
	yStep = Sign(x2 - x1);

	while (x != x2)
	{
		x += xStep;
		error += deltaY;
		
		if ((error << 1) > deltaX)
		{
			y += yStep;
			error = error - deltaX;
		}
		SetCubePixel(steep == true ? y : x, 
				 steep == true ? x : y, 
				 0xFF000000, g_lSystemSequence, off++);
	}
	
	return off;
}

// =======================================================
// Draws the current LSystem tree to the screen.
// =======================================================
int PaintLSystem(int offset, double cx, double cy, double ca, int depth, int seq)
{
	double x = cx;
	double y = cy;
	double a = ca;
	
	unsigned int i = 0;
	int subSeqOffset = 0;

	double xAmt = 0;
	double yAmt = 0;

	int* addr = NULL;
	int* endAddr = NULL;

	// Iterate through each character in our string and
	// perform the operation it refers to.
	for (i = offset; i < strlen(g_tree); i++)
	{
		switch (g_tree[i])
		{
			// Draw foreward.
			case 'A':
			{
				// Work out amount to move foreward by.
				xAmt = g_treeSize * P_Cos(a);
				yAmt = g_treeSize * P_Sin(a);
				
				// Draw the actual line between the points.
				g_lSystemSequence = seq;
				subSeqOffset = DrawLineBetweenPoints((int)x, (int)y, (int)(x + xAmt), (int)(y + yAmt), subSeqOffset);
				
				// Move foreward.
				x += xAmt;
				y += yAmt;
				
				break;
			}
			
			// Save current position and angle.
			case 'D':	i = PaintLSystem(i + 1, x, y, a, depth + 1, seq + 1);	break;
			
			// Restore current position and angle.
			case 'E':	return i;

			// Turn right.
			case 'B':	a += g_treeAngle; 	break;
			
			// Turn left.
			case 'C':	a -= g_treeAngle;  	break;
		}
	}

	// Calculate total sub sequences.
	// Find the max sub sequence for this generation
	g_lSystemTotalSubSequences = LAST_GENERATION_SUB_SEQUENCES;

	// 4 bytes cheaper than a for loop >_>
	addr = g_totalSubSequences;
	endAddr = g_totalSubSequences + MAX_GENERATIONS;
	while (addr != endAddr)
		g_lSystemTotalSubSequences += *(addr++);

	return 0;
}

// =======================================================
// Performs maxGeneration iterations using the
// current Lsystem rules.
// =======================================================
void IterateLSystem(unsigned int maxGenerations)
{
	unsigned int num, i;
	char newTree[MAX_TREE_LEN];
	int index;
	int len;

	// Set default tree to starting axiom.
	strcpy(g_tree, g_axiom); // Don't need to clear array, when we copy that should be enough as we add a \0 on the end.
	
	// Iterate through as many generations as we want to create.
	num = maxGenerations;
	while (num--) // Cheaper than for loop.
	{
		// Create a new string to store our new tree.
		newTree[0] = '\0'; // Cheaper than a memset or static array initialization.
		
		// Iterate through previous tree and match characters.
		for (i = 0; i < strlen(g_tree); i++)
		{
			// Index into rule and alphabet array.
			index = g_tree[i] - 'A';
			
			// Generate a random number between 0-2, 
			// if its not 0 then apply rule, otherwise don't do anything.
			// - This just produces a simple, but crude, randomised tree effect.
			if (rand() % 3 != 0)// || g_tree[i] == "A")
			{
				strcat((char*)&newTree, g_rule[index]);
			}
			else
			{
				len = strlen(newTree);
				newTree[len] = g_tree[i];
				newTree[len + 1] = '\0';
			}
		}
		
		// Copy the new tree to the main tree.
		strcpy(g_tree, (char*)&newTree);
	}
}

// =======================================================
// Plays the sound in the given buffer.
// =======================================================
void PlayPCMSound(int index)
{
	static int channel = 0;
	if (++channel >= MAX_SOUND_CHANNELS_TO_USE)
		channel = 0;
	
	P_PlayPCM(g_sampleRate, SOUND_FORMAT_MONO, MAX_VOLUME, MAX_VOLUME, channel, g_soundData[index], g_soundDataSize[index]);
}
