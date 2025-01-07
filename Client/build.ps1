premake vs2022
msbuild Client.sln -p:Configuration=Debug -m
msbuild Client.sln -p:Configuration=Release -m
cp ./compile_commands/Release.json ./compile_commands.json -Force
