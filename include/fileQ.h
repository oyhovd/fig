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

FileQ::FileQ(size_t _size) : size(_size)
{
  lock = PTHREAD_MUTEX_INITIALIZER;
  p_files = new File * [size];
  tail = 0;
  head = 0;
  is_closing = false;
  sem_init(&freeSlots, 0, size);
  sem_init(&usedSlots, 0, 0);
}

FileQ::~FileQ(void)
{
  sem_destroy(&freeSlots);
  sem_destroy(&usedSlots);
  delete p_files;
}

void FileQ::close(void)
{
  for(size_t i = 0; i < size; i++)
  {
    //eat all free slots. When all are available, we are done.
    int s;
    while ((s = sem_wait(&freeSlots)) == -1 && errno == EINTR)
    {
      continue;       /* Restart if interrupted by handler */
    }
  }

  is_closing = true;

  //give semaphore so getFile can move on and is_closing this madness
  if (sem_post(&usedSlots) == -1)
  {
    //semaphore dead
    perror("FATAL ERROR: Semaphore");
    exit(-1);
  }
}

void FileQ::putFile(File * file)
{
  //sem wait on free space
  int s;
  while ((s = sem_wait(&freeSlots)) == -1 && errno == EINTR)
  {
    continue;       /* Restart if interrupted by handler */
  }
  if(s == -1)
  {
    //semaphore dead
    perror("FATAL ERROR: Semaphore");
    exit(-1);
  }

  //get mutex
  pthread_mutex_lock(&lock);
  p_files[head++] = file;
  if (head >= size)
  {
    head = 0;
  }

  //release mutex
  pthread_mutex_unlock(&lock);

  //sem give on p_files
  if (sem_post(&usedSlots) == -1)
  {
    //semaphore dead
    perror("FATAL ERROR: Semaphore");
    exit(-1);
  }
}

//If this returns NULL, the pipe is stopped and the semaphores closed.
File * FileQ::getFile(void)
{
  File * retval;

  //sem wait on ready p_files
  int s;
  while ((s = sem_wait(&usedSlots)) == -1 && errno == EINTR)
  {
    continue;       /* Restart if interrupted by handler */
  }
  if(s == -1)
  {
    //semaphore dead
    perror("FATAL ERROR: Semaphore");
    exit(-1);
  }

  if(is_closing)
  {
    return NULL;
  }

  //get mutex
  pthread_mutex_lock(&lock);

  retval = p_files[tail++];
  if (tail >= size)
  {
    tail = 0;
  }

  //release mutex
  pthread_mutex_unlock(&lock);

  //sem give on free slots
  if (sem_post(&freeSlots) == -1)
  {
    //semaphore dead
    perror("FATAL ERROR: Semaphore");
    exit(-1);
  }

  return retval;
}

#endif //FILEQ_H

