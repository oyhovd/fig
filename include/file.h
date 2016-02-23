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

#endif
