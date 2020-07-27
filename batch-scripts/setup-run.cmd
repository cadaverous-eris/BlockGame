
@ECHO OFF

IF NOT EXIST "run" (
    MKDIR "run"
)

IF EXIST "run\res" (
	RMDIR "run\res" /S /Q
)

IF EXIST "res" (
	MKLINK /D "run\res" "..\res"
)