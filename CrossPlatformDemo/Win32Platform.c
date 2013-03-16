// ---------------------------------------------------------------
//	PSP/Win32 Cross Platform Demo
//	Assignment 4 - Console Development
//	Written By Timothy Leonard
// ---------------------------------------------------------------
#include "Win32Platform.h"

#ifdef WINDOWS

// =======================================================
// Platform-Specific Functions.
// =======================================================
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Call the cross-platform entry point.
	CrossPlatformEntryPoint();

	return 0;
}

// =======================================================
// Platform-Specific Variables.
// =======================================================
HINSTANCE				g_hInstance;
HWND					g_hWnd;
bool					g_quitPosted;

IDirect3D9*				g_d3dObject;
IDirect3DDevice9*		g_d3dDevice;
D3DPRESENT_PARAMETERS	g_presentParameters;

D3DDEVTYPE				g_deviceType;
DWORD					g_behaviourFlags;

ID3DXFont*				g_defaultFont;

LPDIRECT3DVERTEXBUFFER9	g_cubeVertexBuffer;

D3DXMATRIXA16			g_worldMatrix;
D3DXMATRIXA16			g_viewMatrix;
D3DXMATRIXA16			g_projMatrix;

D3DXVECTOR3				g_eyeVector( 0.0f, 0.0f, -5.0f );
D3DXVECTOR3				g_lookatVector( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3				g_upVector( 0.0f, 1.0f, 0.0f );

D3DXMATRIXA16			g_matrixStack[32];
int						g_matrixStackPtr = 0;

int						g_currentColor = 0xFFFFFFFF;

HWAVEOUT				g_waveOut;
WAVEFORMATEX			g_waveFormatEx;
MMRESULT				g_waveResult;
WAVEHDR					g_waveHeader;
bool					g_soundInitialized = false;

char					g_keyCodes[256];

// =======================================================
// Platform-Specific Structs.
// =======================================================
struct CUBECUSTOMVERTEX
{
    float x,y,z;
    DWORD color;
};
#define CUSTOMCUBEVERTEXFORMAT D3DFVF_XYZ|D3DFVF_DIFFUSE

// =======================================================
// Platform-Independent Functions.
// =======================================================
double P_Sin(double x)
{
	return sin(x);
}
double P_Cos(double x)
{
	return cos(x);
}

void P_InitializeScreen()
{
	WNDCLASS wc;
	RECT rect = { 0, 0, 480, 272 };
	void* vertices = NULL;

	g_quitPosted = false;

	// Register the window class.
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)MsgProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance     = g_hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";

	if (!RegisterClass(&wc))
		return;

	// Create the actual window.
	AdjustWindowRect(&rect, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU), false);
	g_hWnd = CreateWindow("D3DWndClassName", 
							"Cross-Platform Demo Project", 
							(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU),
							100, 100,
							rect.right - rect.left, rect.bottom - rect.top,
							0, 0, g_hInstance, 0);
	if (!g_hWnd)
		return;

	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

	// Initialize direct3d parameters.
	g_behaviourFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;
	g_deviceType = D3DDEVTYPE_HAL;

	g_presentParameters.BackBufferWidth            = 0; 
	g_presentParameters.BackBufferHeight           = 0;
	g_presentParameters.BackBufferFormat           = D3DFMT_UNKNOWN;
	g_presentParameters.BackBufferCount            = 1;
	g_presentParameters.MultiSampleType            = D3DMULTISAMPLE_NONE;
	g_presentParameters.MultiSampleQuality         = 0;
	g_presentParameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	g_presentParameters.hDeviceWindow              = g_hWnd;
	g_presentParameters.Windowed                   = true;
	g_presentParameters.EnableAutoDepthStencil     = true; 
	g_presentParameters.AutoDepthStencilFormat     = D3DFMT_D24S8;
	g_presentParameters.Flags                      = 0;
	g_presentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	g_presentParameters.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create Direct3D device.
	g_d3dObject = Direct3DCreate9(D3D_SDK_VERSION);
	g_d3dObject->CreateDevice(D3DADAPTER_DEFAULT, g_deviceType, g_hWnd, g_behaviourFlags, &g_presentParameters, &g_d3dDevice);

	// Create our drawing font.
	// Height should be set to 8 to be exactly like the psp - but looks squashed on widescreen
	// displays :(.
	D3DXCreateFont(g_d3dDevice, 11, 8, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Lucida Console"), &g_defaultFont);

	// Create our cube vertex buffer.
	g_d3dDevice->CreateVertexBuffer(32 * sizeof(CUBECUSTOMVERTEX), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, CUSTOMCUBEVERTEXFORMAT, D3DPOOL_DEFAULT, &g_cubeVertexBuffer, NULL);

	// Fill vertex buffer.
	g_cubeVertexBuffer->Lock(0, 0, &vertices, 0);

	CUBECUSTOMVERTEX verts[32];
	for (int i = 0; i < 32; i++)
	{
		verts[i].x = g_cubeData[i].x;
		verts[i].y = g_cubeData[i].y;
		verts[i].z = g_cubeData[i].z;
		verts[i].color = 0xFFFFFFFF;
	}
	memcpy(vertices, verts, 32 * sizeof(CUBECUSTOMVERTEX));

	g_cubeVertexBuffer->Unlock();
}

void P_DrawCube()
{
	// Render cube.
	g_d3dDevice->SetStreamSource(0, g_cubeVertexBuffer, 0, sizeof(CUBECUSTOMVERTEX));
    g_d3dDevice->SetFVF(CUSTOMCUBEVERTEXFORMAT);
    g_d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 32 / 3);
}

void SetupMatrices()
{
	// Setup world matrix.
	D3DXMatrixIdentity(&g_worldMatrix);
	g_d3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);

	// Setup view matrix.
	D3DXMatrixLookAtLH(&g_viewMatrix, &g_eyeVector, &g_lookatVector, &g_upVector);

	// Change from right-hand to left-hand coordinates.
	g_viewMatrix._31 = -g_viewMatrix._31;
	g_viewMatrix._32 = -g_viewMatrix._32;
	g_viewMatrix._33 = -g_viewMatrix._33;
	g_viewMatrix._34 = -g_viewMatrix._34;

	g_d3dDevice->SetTransform(D3DTS_VIEW, &g_viewMatrix);

	// Setup projection matrix.
	float aspect = 480.0f / 272.0f;
	D3DXMatrixPerspectiveFovLH(&g_projMatrix, DegToRad(45.0f), aspect, 1.0f, 10000.0f);
	g_d3dDevice->SetTransform(D3DTS_PROJECTION, &g_projMatrix);
}

void P_StartFrame()
{	
	g_d3dDevice->SetRenderState(D3DRS_ZENABLE, true);
	g_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);

	g_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	g_d3dDevice->BeginScene();

	SetupMatrices();
}

void P_FinishFrame()
{
	g_d3dDevice->EndScene();
}

void P_FlipBuffers()
{
	g_d3dDevice->Present(NULL, NULL, NULL, NULL);
	
	MSG msg;
	msg.message = WM_NULL;

	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) && !g_quitPosted)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		g_quitPosted = true;
		return 0;

	case WM_KEYDOWN:
		g_keyCodes[wParam] = true;
		break;

	case WM_KEYUP:
		g_keyCodes[wParam] = false;
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool P_CloseRequested()
{
	return g_quitPosted;
}

void P_InitializeSound()
{
	// Done in PlayPCM.
}

void P_ClearBuffer(int color)
{	
	// Flip color bits around - directx uses a different format to psp.
	D3DCOLOR realColor = D3DCOLOR_XRGB(GetB(color), GetG(color), GetR(color));

	// Clear screen.
	g_d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, realColor, 1.0f, 0);
}

void P_SetColor(int color)
{
	// Flip color bits around - directx uses a different format to psp.
	D3DCOLOR realColor = D3DCOLOR_XRGB(GetB(color), GetG(color), GetR(color));

	g_currentColor = realColor;

	g_d3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, g_currentColor);
	g_d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
	g_d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	g_cubeVertexBuffer->Unlock();
}

void P_DebugPrint(int x, int y, int color, const char* txt)
{
	// Flip color bits around - directx uses a different format to psp.
	D3DCOLOR realColor = D3DCOLOR_XRGB(GetB(color), GetG(color), GetR(color));

	// Work out a rect to draw text to.
	RECT rect;
	rect.left = x;
	rect.right = x + 2000;
	rect.top = y;
	rect.bottom = y + 20;
 
	// Draw the text.
	g_defaultFont->DrawText(NULL, txt, -1, &rect, 0, realColor);
}

bool P_ButtonDown(int button)
{
	switch (button)
	{
		case BUTTON_LEFT:	return g_keyCodes[VK_LEFT];	
		case BUTTON_RIGHT:	return g_keyCodes[VK_RIGHT];	
	}
	return false;
}

void P_Translate(P_Vector3* val)
{
	D3DXMATRIXA16 translationMat;
	D3DXMatrixTranslation(&translationMat, val->x, val->y, val->z);
	D3DXMatrixMultiply(&g_worldMatrix, &translationMat, &g_worldMatrix);
	g_d3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
}

void P_Scale(P_Vector3* val)
{
	D3DXMATRIXA16 translationMat;
	D3DXMatrixScaling(&translationMat, val->x, val->y, val->z);
	D3DXMatrixMultiply(&g_worldMatrix, &translationMat, &g_worldMatrix);
	g_d3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
}

void P_RotateY(double val)
{
	D3DXMATRIXA16 translationMat;
	D3DXMatrixRotationY(&translationMat, val);
	D3DXMatrixMultiply(&g_worldMatrix, &translationMat, &g_worldMatrix);
	g_d3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
}

void P_PushMatrix()
{
	g_matrixStack[g_matrixStackPtr] = g_worldMatrix;
	g_matrixStackPtr++;
}

void P_PopMatrix()
{
	g_worldMatrix = g_matrixStack[--g_matrixStackPtr];
	g_d3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
}

void P_PlayPCM(int sampleRate, int format, int maxVolumeLeft, int maxVolumeRight, int channel, void* data, int dataSize)
{	
	if (g_soundInitialized == false)
	{
		// Fill wave format information.
		g_waveFormatEx.nSamplesPerSec = 44100 * 2; 
		g_waveFormatEx.wBitsPerSample = 16; 
		g_waveFormatEx.nChannels = 1; 
		g_waveFormatEx.cbSize = 0;
		g_waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
		g_waveFormatEx.nBlockAlign = (g_waveFormatEx.wBitsPerSample >> 3) * g_waveFormatEx.nChannels;
		g_waveFormatEx.nAvgBytesPerSec = g_waveFormatEx.nBlockAlign * g_waveFormatEx.nSamplesPerSec;

		// Open the wave-out device.
		waveOutOpen(&g_waveOut, WAVE_MAPPER, &g_waveFormatEx, 0, 0, CALLBACK_NULL);
	
		g_soundInitialized = true;
	}

	// Initialize wave header.
	ZeroMemory(&g_waveHeader, sizeof(WAVEHDR));
	g_waveHeader.dwBufferLength = dataSize;
	g_waveHeader.lpData = (char*)data;

	// Prepare for playback.
	waveOutPrepareHeader(g_waveOut, &g_waveHeader, sizeof(WAVEHDR));

	// Write the wave to the device.
	waveOutWrite(g_waveOut, &g_waveHeader, sizeof(WAVEHDR));

	//Sleep(500);
	//while(waveOutUnprepareHeader(g_waveOut, &g_waveHeader, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING)
	//	Sleep(100);
}

unsigned long long P_GetMillisecs()
{
	return GetTickCount();
}

void P_Sleep(int val)
{
	Sleep(val);
}


#endif