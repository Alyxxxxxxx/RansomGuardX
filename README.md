# RansomGuardX

**RansomGuardX** is a ransomware detection tool designed to monitor a specified directory for suspicious activities. It uses multiple detection strategies (file extension analysis, file modification monitoring, and entropy-based detection) to identify potential ransomware behavior and logs events for user review. The application features a modern graphical user interface (GUI) built with SDL2, ImGui, and OpenGL, allowing users to configure detection parameters, start/stop scans, and view real-time logs.

This README provides detailed instructions for installing dependencies, setting up the project, compiling the code, and running RansomGuardX on a Windows system using MSYS2 and vcpkg.

![Image](https://github.com/user-attachments/assets/a3856f44-6e74-4dd2-a3ce-e8aafca2e09b)

## Table of Contents
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
  - [Step 1: Install MSYS2](#step-1-install-msys2)
  - [Step 2: Install vcpkg](#step-2-install-vcpkg)
  - [Step 3: Install Dependencies via vcpkg](#step-3-install-dependencies-via-vcpkg)
  - [Step 4: Install MSYS2 Compiler Tools](#step-4-install-msys2-compiler-tools)
- [Project Setup](#project-setup)
  - [Directory Structure](#directory-structure)
  - [Configuring suspicious_extensions.txt](#configuring-file-of-suspicious-extensions)
  - - [Setting Up VS Code (Optional)](#setting-up-vs-code-optional)
- [Compilation](#compilation)
- [Running RansomGuardX](#running-ransomguardx)
- [Usage](#usage)
  - [GUI Overview](#gui-overview)
  - [Configuring Detection Parameters](#configuring-detection-parameters)
  - [Viewing Logs](#viewing-logs)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## Features
- **Multiple Detection Strategies**:
  - **File Extension Detection**: Identifies files with suspicious extensions listed in `Resources/suspicious_extensions.txt`.
  - **File Modification Detection**: Monitors rapid file modifications within a specified time window.
  - **Entropy Detection**: Detects high-entropy files indicative of encryption, with a dynamic threshold.
- **Modern GUI**:
  - Built with SDL2, ImGui, and OpenGL.
  - Full-screen control panel with directory selection, detection settings, scan controls, and event logs.
  - Professional dark theme with rounded corners and clear visual hierarchy.
- **Real-Time Monitoring**:
  - Start/stop scanning with a dedicated thread.
  - Automatic log updates in `ransomware_log.txt` with timestamps.
- **Configurable Parameters**:
  - Adjust suspicious extension file path, modification thresholds, and entropy settings via the GUI.
- **System Actions**:
  - Logs detected threats and can trigger system actions (e.g., network disable, shutdown) when ransomware is detected (configurable in code).

## Prerequisites
- **Operating System**: Windows 10 or later (64-bit).
- **Hardware**:
  - CPU: Any modern processor (e.g., Intel i3 or equivalent).
  - RAM: At least 4 GB.
  - GPU: OpenGL 3.0 compatible graphics driver.
- **Software**:
  - [MSYS2](https://www.msys2.org/) for the MinGW-w64 compiler and build tools.
  - [vcpkg](https://github.com/microsoft/vcpkg) for dependency management.
  - [Git](https://git-scm.com/) for cloning the repository (optional).
- **Disk Space**: Approximately 2 GB for MSYS2, vcpkg, and dependencies.

## Installation

### Step 1: Install MSYS2
MSYS2 provides the MinGW-w64 toolchain for compiling RansomGuardX.

1. **Download MSYS2**:
   - Visit [msys2.org](https://www.msys2.org/).
   - Download the latest installer (e.g., `msys2-x86_64-latest.exe`).

2. **Install MSYS2**:
   - Run the installer and follow the prompts.
   - Default installation path: `C:\msys64` (recommended).

3. **Update MSYS2**:
   - Open the MSYS2 MSYS terminal (search for "MSYS2 MSYS" in the Start menu or run `C:\msys64\msys2.exe`).
   - Update the package database and core packages:
     ```bash
     pacman -Syu
     ```
   - Close the terminal, reopen it, and run the update again to ensure all packages are updated:
     ```bash
     pacman -Syu
     ```

### Step 2: Install vcpkg
vcpkg is used to install SDL2 and ImGui dependencies.

1. **Clone vcpkg**:
   - Open the MSYS2 MSYS terminal.
   - Navigate to a directory where you want to install vcpkg (e.g., `C:\Users\YourUsername`):
     ```bash
     cd /c/Users/YourUsername
     git clone https://github.com/microsoft/vcpkg.git
     ```

2. **Bootstrap vcpkg**:
   - Navigate to the vcpkg directory:
     ```bash
     cd vcpkg
     ```
   - Run the bootstrap script:
     ```bash
     ./bootstrap-vcpkg.bat
     ```

3. **Integrate vcpkg**:
   - Integrate vcpkg with your system to simplify dependency inclusion:
     ```bash
     ./vcpkg integrate user
     ```
   - This sets up vcpkg to work with compilers without manually specifying include/library paths.

### Step 3: Install Dependencies via vcpkg
RansomGuardX requires SDL2 and ImGui, compiled for the `x64-mingw-static` triplet.

1. **Set the Triplet**:
   - Ensure vcpkg uses the `x64-mingw-static` triplet for static linking:
     ```bash
     export VCPKG_DEFAULT_TRIPLET=x64-mingw-static
     ```

2. **Install SDL2**:
   - In the vcpkg directory, install SDL2:
     ```bash
     ./vcpkg install sdl2:x64-mingw-static
     ```

3. **Install ImGui**:
   - ImGui is not directly available in vcpkg’s default ports, but you can use the source files included in the project (see [Project Setup](#project-setup)).
   - Ensure the ImGui source files (`imgui/*.cpp`, `imgui/*.h`) are in the project’s `imgui/` directory.

4. **Verify Installation**:
   - Check that SDL2 libraries are installed in:
     ```
     C:\Users\YourUsername\vcpkg\installed\x64-mingw-static\lib
     ```
     Look for `libSDL2.a`.
   - Check include files in:
     ```
     C:\Users\YourUsername\vcpkg\installed\x64-mingw-static\include\SDL2
     ```

### Step 4: Install MSYS2 Compiler Tools
Install the MinGW-w64 GCC compiler and related tools.

1. **Open MSYS2 MinGW64 Terminal**:
   - Run `C:\msys64\mingw64.exe` or search for "MSYS2 MinGW 64-bit" in the Start menu.

2. **Install GCC and Tools**:
   - Install the necessary packages:
     ```bash
     pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-toolchain
     ```

3. **Verify GCC Installation**:
   - Check the GCC version:
     ```bash
     g++ --version
     ```
     Expected output (example):
     ```
     g++ (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 13.1.0
     ```

## Project Setup

### Directory Structure
Clone or create the RansomGuardX project directory with the following structure:

```
RansomGuardX/
├── Detection/
│   ├── RansomGuardX.h
│   ├── RansomGuardX.cpp
├── Strategies/
│   ├── DetectionStrategy.h
│   ├── EntropyDetectionStrategy.h
│   ├── EntropyDetectionStrategy.cpp
│   ├── FileExtensionDetectionStrategy.h
│   ├── FileExtensionDetectionStrategy.cpp
│   ├── FileModificationDetectionStrategy.h
│   ├── FileModificationDetectionStrategy.cpp
├── Utilities/
│   ├── EventLogger.h
│   ├── EventLogger.cpp
│   ├── SystemActions.h
│   ├── SystemActions.cpp
├── imgui/
│   ├── imgui.h
│   ├── imgui.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui_tables.cpp
│   ├── imgui_impl_sdl2.h
│   ├── imgui_impl_sdl2.cpp
│   ├── imgui_impl_opengl3.h
│   ├── imgui_impl_opengl3.cpp
├── Resources/
│   ├── suspicious_extensions.txt
├── main.cpp
├── ransomware_log.txt (generated at runtime)
```

1. **Create the Project Directory**:
   - Create a directory, e.g., `C:\Users\YourUsername\Documents\Programming\C++\RansomGuardX`.
   - Copy or create the source files as listed above, ensuring the directory structure matches.

2. **Obtain ImGui Source Files**:
   - Download ImGui from [GitHub](https://github.com/ocornut/imgui) (e.g., version 1.90 or later).
   - Extract and copy the following files to the `imgui/` directory:
     - `imgui.h`, `imgui.cpp`, `imgui_draw.cpp`, `imgui_widgets.cpp`, `imgui_tables.cpp`
     - `imgui_impl_sdl2.h`, `imgui_impl_sdl2.cpp`
     - `imgui_impl_opengl3.h`, `imgui_impl_opengl3.cpp`

3. **Set Up Source Files**:
   - Copy the provided `main.cpp`, `RansomGuardX.h`, `RansomGuardX.cpp`, and all files in `Detection/`, `Strategies/`, and `Utilities/` to their respective directories.
   - Ensure file names and paths match exactly, as the compilation command relies on this structure.

### Configuring file of suspicious extensions
The `FileExtensionDetectionStrategy` uses `Resources/suspicious_extensions.txt` to list suspicious file extensions.

1. **Create the File**:
   - In the `Resources/` directory, create `suspicious_extensions.txt`.

2. **Add Extensions**:
   - List one extension per line, starting with a dot (e.g., `.encrypted`, `.locky`).
   - Example content:
     ```
     .encrypted
     .locky
     .crypt
     .ransom
     ```
   - Add any extensions associated with known ransomware variants.

3. **Verify Path**:
   - Ensure the file is located at `RansomGuardX/Resources/suspicious_extensions.txt` relative to the project root.

### Setting Up VS Code (Optional)
If using Visual Studio Code for development, configure IntelliSense with `c_cpp_properties.json` to recognize include paths and compiler settings.

1. **Install VS Code**:
   - Download and install [Visual Studio Code](https://code.visualstudio.com/).
   - Install the C/C++ extension by Microsoft (`ms-vscode.cpptools`).

2. **Create `.vscode/c_cpp_properties.json`**:
   - In the `RansomGuardX/` directory, create a `.vscode/` folder.
   - Create `c_cpp_properties.json` with the following content:
     ```json
     {
         "configurations": [
             {
                 "name": "Win32",
                 "includePath": [
                     "${workspaceFolder}/**",
                     "C:/Users/Alyso/vcpkg/installed/x64-mingw-static/include",
                     "C:/Users/Alyso/vcpkg/installed/x64-mingw-static/include/SDL2",
                     "C:/msys64/ucrt64/include"
                 ],
                 "defines": [
                     "SDL_MAIN_HANDLED"
                 ],
                 "compilerPath": "C:/msys64/ucrt64/bin/g++.exe",
                 "cStandard": "c11",
                 "cppStandard": "c++17",
                 "intelliSenseMode": "gcc-x64"
             }
         ],
         "version": 4
     }
     ```
   - **Notes**:
     - Adjust `"C:/Users/Alyso/vcpkg/..."` to your vcpkg installation path if different.
     - Ensure `"compilerPath"` points to the UCRT64 `g++.exe` (e.g., `C:/msys64/ucrt64/bin/g++.exe`).
     - The `"defines"` section includes `SDL_MAIN_HANDLED` to match the code’s configuration.

3. **Verify IntelliSense**:
   - Open `RansomGuardX/` in VS Code.
   - Open `main.cpp` and check that includes (e.g., `<SDL2/SDL.h>`, `"imgui.h"`) resolve without errors.
   - Ensure autocompletion and go-to-definition work for SDL2 and ImGui functions.

## Compilation
Compile RansomGuardX using the MSYS2 MinGW64 terminal.

1. **Open MSYS2 MinGW64 Terminal**:
   - Run `C:\msys64\mingw64.exe` or search for "MSYS2 MinGW 64-bit".

2. **Navigate to Project Directory**:
   - Change to your project directory:
     ```bash
     cd /c/Users/YourUsername/Documents/Programming/C++/RansomGuardX
     ```

3. **Compile the Project**:
   - Run the following command, adjusting the vcpkg path if different:
     ```bash
     g++ -std=c++17 Detection/*.cpp Strategies/*.cpp Utilities/*.cpp imgui/*.cpp main.cpp -Wall -O3 -o main -I /c/Users/YourUsername/vcpkg/installed/x64-mingw-static/include -L /c/Users/YourUsername/vcpkg/installed/x64-mingw-static/lib -lSDL2 -lopengl32 -lsetupapi -lole32 -lgdi32 -limm32 -lversion -lwinmm -loleaut32 -luuid -mwindows
     ```
   - **Explanation**:
     - `-std=c++17`: Enables C++17 for `std::filesystem` and other features.
     - `Detection/*.cpp Strategies/*.cpp Utilities/*.cpp imgui/*.cpp main.cpp`: Includes all source files.
     - `-Wall`: Enables all warnings.
     - `-O3`: Optimizes for performance.
     - `-I /c/.../include`: Specifies vcpkg include path for SDL2 headers.
     - `-L /c/.../lib`: Specifies vcpkg library path for SDL2 and other libraries.
     - `-lSDL2 -lopengl32 ...`: Links required libraries.
     - `-mwindows`: Compiles as a Windows GUI application.

4. **Verify Compilation**:
   - If successful, an executable named `main.exe` is created in the project directory.
   - If errors occur, see [Troubleshooting](#troubleshooting).

## Running RansomGuardX
1. **Run the Executable**:
   - In the MSYS2 MinGW64 terminal, from the project directory:
     ```bash
     ./main.exe
     ```
   - Alternatively, double-click `main.exe` in File Explorer.

2. **Expected Output**:
   - A window titled "RansomGuardX - Control Panel" opens with a dark-themed GUI.
   - The interface includes fields for directory monitoring, detection settings, scan controls, and event logs.

## Usage

### GUI Overview
The RansomGuardX GUI is a full-screen control panel with the following sections:
- **Monitored Directory**:
  - Input field to specify the directory to monitor.
  - "Apply Directory" button to update the monitored path.
- **Detection Configuration**:
  - **Suspicious Extensions**: Path to `suspicious_extensions.txt`.
  - **File Modification Threshold**: Number of file changes to trigger an alert (1–50).
  - **Time Window**: Time window for modification checks (1–60 seconds).
  - **Entropy Threshold**: Entropy value for detecting encrypted files (0.0–8.0).
  - "Apply Configuration" button to update detection parameters.
- **Scan Controls**:
  - "Start Scan" / "Stop Scan" button to toggle scanning.
  - Status indicator (yellow: scanning, green: finished, red: stopped).
- **Event Logs**:
  - Displays real-time logs from `ransomware_log.txt` with timestamps.
  - Logs suspicious files, high entropy, rapid modifications, and system actions.

### Configuring Detection Parameters
1. **Set Monitored Directory**:
   - Enter a directory path (e.g., `C:\Users\YourUsername\Documents`) in the "Monitored Directory" field.
   - Click "Apply Directory" to start monitoring the new path.
   - If the path is invalid, an error is logged.

2. **Adjust Detection Settings**:
   - Update the path to `suspicious_extensions.txt` if needed.
   - Use sliders to set:
     - **File Modification Threshold**: E.g., 10 changes.
     - **Time Window**: E.g., 10 seconds.
     - **Entropy Threshold**: E.g., 7.0.
   - Click "Apply Configuration" to update the scanners.
   - Note: The entropy max file size (10 MB) is fixed in the code.

3. **Start Scanning**:
   - Click "Start Scan" to begin monitoring.
   - The status changes to "Scanning..." (yellow).
   - Logs update in real-time as threats are detected.

4. **Stop Scanning**:
   - Click "Stop Scan" to pause monitoring.
   - The status changes to "Stopped" (red) or "Scan Finished" (green) if complete.

### Viewing Logs
- Logs are displayed in the "Event Logs" section at the bottom of the GUI.
- Each log includes a timestamp and details (e.g., "Suspicious file detected: file.encrypted").
- Logs are saved to `ransomware_log.txt` in the project directory.
- Example log entries:
  ```
  [2025-05-14 23:10:00] Directory updated: C:\Users\YourUsername\Documents
  [2025-05-14 23:10:05] Suspicious file detected: document.encrypted
  [2025-05-14 23:10:10] High entropy detected in file: data.bin, Entropy: 7.5
  ```

## Troubleshooting

- **Compilation Errors**:
  - **"SDL2 not found"**:
    - Verify SDL2 is installed in vcpkg:
      ```bash
      ls /c/Users/YourUsername/vcpkg/installed/x64-mingw-static/lib/libSDL2.a
      ```
    - Reinstall SDL2:
      ```bash
      ./vcpkg remove sdl2:x64-mingw-static
      ./vcpkg install sdl2:x64-mingw-static
      ```
  - **"undefined reference to ..."**:
    - Ensure all source files are included in the compilation command.
    - Check that library paths (`-L`) and include paths (`-I`) point to the correct vcpkg directories.
  - **"std::filesystem errors"**:
    - Add `-lstdc++fs` to the compilation command:
      ```bash
      g++ ... -lstdc++fs
      ```

- **Runtime Errors**:
  - **"Error initializing SDL"**:
    - Update graphics drivers to support OpenGL 3.0.
    - Check SDL2 installation.
  - **"Could not open suspicious_extensions.txt"**:
    - Verify `Resources/suspicious_extensions.txt` exists and is accessible.
  - **GUI Not Rendering**:
    - Ensure `imgui/` contains all required files.
    - Try running `main.exe` from the terminal to see error messages:
      ```bash
      ./main.exe
      ```

- **Logs Not Updating**:
  - Check that `ransomguard_log.txt` is writable in the project directory.
  - Verify the `EventLogger` is writing logs correctly.

- **Directory Monitoring Issues**:
  - Ensure the monitored directory exists and is accessible.
  - Check logs for filesystem errors.

## Contributing
Contributions are welcome! To contribute:
1. Fork the repository (if hosted on GitHub).
2. Create a feature branch (`git checkout -b feature/YourFeature`).
3. Commit changes (`git commit -m "Add YourFeature"`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Open a pull request.

Please include:
- Detailed commit messages.
- Tests for new features (if applicable).
- Updates to this README for new dependencies or setup steps.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

**Developed by**: Alyson Melissa Sánchez Serratos 
**Last Updated**: May 14, 2025
