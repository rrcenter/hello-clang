# hello-clang

## Requirements

- https://vcpkg.io/en/index.html

llvm version
```
llvm:x86-windows                                   13.0.0#3         The LLVM Compiler Infrastructure.
```

## x64
```powershell
vcpkg.exe install llvm:x64-windows
mkdir build
cd build 

# Debug build on Windows
cmake .. -DCMAKE_TOOLCHAIN_FILE="E:\dev\vcpkg\vcpkg\scripts\buildsystems\vcpkg.cmake" -DLLVM_CONFIGURATION_TYPES=Debug -DLLVM_BUILD_TYPE=Debug
    
cmake --build . --config Debug

# Release build on Windows
cmake .. -DCMAKE_TOOLCHAIN_FILE="E:\dev\vcpkg\vcpkg\scripts\buildsystems\vcpkg.cmake" -DLLVM_CONFIGURATION_TYPES=Release -DLLVM_BUILD_TYPE=Release
    
cmake --build . --config Release
```

## x86

```powershell
vcpkg.exe install llvm:x86-windows
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="E:\dev\vcpkg\vcpkg\scripts\buildsystems\vcpkg.cmake" -DLLVM_CONFIGURATION_TYPES=Debug -DLLVM_BUILD_TYPE=Debug -A Win32
cmake --build . --config Debug
```
