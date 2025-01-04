Write-Host "Building Client Debug..."
./Client_Build_Debug.ps1
Write-Host "Client Debug build complete.`n"

Write-Host "Building Client Release..."
./Client_Build_Release.ps1
Write-Host "Client Release build complete.`n"

Write-Host "Building Server Debug..."
./Server_Build_Debug.ps1
Write-Host "Server Debug build complete.`n"

Write-Host "Building Server Release..."
./Server_Build_Release.ps1
Write-Host "Server Release build complete.`n"

Write-Host "Copying clangd config for smoother editing...`n"

cp ./Client/compile_commands/Debug.json ./Client/compile_commands.json
