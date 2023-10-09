# WMS 1.3 server

Implementation of a Web Map Server v1.3.0

## Features

- Cross platform (Linux, Windows)
- Supported Web Protocols: HTTP and HTTPS
- Only process GetMap request and response with Map Image (PNG, JPG).
- Othere request is dismissed with 404 status code.
- User could get s57 ENC and s63 Map Images.
- Parameters are as follows.
  LAYERS : TEXT, DEPTHS, BLLABELS, LIGHTS, LDESCR, AINFO, SLVIS, GRID.
  FORMAT : image/png or image/jpg
  CRS : 4326
  BBOX : The region configured by min and max value of latitude and longitude(ex : min latitude, min longitude, max latitude, max longitude)
  WIDTH : Image width
  HEIGHT : Image height
  REQUEST : GetMap
  VERSION : 1.3.0
  SERVICE : WMS

## Requirements

* Linux 
* Windows
* [cmake](https://www.cmake.org)
* [gcc](https://gcc.gnu.org)
* [git](https://git-scm.com)

Optional:
* [clang](https://clang.llvm.org)
* [MSYS2](https://www.msys2.org)
* [MinGW](https://mingw-w64.org/doku.php)
* [Visual Studio](https://www.visualstudio.com)

#### Environment
Environment File Path : ./env/config.json

#### Description
The Web Map Server is real time service server.
Users could browse the maps with combination of several parameters.
The lighthouses and buoys, several constructions could be distincted by combination of parameters.
For example, if you want to see the buoys only, you could remove all Layer parameters except the BLLABELS.
As you can see with the result image, on the map, only buoys would be displayed.
How efficient way to browse map!