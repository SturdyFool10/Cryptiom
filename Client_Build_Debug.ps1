Set-Location -Path "./Client"
premake vs2022
premake export-compile-commands
msbuild /p:Configuration=Debug
Set-Location -Path "../"