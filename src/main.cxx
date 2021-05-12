/**
 *  \file main.cxx
 *  \brief This program generates the code required to generate an wxImage or wxBitmap object from an actual image.
 *         Is based on the wxFormBuilder functionality, except that this can be used to process files in batches.
 */
 
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <filesystem>

void MakeUpperCase(std::string& stringToMakeUppercase)
{
  for(uint16_t i=0; i<stringToMakeUppercase.length(); i++)
  {
    stringToMakeUppercase[i] = toupper(stringToMakeUppercase[i]);
  }
};

void PrintMessage(const std::string message)
{
  std::cout << message << std::endl;
};

void PrintVersion()
{
  PrintMessage("wxImg2Code v1.0.0.0"); 
};

void PrintHelp()
{
  PrintVersion();
  PrintMessage("===================");
  PrintMessage("Usage: wxImg2Code <filename>");
  PrintMessage("Generates the header and source files.");
  PrintMessage("Example:");
  PrintMessage("img2cod test1.jpg test2.jpg");
};

void GenerateCode(std::string fileNamePathUrl)
{
  const std::filesystem::path path = std::string(fileNamePathUrl);
  if(std::filesystem::exists(path) == false)
  {
    PrintMessage("Error: File " + fileNamePathUrl + " does not exists");
    return;
  }

  std::string inputFileName = path.filename().string();
  std::string inputExtension = path.filename().string();

  inputExtension = inputExtension.substr(inputExtension.length() - 3, 3);
  std::transform(inputExtension.begin(), inputExtension.end(), inputExtension.begin(), ::toupper);

  std::string imageDataContainerName = path.filename().string();
  imageDataContainerName = imageDataContainerName.erase(imageDataContainerName.length() - 4) + "ImageData";

  std::string getImageBitmapFunctionName = path.filename().string();
  getImageBitmapFunctionName = getImageBitmapFunctionName.erase(getImageBitmapFunctionName.length() - 4);
  getImageBitmapFunctionName = "Get" + getImageBitmapFunctionName + "Bitmap";

  std::string getImageImageFunctionName = path.filename().string();
  getImageImageFunctionName = getImageImageFunctionName.erase(getImageImageFunctionName.length() - 4);
  getImageImageFunctionName = "Get" + getImageImageFunctionName + "Image";

  std::string outputHeaderFileName = path.filename().string();
  std::string outputSourceFileName = path.filename().string();

  outputHeaderFileName = outputHeaderFileName.erase(outputHeaderFileName.length() - 4) + ".h";
  outputSourceFileName = outputSourceFileName.erase(outputSourceFileName.length() - 4) + ".cxx";

  std::string IMAGE_FORMAT_TYPE = "wxBITMAP_TYPE_PNG";
  if(inputExtension == "TIF")
  {
    IMAGE_FORMAT_TYPE = "wxBITMAP_TYPE_TIF";
  }
  if(inputExtension == "JPG")
  {
    IMAGE_FORMAT_TYPE = "wxBITMAP_TYPE_JPG";
  }
  if(inputExtension == "PNG")
  {
    IMAGE_FORMAT_TYPE = "wxBITMAP_TYPE_PNG";
  }
  if(inputExtension == "BMP")
  {
    IMAGE_FORMAT_TYPE = "wxBITMAP_TYPE_BMP";
  }
  if(inputExtension == "GIF")
  {
    IMAGE_FORMAT_TYPE = "wxBITMAP_TYPE_GIF";
  }

  std::ifstream inputFile;
  inputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try
  {
    inputFile.open(fileNamePathUrl, std::ios::binary);
  }
  catch(std::system_error& e)
  {
    PrintMessage(e.what()); 
    return;
  }
  // Get the file lenght.
  inputFile.seekg(0, std::ios::end);
  long  x = inputFile.tellg();

  // Start to move around the file.
  inputFile.seekg(0, std::ios::beg);
  char* s = new char[x];
  inputFile.read(s, x);
  inputFile.close();

  std::ofstream headerFile;
  headerFile.open(outputHeaderFileName);

  headerFile << "#pragma once" << std::endl << std::endl;

  headerFile << "#include <wx/mstream.h>" << std::endl;
  headerFile << "#include <wx/image.h>" << std::endl;
  headerFile << "#include <wx/bitmap.h>" << std::endl;
  headerFile << std::endl;

  // Header file
  headerFile << "/** " << std::endl;
  headerFile << " *  \\file " << outputHeaderFileName << std::endl;
  headerFile << " *  \\brief " << std::endl;
  headerFile << " *  Data size = " << x << " Bytes" << std::endl;
  headerFile << " */" << std::endl << std::endl;

  // Declare the structure
  headerFile << "static const uint8_t " << imageDataContainerName << "[] =" << std::endl;
  headerFile << "{" << std::endl;

  inputFile >> std::noskipws;
  headerFile << "  ";
  for(int i = 1; i < x; i++) // Iterate the file and get the chunks as hexa.
  {
    headerFile << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>((unsigned char)s[i - 1]);
    if(i < (x - 1))
    {
      headerFile << ", ";
    }

    if((i % 10 == 0)) // Line width format.
    {
      headerFile << std::endl << "  ";
    }

    if(i == x - 1) // Last character.
    {
      headerFile << ", 0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << static_cast<int>((unsigned char)s[i]);
    }
  }

  headerFile << std::endl;
  headerFile << "};" << std::endl;
  headerFile << std::endl;
  headerFile << "wxBitmap& " << getImageBitmapFunctionName << "();" << std::endl;
  headerFile << "wxImage& "  << getImageImageFunctionName << "();" << std::endl;
  headerFile.close();

  std::ofstream implementationFile;
  implementationFile.open(outputSourceFileName);
  implementationFile << "#include \"" << outputHeaderFileName << "\"" << std::endl;
  implementationFile << std::endl;

  implementationFile << "wxBitmap& " << getImageBitmapFunctionName << "()" << std::endl;
  implementationFile << "{" << std::endl;
  implementationFile << "  static wxMemoryInputStream memIStream( " << imageDataContainerName << ", sizeof( " << imageDataContainerName << " ) );" << std::endl;
  implementationFile << "  static wxImage image( memIStream, " + IMAGE_FORMAT_TYPE + " );" << std::endl;
  implementationFile << "  static wxBitmap bmp( image );" << std::endl;
  implementationFile << "  return bmp;" << std::endl;
  implementationFile << "}" << std::endl;

  implementationFile << "wxImage& " << getImageImageFunctionName << "()" << std::endl;
  implementationFile << "{" << std::endl;
  implementationFile << "  static wxMemoryInputStream memIStream( " << imageDataContainerName << ", sizeof( " << imageDataContainerName << " ) );" << std::endl;
  implementationFile << "  static wxImage image( memIStream, " + IMAGE_FORMAT_TYPE + " );" << std::endl;
  implementationFile << "  return image;" << std::endl;
  implementationFile << "}" << std::endl;

  implementationFile.close();
}

int main(int argc, char *argv[])
{
  std::vector<std::string> arguments(argv + 1, argv + argc);
  
  if(arguments.size() < 1)
  {
    PrintHelp();
    return 0;
  }

  std::string firstParam = arguments[0];
  MakeUpperCase(firstParam);
  if(firstParam == "-H" || firstParam == "-HELP" || firstParam == "-?")
  {
    PrintHelp();
    return 0;
  }

  if(firstParam == "-V"|| firstParam == "-VERSION")
  {
    PrintVersion();
    return 0;
  }

  for(auto it: arguments)
  {
    GenerateCode(it);
  }
  return 0;
}
