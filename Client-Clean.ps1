Remove-Item -Path "Client/Bin" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path "Client/bin-int" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path "Client/build" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path "Client/compile_commands" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item -Path "Client/Client.sln" -Force -ErrorAction SilentlyContinue
Remove-Item -Path "Client/compile_commands.json" -Force -ErrorAction SilentlyContinue
