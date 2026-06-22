@echo off
setlocal enabledelayedexpansion

:: Путь к программе (в папке Executable, которая на уровень выше)
set "program=%~dp0..\Executable\calculatingOptimalCoins.exe"
set "test_dir=%~dp0Input"
set "output_dir=%~dp0Output"
set "result_file=%~dp0test_results.txt"

if not exist "%program%" (
    echo ERROR: File %program% not found!
    echo.
    echo Make sure that:
    echo 1. calculatingOptimalCoins.exe is in the Executable folder
    echo 2. All required DLL files are also in the Executable folder
    echo.
    echo Current folder: %~dp0
    echo.
    echo Executable folder contents:
    dir "%~dp0..\Executable" /b 2>nul
    pause
    exit /b 1
)

mkdir "%output_dir%" 2>nul
if exist "%result_file%" del "%result_file%"

echo ======================================== >> "%result_file%"
echo TESTS FOR calculatingOptimalCoins        >> "%result_file%"
echo ======================================== >> "%result_file%"
echo. >> "%result_file%"

echo Starting tests...
echo.

for /L %%n in (1,1,40) do (
    set "num=0%%n"
    if %%n geq 10 set "num=%%n"
    
    echo [TEST !num!] Running test... >> "%result_file%"
    echo. >> "%result_file%"
    
    echo Input data: >> "%result_file%"
    type "%test_dir%\input!num!.txt" >> "%result_file%"
    echo. >> "%result_file%"
    
    echo Result: >> "%result_file%"

    "%program%" "%test_dir%\input!num!.txt" "%output_dir%\output!num!.txt" >nul 2>nul
    type "%output_dir%\output!num!.txt" >> "%result_file%" 2>nul
    echo. >> "%result_file%"
    
    echo ---------------------------- >> "%result_file%"
    echo. >> "%result_file%"
    
    echo [TEST !num!] Done
)

echo.
echo ========================================
echo Tests completed. Results saved to test_results.txt
echo ========================================
pause