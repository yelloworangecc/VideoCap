md temp
copy 3rdDLL\* temp\
copy build\Release\VideoCap.exe temp\
cd .\7z
7z a ..\temp.7z ..\temp\*
copy /b 7zsd_All_x64.sfx + config.txt + ..\temp.7z ..\videocap_installer.exe
del /Q ..\temp\*
rd ..\temp
del ..\temp.7z
pause
