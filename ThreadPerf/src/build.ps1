cd ThreadPerfCore
dotnet restore
cd ..

msbuild /t:rebuild /p:Configuration=Release
