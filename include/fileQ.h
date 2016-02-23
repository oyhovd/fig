#ifndef FILEQ_H
#define FILEQ_H

#include <semaphore.h>
#include <errno.h>
#include "file.h"

class FileQ
{
  public:
    FileQ(size_t _size);
    ~FileQ(void);

    void close(void);
    void putFile(File * file);
    File * getFile(void);

  private:
    pthread_mutex_t lock;
    size_t size;
    std::string name;
    File ** p_files;
    size_t tail; //get from tail
    size_t head; //add to head
    size_t count;
    sem_t freeSlots;
    sem_t usedSlots;
    volatile bool is_closing;
};

#endif //FILEQ_H

