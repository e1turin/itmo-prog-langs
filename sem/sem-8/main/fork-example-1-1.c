/* fork-example-1.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

void *create_shared_memory(size_t size) {
  return mmap(NULL,
			  size,
			  PROT_READ | PROT_WRITE,
			  MAP_SHARED | MAP_ANONYMOUS,
			  -1, 0);
}

int main() {
  const size_t N = 10;
  int *shmem = create_shared_memory(N * sizeof(int));

  printf("Shared memory at: %p\n", shmem);
  int pid = fork();
  if (pid == 0) { /* child */
	for (size_t i = 0; i < N; ++i) {
	  shmem[i] = i + 1;
	  printf("[child]: shmem[%zu] = %d\n", i, shmem[i]);
	}

	size_t i;
	int n;

	puts("[child]: enter i and n in format %zu %d:\n");
	scanf("%zu %d", &i, &n);
	shmem[i] = n;
	printf("[child]: shmem[%zu] = %d\n", i, n);

  } else { /* parent */
	printf("Child's pid is: %d\n", pid);
	wait(NULL);
	for (size_t i = 0; i < N; ++i) {
	  printf("[parent]: shmem[%zu] = %d\n", i, shmem[i]);
	}
  }

  shmdt(shmem);
}
