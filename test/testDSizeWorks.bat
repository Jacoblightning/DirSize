cd %temp%
set /a dirname=%RANDOM%
mkdir %dirname%
cd %dirname%

echo 5char > file.txt

FOR /F "tokens=*" %%g IN ('%1 -m .') do (set output=%%g)

echo "Got output: %output%"

FOR /F "tokens=2 delims=:" %%i in ('%output%') do set len=%%i

if "%len%" == "6" (goto exitsuccess)

exit /b 1

:exitsuccess
exit /b 0