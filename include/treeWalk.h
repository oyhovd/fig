#ifndef TREEWALK_H
#define TREEWALK_H

#include <iostream>

void treeWalk(std::string parent, std::string dir);
void treeWalkDirCb(std::string parent, std::string dirName);
void treeWalkFileCb(std::string parent, std::string fileName);

#endif //TREEWALK_H
