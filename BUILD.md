srcSlice is a srcML tool, if you do not have srcML installed you must install it.
You can download an installer [here](https://www.srcml.org/#download) or you can
```console
# After running 'make' when building srcML, run 'make install' to ensure cmake can find the srcML package when building srcSlice
git clone --recursive https://github.com/srcML/srcML.git
```

## Unix

This project uses the `nlohmann json` for handling JSON
```console
# Linux
apt install nlohmann-json3-dev
sudo pacman -S nlohmann-json
# macOS
brew install nlohmann-json
```

```console
# Clone srcSlice repository
git clone --recursive https://github.com/srcML/srcSlice.git

# Run cmake to create the build files and run make to build the executable
cmake [path to srcSlice source directory]

# You can run make but it will take longer
make srcslice
```

## Windows Using MSVC

Necessary tools include:

* [CMake](http://www.cmake.org)
* [Visual Studio 2017 or later](https://www.visualstudio.com/downloads/)

Building in Windows requires MSVC. Dependencies are handled via [vcpkg](vcpkg.io), see the [vcpkg installation directions](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd).

First, generate the build files in your target build directory. Using the preset sets up the use of vcpkg:

```console
cmake [path to srcSlice source directory] --preset ci-msvc -DBUILD_TESTS=OFF
```

Second, build. Doing so via cmake means you do not have to know the build program name or location:

```console
cmake --build . --target srcslice --config release
```

The directory `bin` in the build folder contains the srcSlice executable along with all other dependencies.