-----------------------------------------------
DirectXTK - the DirectX Tool Kit for DirectX 12
-----------------------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

*** MICROSOFT CONFIDENTIAL ***

This is a private fork of the DirectX Tool Kit for DX 12 project from GitHub
updated to support "Project Antibes".

https://github.com/Microsoft/DirectXTK12

This matches the functionality of the April 26, 2019 release.


--------
XBOX ONE
--------

* Developers using the Project Antibes Gaming GXDK need to generate the
  Src\Shaders\Compiled\GamingXbox*.inc files to build the library as they are
  not included in the distribution package. They are built by running the script
  in Src\Shaders - "CompileShaders gxdk" from the
  "Xbox Gaming GSDK Developer Command Prompt".


------------
KNOWN ISSUES
------------

* The Mouse class is currently a 'null device' for the Project Antibes
  Gaming.Xbox.x64 platform.
