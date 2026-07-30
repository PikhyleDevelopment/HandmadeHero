/* Handmade Hero - Win32 Implementation */

#include <windows.h>
#include <stdint.h>
#include <Xinput.h>

#define internal static
#define local_persist static
#define global_variable static


/************************
		Structs
************************/

struct win32_offscreen_buffer {
	// Pixels are always 32-bits wide, Memory Order: BB GG RR XX
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
};

struct win32_window_dimension {
	int Width;
	int Height;
};

/************************
		Globals
************************/

global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;

/************************
	  Helper Funcs
************************/

// What is this tomfoolery?!
// XInputGetState and SetState Support without linking to XInput O_o.

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_GET_STATE(x_input_get_state);
typedef X_INPUT_SET_STATE(x_input_set_state);

X_INPUT_GET_STATE(XInputGetStateStub) {
	return (0);
}

X_INPUT_SET_STATE(XInputSetStateStub) {
	return (0);
}

global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

internal void Win32LoadXInput(void) {
	HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");

	if (XInputLibrary) {
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
	}
}

internal win32_window_dimension Win32GetWindowDimension(HWND Window) {
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return Result;
}

/************************
		 Funcs
************************/

/**
 * @brief This function renders a weird gradient to the window. For Testing Purposes Only.
 * @param Buffer Buffer Pointer
 * @param XOffset The amount to offset each pixel on the X axis
 * @param YOffset The amount to offset each pixel on the Y axis
 */
internal void Win32RenderWeirdGradient(win32_offscreen_buffer* Buffer, int XOffset, int YOffset) {
	uint8_t* Row = (uint8_t*)Buffer->Memory;
	for (int Y = 0; Y < Buffer->Height; ++Y) {
		uint32_t* Pixel = (uint32_t*)Row;
		for (int X = 0; X < Buffer->Width; ++X) {
			uint8_t Blue = ((X * 2) + XOffset);
			uint8_t Green = ((Y * 2) + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}
		Row += Buffer->Pitch;
	}
}

/**
 * Allocate memory for our Buffer
 * @param Buffer Buffer Pointer
 * @param XOffset The amount to offset each pixel on the X axis
 * @param YOffset The amount to offset each pixel on the Y axis
 */
internal void Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height) {
	if (Buffer->Memory) {
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	int BytesPerPixel = 4;

	/*
	 *  When the biHeight field is negative, it tells Windows
	 *  to treat this bitmap as top-down, not bottom-up, meaning
	 *  the first three bytes of the image are the color for the top left pixel
	 *  in the bitmap, not the bottom left.
	 */
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width * Buffer->Height) * BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	Buffer->Pitch = Width * BytesPerPixel;
}

/**
 * @brief This function displays the buffer in the window
 * @param DeviceContext The Device Context
 * @param WindowWidth Width of the window
 * @param WindowHeight Height of the window
 * @param Buffer Our lovely buffer
 */
internal void Win32DisplayBufferInWindow(win32_offscreen_buffer* Buffer, HDC DeviceContext, 
		int WindowWidth, int WindowHeight) {
	StretchDIBits(
		DeviceContext,
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS, SRCCOPY
	);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
	LRESULT Result = 0;

	switch (Message) {
		case WM_SIZE: {
		}
		break;
		case WM_DESTROY: {
			GlobalRunning = false;
		}
		break;
		case WM_CLOSE: {
			GlobalRunning = false;
		}
		break;
		case WM_ACTIVATEAPP: {
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}
		break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP: {
			uint32_t VKCode = WParam;
			bool WasDown = ((LParam & (1 << 30)) != 0);
			bool IsDown = ((LParam & (1 << 31)) == 0);
			if (WasDown != IsDown) {
				if (VKCode == 'W') {

				}
				else if (VKCode == 'A') {

				}
				else if (VKCode == 'S') {

				}
				else if (VKCode == 'D') {

				}
				else if (VKCode == 'Q') {

				}
				else if (VKCode == 'E') {

				}
				else if (VKCode == VK_UP) {

				}
				else if (VKCode == VK_LEFT) {

				}
				else if (VKCode == VK_DOWN) {

				}
				else if (VKCode == VK_RIGHT) {

				}
				else if (VKCode == VK_ESCAPE) {
					if (IsDown) {
						OutputDebugStringA("Escape Key Is Down\n");
					}
					if (WasDown) {
						OutputDebugStringA("Escape Key WAS Down\n");
					}
				}
				else if (VKCode == VK_SPACE) {

				}
			}
		}
		break;
		case WM_PAINT: {
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);

			EndPaint(Window, &Paint);
		}
		break;
		default: {
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}
		break;
	}

	return Result;
}

int CALLBACK WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode
) {

	Win32LoadXInput();

	WNDCLASSA WindowClass = {};

	Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	// WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	// For some reason, the window title is in a foreign language...
	if (RegisterClassA(&WindowClass)) {
		HWND Window = CreateWindowExA(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0
		);

		if (Window) {
			int XOffset = 0;
			int YOffset = 0;
			GlobalRunning = true;
			while (GlobalRunning) {
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
					if (Message.message == WM_QUIT) {
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}

				for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex) {
					XINPUT_STATE ControllerState;
					if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS) {
						// The controller is plugged in
						XINPUT_GAMEPAD* Pad = &ControllerState.Gamepad;
						bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						int16_t StickX = Pad->sThumbLX;
						int16_t StickY = Pad->sThumbLY;

						XOffset += StickX >> 12;
						YOffset -= StickY >> 12;
					}
					else {
						// The controller is not available
					}
				}

				XINPUT_VIBRATION Vibration;
				//Vibration.wLeftMotorSpeed = 60000;
				//Vibration.wRightMotorSpeed = 60000;
				//XInputSetState(0, &Vibration);

				Win32RenderWeirdGradient(&GlobalBackBuffer, XOffset, YOffset);
				HDC DeviceContext = GetDC(Window);
				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
				ReleaseDC(Window, DeviceContext);
				//++XOffset;
				//YOffset += 2;
			}
		}
		else {
			//TODO: Logging
			return (-1);
		}
	}
	else {
		//TODO: Logging
		return (-2);
	}

	return (0);
}
