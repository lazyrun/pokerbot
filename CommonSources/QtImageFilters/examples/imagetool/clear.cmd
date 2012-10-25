echo off
echo η¨αβª  src
del /F /Q *.vcproj
del /F /Q *.user
del /F /Q /A:H *.suo
del /F /Q *.suo
del /F /Q *.sln
del /F /Q *.ncb
del /F /Q *.idb
del /F /Q *.pdb
del /F /Q bin
rd /S /Q debug
rd /S /Q release

