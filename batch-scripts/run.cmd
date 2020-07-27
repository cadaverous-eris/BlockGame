
@ECHO OFF

IF EXIST "build" (
	.\setup-run.cmd
	cd run
	..\build\game
	cd ..
) ELSE (
	ECHO "Build directory does not exist - you must build the project before running"
)
