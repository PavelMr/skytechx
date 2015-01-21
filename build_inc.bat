@echo off 
set /p var= < ../../skytech_source/build.txt 
set /a var= %var%+1 
echo %var% > ../../skytech_source/build.txt
echo #ifndef BUILD_INC_H > ../../skytech_source/build_inc.h
echo #define BUILD_INC_H >> ../../skytech_source/build_inc.h
echo #define DEF_BUILD_NO %var% >> ../../skytech_source/build_inc.h
echo #endif >> ../../skytech_source/build_inc.h

echo %var%
