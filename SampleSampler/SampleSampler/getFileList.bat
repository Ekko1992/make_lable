@echo on > imgfiles.txt
for /r E:\Databases\Baidu\baidu03 %%i in (*.jpg) do echo %%i >> imgfiles.txt
pause