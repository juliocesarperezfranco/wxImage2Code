# wxImage2Code
This is a very basic program generates that generates the code required to generate a wxImage or wxBitmap object from an actual image, is basically what wxFormBuilder does with embedded files but as a command-line program.

# Requirements
* CMake 3.15 or newer.
* C++17 support.

# How to use
Just pass the list of files and it will generate the header and source files using the inpu file names, example:
```bash
wxImageToCode file1
```
This will generate the file: file1.h that will declare the methods like:
```cpp
wxBitmap& Getfile1NameBitmap();
wxImage& Getfile2NameImage();
```
And will also generate the file1.cxx that defines their respective functions.
