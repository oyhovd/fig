#include <iostream>
#include <algorithm>
#include <errno.h>
#include "file.h"
#include "fileQ.h"

FileQ::FileQ(size_t _size, size_t _threadCount) : size(_size), threadCount(_threadCount)
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

  //give semaphore to all threads so getFile can move on and this madness can be closed
  while(threadCount--)
  {
    if (sem_post(&usedSlots) == -1)
    {
      //semaphore dead
      perror("FATAL ERROR: Semaphore");
      exit(-1);
    }
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


