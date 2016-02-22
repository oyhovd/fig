#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include "file.h"
#include "treeWalk.h"
#include "fileQ.h"

static char * pattern = NULL;
static char * path = NULL;
static std::vector<std::string> fileTypes;
static FileQ * treeToReaderQ;
static FileQ * readerToGrepperQ;
static pthread_t readWorker;
static pthread_t grepWorker;

static bool caseInsensitive = false;
static bool listFiles = false;
static bool lineNumbers = false;

void * readWorkerThread(void * arg);
void * grepWorkerThread(void * arg);

int main(int argc, char * argv[])
{
  std::string helpString = "Usage: fig [-i] [-l] [-n] [-t <file type>] [path] pattern\n";

  if(argc < 2)
  {
    std::cout << helpString;
    return 0;
  }

  for(int i = 1; i < argc; i++)
  {
    if(argv[i][0] == '-')
    {
      int j = 1;
      while(argv[i][j] != '\0')
      {
        if(argv[i][j] == 'i')
        {
          caseInsensitive = true;
          std::cout << "Not implemented yet\n";
          return 0;
        }
        else if(argv[i][j] == 'l')
        {
          listFiles = true;
          std::cout << "Not implemented yet\n";
          return 0;
        }
        else if(argv[i][j] == 'n')
        {
          lineNumbers = true;
          std::cout << "Not implemented yet\n";
          return 0;
        }
        else if(argv[i][j] == 't')
        {
          fileTypes.push_back(std::string(argv[++i]));
          break;
        }
        else
        {
          std::cout << helpString;
          return 0;
        }

        j++;
      }
    }
    else
    {
      if(!path && pattern)
      {
        path = pattern;
      }

      pattern = argv[i];
    }
  }
  if(argc > 2)
  {
    //store file type pattern
    for(int i = 0; i < argc-2; i++)
    {
      fileTypes.push_back(std::string(argv[i+1]));
    }

  }

  //Creating resources
  treeToReaderQ = new FileQ(10);
  readerToGrepperQ = new FileQ(10);
  pthread_create(&readWorker, NULL, readWorkerThread, NULL);
  pthread_create(&grepWorker, NULL, grepWorkerThread, NULL);

  //Parse tree

  if(!path)
  {
    treeWalk("", ".");
  }
  else
  {
    treeWalk("", path);
  }

  //Wait for grepping to be done
  treeToReaderQ->close();
  readerToGrepperQ->close();

  //Clean up
  pthread_join(readWorker, NULL);
  pthread_join(grepWorker, NULL);
  delete treeToReaderQ;
  delete readerToGrepperQ;

  //workaround for cygwin not waiting for all output being done
  std::cin.putback('\n');
  std::cin.get();

  return 0;
}

void treeWalkDirCb(std::string parent, std::string dirName)
{
  //Nada, for debugging
}

void * readWorkerThread(void * arg)
{
  File * p_file;
  while((p_file = treeToReaderQ->getFile()) != NULL)
  {
    p_file->readContent();
    readerToGrepperQ->putFile(p_file);
  }

  pthread_exit(NULL);

}

void * grepWorkerThread(void * arg)
{
  File * p_file;
  while((p_file = readerToGrepperQ->getFile()) != NULL)
  {
    p_file->findPattern(pattern);
    delete p_file;
  }

  pthread_exit(NULL);

}

void treeWalkFileCb(std::string parent, std::string fileName)
{
  std::string fullPath = parent + "/" + fileName;
  std::string fileType;
  for(size_t i = 0; i < fileName.length(); i++)
  {
    if(fileName[i] != '.')
    {
      continue;
    }
    if(++i < fileName.length())
    {
      fileType = std::string(&fileName.c_str()[i]);
    }
  }

  if (fileTypes.size() > 0 && !(std::find(fileTypes.begin(), fileTypes.end(), fileType) != fileTypes.end()))
  {
    return;
  }

  File * p_file = new File(fullPath);
  treeToReaderQ->putFile(p_file);
}


