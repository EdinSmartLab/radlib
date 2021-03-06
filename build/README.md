
## Build source code with cmake. 

* Note: CMakeLists.txt files are in ../source directory and its subdirectories.
* Note: All files in this folder can be deleted except for user_config, this README, 
      and the clean_this_dir.sh script.

--------------------------------------------------------------------------

### Software required:
* cmake (3.12 or higher)
* doxygen (optional; for building documentation)

--------------------------------------------------------------------------

### Build steps
* Step 1: run cmake
    * Edit the user_config file for settings and paths.
    * Run: `cmake -C user_config ../source/c++` 
* Step 2: build the radlib code
    * Run: `make`
* Step 3: install the library
    * Run: `make install`
* Step 4 (OPTIONAL): build documentation
    * Run: `make doxygen`
* Step 5: clean the build
    * Run: `make clean`
    * Or, for a more thorough clean, run: `./clean_this_dir.sh`
