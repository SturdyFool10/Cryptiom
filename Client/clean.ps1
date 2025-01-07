# Clean directories and files
Remove-Item -Recurse -Force -Path ".\bin" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force -Path ".\bin-int" -ErrorAction SilentlyContinue  
Remove-Item -Force -Path ".\Client.sln" -ErrorAction SilentlyContinue
Remove-Item -Force -Path ".\compile_commands.json" -ErrorAction SilentlyContinue