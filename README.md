# HandmadeHero

This project follows along with the YouTube series of Casey Muratori of Molly Rocket building and explaining Handmade Hero code. The first of such videos can be found at https://www.youtube.com/watch?v=Ee3EtYb8d1o&pp=ygURaGFuZG1hZGUgaGVybyAwMDE%3D

## Build Instructions
### CL.exe
cl.exe /Zi /FC win32_handmade.cpp user32.lib gdi32.lib

### Visual Studio
Configure Visual Studio Project linker (Properties > Linker > Input > Additional Dependencies) and add User32.lib and Gdi32.lib. Will also need to add path to these files to Linker > General > Additional Library Directories. If you have the Windows SDK installed on your system (needed for this project and available through Visual Studio installer), the path should be similar to C:\Program Files (x86)\Windows Kits\10\Lib\{Version Number}\um\x64. YMMV.

This should be all that's needed to successfully build and run the executable. 
