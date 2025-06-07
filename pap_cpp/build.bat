@echo off

setlocal

set /p purge_answer=Do you wish to purge the currently built project? (Y/N):
if /i "%purge_answer%"=="Y" (
        echo Purging build and other directories ...
        rmdir /s /q build_vs build_rad
        rmdir /s /q ..\..\other\raddebugger
)

if not exist "..\..\other" (
        mkdir ..\..\other
        if errorlevel 1 echo Failed to create ..\..\other !
)

if not exist "..\..\other\raddebugger\" (
        echo Cloning and building raddebugger ...
        start /wait git clone "https://github.com/EpicGamesExt/raddebugger.git" "..\..\other\raddebugger"
        start "" /wait cmd /c "..\..\other\raddebugger\build"
)

if not exist "..\..\other\raddebugger\build\" (
        echo Building raddebugger ...
        start "" /wait cmd /c "..\..\other\raddebugger\build"
)

set /p vs_or_rad=Do you wish to open a visual_studo_solution or a raddebugger attached process? (vs/rad):
if /i "%vs_or_rad%"=="vs" (
        echo Building pap_cpp and starting its visual_studio solution ...

        if not exist "build_vs" (
                mkdir build_vs
                if errorlevel 1 echo Failed to create build_vs !
        )
        cd build_vs

        cmake -G "Visual Studio 17 2022" ..
        start "" /wait cmd /c "cmake --build . --target run_vs_sln & exit"
)

if /i "%vs_or_rad%"=="rad" (
        echo Building pap_cpp and starting it attached to raddebugger ...

        if not exist "build_rad" (
                mkdir build_rad
                if errorlevel 1 echo Failed to create build_rad !
        )
        cd build_rad

        cmake -G Ninja ..
        start "" /wait cmd /c "cmake --build . --target run_raddbg & exit"
)

endlocal
