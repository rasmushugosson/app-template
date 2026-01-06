# App Template

## General

This is a minimal project template for building graphics applications in `C++` using my utility libraries. It serves as a starting point for new projects and demonstrates how the libraries work together. The template is built using [Premake5](https://premake.github.io/) for `C++23`.

To start a new project, clone this repository and replace the sandbox code with your own application.

This project depends on the following libraries, included as git submodules:
- [log-lib](https://github.com/rasmushugosson/log-lib) - Logging, exceptions and timing utilities
- [ecs-lib](https://github.com/rasmushugosson/ecs-lib) - Entity Component System
- [app-lib](https://github.com/rasmushugosson/app-lib) - Graphics application wrapper (GLFW, GLEW, ImGui, OpenGL, Vulkan, etc.)

## Getting Started

1. **Clone the repository** with submodules and open a terminal in the project root.
   ```bash
   git clone --recursive https://github.com/rasmushugosson/app-template.git
   ```

The next steps depend on your preferred build system below.

### Visual Studio

2. Run `premake5 vs20XX` to generate a Visual Studio solution file (`.sln`).
3. Open the solution file in Visual Studio and build using MSVC.

### Gmake (force G++)

2. Run the pre-defined action `premake5 gmake-gcc` to generate makefiles specifically for GCC.
3. Navigate into `/build/[os]-gcc` where the `Makefile` is created.
4. Run `make config=[build type]` where the possible options are `debug`, `release` or `dist`.
5. Run the `Sandbox` executable from the project root: `./bin/Sandbox/[build type]/Sandbox`.

### Gmake (force Clang++)

2. Run the pre-defined action `premake5 gmake-clang` to generate makefiles specifically for Clang.
3. Navigate into `/build/[os]-clang` where the `Makefile` is created.
4. Run `make config=[build type]` where the possible options are `debug`, `release` or `dist`.
5. Run the `Sandbox` executable from the project root: `./bin/Sandbox/[build type]/Sandbox`.

### Formatting and Linting

There are additional actions for formatting with `clang-format` and linting through `clang-tidy`. These are run through:

```bash
# Run clang-format
premake5 format

# Run clang-tidy
premake5 lint

# Run clang-tidy and apply fixes
premake5 lint-fix
```

These commands assume `clang-format` and `clang-tidy` are installed on your system.

### Clangd

If you use `clangd` for intellisense and code completion, the provided `gen-build-cmds.sh` script will generate build commands for `clangd`. This will call `premake5 gmake-clang` and use `bear` to generate the build commands. Both `clang` and `bear` are assumed to be installed on your system.

### Additional Dependencies

- **Premake5:** This project uses [Premake5](https://premake.github.io/) as its build configuration tool.
  Ensure that `premake5` is installed on your system or copied into the root folder.
  You can download it [here](https://premake.github.io/download/).

- **Graphics Drivers / SDKs:**
  - **OpenGL:** The project assumes OpenGL drivers are installed and available by default.
  - **Vulkan:** Optional. If Vulkan is not available, only the OpenGL graphics API will be usable.

- **Linux Dependencies:** On Linux, install the required system packages:
  ```bash
  # Debian/Ubuntu
  sudo apt install libglfw3-dev libglew-dev libopenal-dev

  # Arch Linux
  sudo pacman -S glfw glew openal
  ```

  For Vulkan support on Linux, install the Vulkan headers and validation layers:
  ```bash
  # Debian/Ubuntu
  sudo apt install vulkan-headers vulkan-validationlayers

  # Arch Linux
  sudo pacman -S vulkan-headers vulkan-validation-layers
  ```
  Note: Validation layers are only required for debug builds.

- **Windows Vulkan:** For Vulkan support on Windows, install the [Vulkan SDK](https://www.vulkan.org/tools#download-these-essential-development-tools). The SDK installer sets the `VULKAN_SDK` environment variable automatically, which the build system uses to locate Vulkan headers and libraries.

## Project Structure

```
app-template/
├── sandbox/src/       # Application source code (replace with your own)
├── dep/               # Library dependencies (git submodules)
│   ├── app-lib/       # Graphics application wrapper
│   ├── ecs-lib/       # Entity Component System
│   └── log-lib/       # Logging utilities
├── res/               # Resources (icons, cursors, fonts)
├── premake5.lua       # Build configuration
└── gen-build-cmds.sh  # Clangd build commands generator
```

## Build Configurations

The build configuration determines which logging macros from log-lib are active:

| Configuration | Define        | Logging                                      |
|---------------|---------------|----------------------------------------------|
| `debug`       | `AE_DEBUG`    | `AE_LOG()` and `AE_LOG_BOTH()` are active    |
| `release`     | `AE_RELEASE`  | `AE_LOG_RELEASE()` and `AE_LOG_BOTH()` are active |
| `dist`        | `AE_DIST`     | All logging disabled                         |

## Included Assets

The `res` folder contains assets used in the example program:
- `icons/` - Window icons of different sizes
- `cursors/` - Custom cursor images
- `fonts/` - Font used by Dear ImGui (Ubuntu font, licensed under Ubuntu Font License)

The font license can be found in `res/fonts/UFL.txt`. All other assets are created by me.

## Supported Platforms

- Windows (MSVC)
- Linux (GCC / Clang)
- Likely MacOS (not yet tested)

## License

This project is licensed under the **Apache License 2.0**.
See the [LICENSE](LICENSE) file in this repository for details.
