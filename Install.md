# WMS 1.3 server install and test guide

## Supported OS
  Ubuntu 22.04.2 LTS and Windows 7+

## Steps - Windows
### Steps - Run with Pre-Built executable file.
* In "bin" directory, run the "WMSServer.exe" in "command prompt".
  Wait until the message "Press Enter to stop the server or '!' to restart the server..." is displayed.

### Steps - Obtain a executable file by compile source codes.
* At first, unzip the "modules.7z" and "cache.7z" files.
* Install Visual Studio 2019 and cmake version 3.26.4.
* After installed cmake, append the following path into system environment's path.
   cmake_path/bin/cmake.exe(cmake_path is path of cmake installed in your system)
* Run "x86 Native Tools Command Prompt" of Visual Studio 2019.
* Go to the project path.
* Run the following commands.
  - mkdir build
  - cd build
  - cmake ..
* Close the Command Prompt and check the "build" directory that contains the WMSServer.sln file.
* Open the solution with Visual Studio 2019.
* Go to "Build" menu - "Configuration Manager".
* set "Active solution configuration" to "Release" and build the "WMSServer" project.
* Run the "command prompt" and go to the "Release" path and execute the "WMSServer.exe".
  Wait until the message "Press Enter to stop the server or '!' to restart the server..." is displayed.

## Steps - Ubuntu
* At first, unzip the "modules.7z" file.
* Go to the project path.
* Run the following commands.
  - sudo apt install devscripts equivs
  - sudo mk-build-deps -i -r ci/control
  - sudo apt-get --allow-unauthenticated install -f
  - sudo apt-get install -y binutils-dev uuid-dev libssl-dev  
  - sudo apt-get install libgtk-3-dev
  - mkdir build
  - cd build
  - cmake ..
  - make -j4
* Execute the "build/WMSServer.exe".
  Wait until the message "Press Enter to stop the server or '!' to restart the server..." is displayed.

## Pre-Build Steps
* The directory that contains ENC files must be typed at "ENCDirPath" in "project_path/env/config.json" file.  
* The rendered image file prefix must be typed at "IMGFilePath" in "project_path/env/config.json" file.
* The ssl certificate files should be located in "project_path/Append_data/tools/certificates/" directory.
The "project_path" is path in which the "CMakeLists.txt" file exists.

## Pre-Run Steps
* Copy all ENC files into the directory indicated in "config.json" file.
* Should modify the port number for HTTP and HTTPS in "config.json" file

## Test
* Open the Web Browser.
* If you need to rebuild Chart DB, you should set the "rebuildCharts" to "1". Otherwise, you should set the "rebuildCharts" to "0".
* Test with the following URL

For HTTP
  http://localhost:8080/wms?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&LAYERS=grid,depths&CRS=4326&BBOX=41,49,41.5,49.5&WIDTH=651&HEIGHT=740&FORMAT=image/png

For HTTPS
  https://localhost:8081/wms?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&LAYERS=grid,depths&CRS=4326&BBOX=41,49,41.5,49.5&WIDTH=651&HEIGHT=740&FORMAT=image/png