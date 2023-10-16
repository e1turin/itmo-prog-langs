/* fork-example-1.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>

sem_t shmem_read_semaphore;
sem_t shmem_write_semaphore;
const size_t N = 10;
//resource (could be in single volatile struct):
size_t idx;
int n;

void *array_holder(void *_) { // thread holds state of resource
  int array[N] = {0};		// also resource
  for (;;) {
	sem_wait(&shmem_read_semaphore); // restrict read-access

	if (idx < 0) break;
	array[idx] = n;
	for (size_t i = 0; i < N; ++i)
	  printf("%d ", array[i]);
	printf("\n");

	sem_post(&shmem_write_semaphore); // allow write-access
  }
  return 0;
}

void *array_mutator(void *_) {
  for (;;) {
	sem_wait(&shmem_write_semaphore); // restrict write-access

	scanf("%zu %d", &idx, &n);
	if (idx > N) {
	  break;
	}

	sem_post(&shmem_read_semaphore); // allow read-access
  }
  return 0;
}

int main() {
  sem_init(&shmem_write_semaphore, 0, 0);
  sem_init(&shmem_read_semaphore, 0, 1);

  pthread_t t1, t2;
  pthread_create(&t1, NULL, array_holder, NULL);
  pthread_create(&t2, NULL, array_mutator, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  sem_destroy(&shmem_write_semaphore);
  sem_destroy(&shmem_read_semaphore);
  return 0;
}
