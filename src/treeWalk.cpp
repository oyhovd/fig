#include <iostream>
#include <dirent.h>

#include "treeWalk.h"

void treeWalk(std::string parent, std::string dir)
{
  if(parent != "")
  {
    parent += "/";
  }

  std::string dirFullPath = parent + dir;

  dirent* de;
  DIR* dp;
  dp = opendir(dirFullPath.c_str());

  if (dp)
  {
    while (true)
    {
      de = readdir( dp );
      if (de == NULL)
      {
        break;
      }

      if (de->d_name[0] == '.')
      {
        continue;
      }

      if(de->d_type == DT_DIR)
      {
        std::string newParent = parent + dir;
        treeWalkDirCb(newParent, de->d_name);
        treeWalk(newParent, de->d_name);
        continue;
      }

      if(de->d_type == DT_REG)
      {
        std::string newParent = parent + dir;
        treeWalkFileCb(newParent, de->d_name);
        continue;
      }
    }

  closedir( dp );
  }

}

