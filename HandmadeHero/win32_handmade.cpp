/* Handmade Hero - Win32 Implementation */

#include <windows.h>
#include <stdint.h>

/************************
        Defines
************************/

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
internal void Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight,
    win32_offscreen_buffer Buffer) {
    StretchDIBits(
        DeviceContext,
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer.Width, Buffer.Height,
        Buffer.Memory,
        &Buffer.Info,
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
    case WM_PAINT: {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        int X = Paint.rcPaint.left;
        int Y = Paint.rcPaint.top;
        int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
        int Width = Paint.rcPaint.right - Paint.rcPaint.left;

        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer);

        EndPaint(Window, &Paint);
    }
                 break;
    default: {
        // OutputDebugStringA("DEFAULT\n")
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
    WNDCLASS WindowClass = {};

    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon;
    WindowClass.lpszClassName = (LPCWSTR)"HandmadeHeroWindowClass";

    if (RegisterClass(&WindowClass)) {
        HWND Window = CreateWindowEx(
            0,
            WindowClass.lpszClassName,
            (LPCWSTR)"Handmade Hero",
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
                Win32RenderWeirdGradient(&GlobalBackBuffer, XOffset, YOffset);
                HDC DeviceContext = GetDC(Window);
                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height,
                    GlobalBackBuffer);
                ReleaseDC(Window, DeviceContext);
                ++XOffset;
                --YOffset;
            }
        }
        else {
            //TODO: Logging
        }
    }
    else {
        //TODO: Logging
    }

    return (0);
}
