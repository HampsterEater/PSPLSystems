// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------
#include "PSPPlatform.h"

#ifdef PSP

// =======================================================
// SCE module information.
// =======================================================

SCE_MODULE_INFO(lsystem, 0, 1, 1);

// This defines the size of the default c-heap 
// created on the psp.
int sce_newlib_heap_kb_size = 1024 * 30;  

// =======================================================
// Platform-Specific Variables.
// =======================================================

// Rendering variables.
static char g_displayList[0x800000] __attribute__((aligned(64)));
static ScePspFMatrix4 g_matrixStack[16];

// =======================================================
// Platform-Specific Functions.
// =======================================================

// Main entry point for the platform.
int main()
{
	// Call the cross-platform entry point.
	CrossPlatformEntryPoint();

	return 0;
}

// =======================================================
// Platform-Independent Functions.
// =======================================================
double P_Sin(double x)
{
	return sceFpuSin(x);
}
double P_Cos(double x)
{
	return sceFpuCos(x);
}

void P_InitializeScreen()
{
	// Initialize graphics system.
	sceGuInit();

	// Setup display buffers.
	sceGuStart(SCEGU_IMMEDIATE, g_displayList, sizeof(g_displayList));
	sceGuDrawBuffer(SCEGU_PF5551, SCEGU_VRAM_BP_0, SCEGU_VRAM_WIDTH);
	sceGuDispBuffer(SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT, SCEGU_VRAM_BP_1, SCEGU_VRAM_WIDTH);
	sceGuDepthBuffer(SCEGU_VRAM_BP_2, SCEGU_VRAM_WIDTH);

	// Setup viewport.
	sceGuOffset(SCEGU_SCR_OFFSETX, SCEGU_SCR_OFFSETY);
	sceGuViewport(2048, 2048, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);

	// Setup scissor clipping.
	sceGuScissor(0, 0, SCEGU_SCR_WIDTH, SCEGU_SCR_HEIGHT);
	sceGuEnable(SCEGU_SCISSOR_TEST);

	// Setup depth test.
	sceGuDepthFunc(SCEGU_GEQUAL);
	sceGuEnable(SCEGU_DEPTH_TEST);

	// Enable textures and a smooth shade model.
	sceGuDisable(SCEGU_TEXTURE);
	sceGuShadeModel(SCEGU_FLAT);

	// Setup buffer clear values.
	sceGuClearDepth(0);
	
	// Initialize projection matrix.
	sceGumSetMatrixStack(g_matrixStack, 4, 4, 8, 0);
	sceGumMatrixMode(SCEGU_MATRIX_PROJECTION);
	sceGumPerspective(SCEGU_RAD(45.0f), SCEGU_SCR_ASPECT, 1.000000f, 10000.000000f);

	// Initialize world matrix.
	sceGumMatrixMode(SCEGU_MATRIX_WORLD);

	// Finish everything off.
	sceGuFinish();
	sceGuSync(SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);

	// Turn on display.
	sceGuDisplay(SCEGU_DISPLAY_ON);
}

bool P_CloseRequested()
{
	return false;
}

void P_InitializeSound()
{
	sceWaveInit();
}

void P_FlipBuffers()
{
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

void P_StartFrame()
{
	sceGuStart(SCEGU_IMMEDIATE, g_displayList, sizeof(g_displayList));
}

void P_FinishFrame()
{
	sceGuFinish();
	sceGuSync(SCEGU_SYNC_FINISH, SCEGU_SYNC_WAIT);
}

void P_ClearBuffer(int color)
{	
	sceGuClearColor(color);
	sceGuClear(SCEGU_CLEAR_ALL);
}

void P_SetColor(int color)
{
	sceGuColor(color);
}

void P_DebugPrint(int x, int y, int color, const char* txt)
{
	sceGuDebugPrint(x, y, color, txt);
}

bool P_ButtonDown(int button)
{
	SceCtrlData inputBuffer;	
	sceCtrlReadBufferPositive(&inputBuffer, 1);

	switch (button)
	{
	case BUTTON_LEFT:  return ((inputBuffer.Buttons & SCE_CTRL_LEFT) != 0); 
	case BUTTON_RIGHT: return ((inputBuffer.Buttons & SCE_CTRL_RIGHT) != 0); 
	}

	return false;
}

void P_Translate(P_Vector3* val)
{
	ScePspFVector3 trans;
	trans.x = val->x;
	trans.y = val->y;
	trans.z = val->z;

	sceGumTranslate(&trans);
}

void P_Scale(P_Vector3* val)
{
	ScePspFVector3 trans;
	trans.x = val->x;
	trans.y = val->y;
	trans.z = val->z;

	sceGumScale(&trans);
}

void P_RotateY(double val)
{
	sceGumRotateY(val);
}

void P_PushMatrix()
{
	sceGumPushMatrix();
}

void P_PopMatrix()
{
	sceGumPopMatrix();
}

void P_DrawCube()
{
	sceGumDrawArray(SCEGU_PRIM_TRIANGLES, SCEGU_VERTEX_FLOAT, 32, 0, g_cubeData);
}

void P_PlayPCM(int sampleRate, int format, int maxVolumeLeft, int maxVolumeRight, int channel, void* data, int dataSize)
{	
	int realFormat = SCE_WAVE_AUDIO_FMT_S16_MONO;
	switch (format)
	{
		case SOUND_FORMAT_MONO: realFormat = SCE_WAVE_AUDIO_FMT_S16_MONO; break;
	}

	float realVolumeLeft = SCE_WAVE_AUDIO_VOL_MAX * (maxVolumeLeft / 100.0f);
	float realVolumeRight = SCE_WAVE_AUDIO_VOL_MAX * (maxVolumeRight / 100.0f);

	sceWaveAudioSetSample(channel, sampleRate);
	sceWaveAudioSetFormat(channel, format);
	sceWaveAudioSetVolume(channel, realVolumeLeft, realVolumeRight);

	sceWaveAudioWrite(channel, realVolumeLeft, realVolumeRight, data);
}

unsigned long long P_GetMillisecs()
{
	return sceKernelGetSystemTimeWide() / 1000;
}

void P_Sleep(int val)
{
	sceKernelDelayThread(val);
}

#endif