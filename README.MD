# MKCompress project
This is software for changing file headers and compressing files.<br>
It can be change file header then the file can not be recognize on soft.<br>
It can mix folder and file to compress and auto save origin file name then the change file header and file type, And it will recovery on MKCompress soft.

## Enviroment & Ide
  - VS2019 msvc2019 x64
  - QT linguist and desingner v6.2.0

## Use bit7z and MKCY(miku change head tools) to compress/decompress or change header/back
  - bit7z-v3.1.3 https://github.com/rikyoz/bit7z
  - MKCT-v1.2 https://github.com/monoliths-uni/MKCT

## Progress
  - Forward:
    - compress>>add last name to end file>>change header

## Bug list(bug will be erase on next version):
  - Can not compress use chinese name fluter
  - Can not use UTF-8 String to initial output file name
  - Compress are not responce
  - This version does not have separate decompression capability

## Package and publish
  - windeployqt
  - Qt Installer Framework


## Verion
  - 1.0 added basics compose
    - change header
    - compress