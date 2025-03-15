cd %temp%
set /a dirname=%RANDOM%
mkdir %dirname%
cd %dirname%

echo 5char > file.txt

FOR /F "tokens=*" %%g IN ('%1 -m .') do (set output=%%g)

echo "Got output: %output%"

REM Copy to temporary variable for processing.
set "str=%output%"

:loop
REM Check if the string contains a colon.
echo %str% | findstr ":" >nul
if errorlevel 1 (
REM No colon found, so this is the last token.
set "len=%str%"
goto :done
)

REM Otherwise, split the string at the first colon.
for /f "tokens=1* delims=:" %%A in ("%str%") do (
REM %%A is the first token and %%B is the rest of the string.
set "str=%%B"
)
goto :loop

:done

REM Echo apparently adds a space AND a newline after the 5char which ups the total bytes to 8.
if "%len%" == "8" (goto exitsuccess)

exit /b 1

:exitsuccess
exit /b 0