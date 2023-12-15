#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define ACTIVE_SOCKET_LIMIT 10

int iActiveSocketIndex = 0;
int counter = 0;
int iaSocketFd[ACTIVE_SOCKET_LIMIT];

pthread_mutex_t lock;

void *handleMessageTask(void *args)
{
  int iSocketFd = *((int *)args);
  char szBuffer[1025];

  printf("messages waiting from %d...\n", iSocketFd);

  while (1)
  {
    int size = recv(iSocketFd, szBuffer, 1024, 0);
    if (size == 0)
    {
      pthread_mutex_lock(&lock);
      counter++;
      printf("Counter increased for new accept: %d\n", counter);
      pthread_mutex_unlock(&lock);
      shutdown(iSocketFd, SHUT_RDWR);
      close(iSocketFd);
      return NULL;
    }
    szBuffer[size] = '\0';
    //printf("%s", szBuffer);
  }
  return NULL;
}

int main()
{

  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    perror("Mutex cannot created!");
    return -1;
  }

  pthread_t tasks[ACTIVE_SOCKET_LIMIT] = {0};
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 1)
  {
    perror("socket couldn't created!\n");
    return -1;
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(4002);
  // 0.0.0.0
  // 127.0.0.1
  address.sin_addr.s_addr = INADDR_ANY;
  unsigned int addressSize = sizeof(address);

  int opt_val = 1;
  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) < 0)
  {
    perror("setsockopt failed!\n");
    return -1;
  }

  if (bind(socketfd, (struct sockaddr *)&address, (socklen_t)addressSize) < 0)
  {
    perror("socket couldn't binded!\n");
    return -1;
  }

  if (listen(socketfd, 10) < 0)
  {
    perror("socket couldn't listen!\n");
    return -1;
  }

  while (1)
  {
    printf("Connection waiting...\n");
    iaSocketFd[iActiveSocketIndex] = accept(socketfd, (struct sockaddr *)&address, (socklen_t *)&addressSize);
    if (iaSocketFd[iActiveSocketIndex] < 0)
    {
      perror("accept failed!\n");
      return -1;
    }
    printf("New connection accepted!\n");

    if (pthread_create(
            &tasks[iActiveSocketIndex],
            NULL,
            &handleMessageTask,
            (void *)(&iaSocketFd[iActiveSocketIndex])) != 0)
    {
      perror("task creation failed!\n");
      return -1;
    }
    iActiveSocketIndex++;
  }

  close(socketfd);

  printf("Hello World!\n");
  return 0;
}