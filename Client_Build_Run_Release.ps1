Set-Location -Path "./Client"
premake vs2022
premake export-compile-commands
msbuild /p:Configuration=Release
Set-Location -Path "../"
/Client/bin/Release/Client.exe
