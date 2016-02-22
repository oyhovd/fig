#ifndef FILE_H
#define FILE_H

#include <cstdio>

#include <sstream>

//cygwin patch found somewhere on the internets
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

class File
{

  public:
    File(std::string _fileName);
    ~File();

    void readContent(void);
    void findPattern(std::string pattern, bool listFiles, bool lineNumbers);

    std::string name;
    char * content;
    size_t contentSize;
    File * next;
};

File::File(std::string _fileName) : name(_fileName)
{
  this->content = NULL;
}

File::~File()
{
  delete [] this->content;
}

void File::readContent(void)
{
  FILE* f = fopen(this->name.c_str(), "r");
  
  if(f == NULL)
  {
    return;
  }

  // Determine file size
  fseek(f, 0, SEEK_END);
  this->contentSize = ftell(f);

  this->content = new char[this->contentSize];

  rewind(f);
  fread(this->content, sizeof(char), this->contentSize, f);
  fclose(f);
}

void File::findPattern(std::string pattern, bool listFiles, bool lineNumbers)
{
  if(pattern == "")
  {
    return;
  }

  if(this->content == NULL)
  {
    return;
  }

  size_t stringLen = pattern.length();
  char first = pattern[0];
  size_t line = 1;
  size_t lineStart = 0;

  for(size_t i = 0; i < this->contentSize; i++)
  {
    if(this->content[i] == '\n')
    {
      line++;
      lineStart = i+1;
    }
    //optimization: Check first char first
    if(this->content[i] != first)
    {
      continue;
    }
    for(size_t j = 0; j < stringLen; j++) 
    {
      if(this->content[i+j] != pattern[j])
      {
        break;
      }
      if(j == stringLen - 1)
      {
        //the pattern matched!
        std::cout << this->name;

        if(listFiles)
        {
          std::cout << "\n";
          return;
        }

        if(lineNumbers)
        {
          std::cout << ":" << patch::to_string(line);
        }

        std::cout << ":";

        //print the line
        size_t k;
        for(k = lineStart; k < this->contentSize; k++)
        {
          if(this->content[k] == '\r')
          {
            continue;
          }
          if(this->content[k] == '\n')
          {
            break;
          }
          std::cout << this->content[k];
        }
        std::cout << "\n";

        //fast forward to end of line
        i = k-1; //minus one since one will be added before entering the loop again
      }
    }
  }
}

#endif
