@echo on
set out=%1
set sdl=%2
set sdlimg=%3
(robocopy /S /E %sdl%    %out% *.dll		) ^& IF %ERRORLEVEL% GTR 7 exit 1
(robocopy /S /E %sdlimg% %out% *.dll   		) ^& IF %ERRORLEVEL% GTR 7 exit 1