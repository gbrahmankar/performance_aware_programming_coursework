@echo off
setlocal enabledelayedexpansion

set /p purge_answer=Do you wish to purge the currently built project? (Y/N) :
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

set /p vs_or_rad=Do you wish to open a visual_studo_solution or a raddebugger attached process? (vs/rad) :

set cl_args=""
set /p part_id="What part do you wish to run ? (1/2/3) :"
if "%part_id%"=="1" (
    set cl_args=part_one ../part_one_bins/add_sub_cmp_jnz.bin
) else if "%part_id%"=="2" (
    set /p gen_par="1 : generate | 2 : parse ? :"
    if "!gen_par!"=="1" (
	if not exist "part_two_generated_files\" (
		mkdir "part_two_generated_files"
		if errorlevel 1 echo Failed to create .\part_two_generated_files\ !
	)
    	set cl_args=part_two generator uniform 2324568 1000000
    ) else if "!gen_par!"=="2" (
    	set cl_args=part_two parser ..\\part_two_generated_files\\haversine_input.json
    ) else (
        echo "Invalid haversine operation=" !gen_par!
        exit /b 1
    )
) else if "%part_id%"=="3" (
    set cl_args=part_three
) else (
    echo Invalid part chosen=%part_id%
    exit /b 1
)

if /i "%vs_or_rad%"=="vs" (
        echo Building pap_cpp and starting its visual_studio solution ...

        if not exist "build_vs" (
                mkdir build_vs
                if errorlevel 1 echo Failed to create build_vs !
        )
        cd build_vs

        cmake -DCL_ARGS="!cl_args!" -G "Visual Studio 17 2022" ..
        start "" /wait cmd /c "cmake --build . --target run_vs_sln & exit"
)

if /i "%vs_or_rad%"=="rad" (
        echo Building pap_cpp and starting it attached to raddebugger ...

        if not exist "build_rad" (
                mkdir build_rad
                if errorlevel 1 echo Failed to create build_rad !
        )
        cd build_rad

        cmake -DCL_ARGS="!cl_args!" -G Ninja ..
        start "" /wait cmd /c "cmake --build . --target run_raddbg & exit"
)

endlocal
