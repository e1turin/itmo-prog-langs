/* fork-example-1.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
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

  int pipes[2][2];
  pipe(pipes[0]);
  pipe(pipes[1]);

  printf("Shared memory at: %p\n", shmem);
  int pid = fork();
  if (pid == 0) {
	/* child */
	// Сохраним нужные дескпиторы конвейеров
	int to_parent_pipe = pipes[1][1];
	int from_parent_pipe = pipes[0][0];

	// И закроем ненужные
	close(pipes[1][0]);
	close(pipes[0][1]);

	for (size_t i = 0; i < N; ++i) {
	  shmem[i] = i + 1;
	  printf("[child]: shmem[%zu] = %d\n", i, shmem[i]);
	}

	// Будем читать строки и отправлять их сообщениями
	for (;;) {
	  size_t i;
	  int n;
	  puts("[child]: enter i and n in format %zu %d:\n");
	  scanf("%zu %d", &i, &n);
	  shmem[i] = n;
	  printf("[child]: shmem[%zu] = %d\n", i, n);

	  // И отпавим её родителю
	  int is_end = 0;
	  if (i < 0 || i > 10 - 1) is_end = 1;
	  write(to_parent_pipe, &is_end, 1);

	  if (is_end) break;
	}

	close(from_parent_pipe);
	close(to_parent_pipe);

	return 0;
  }

  /* parent */

  // Далее выполняется только родительский поток
  // Сохраним нужные дескпиторы конвейеров
  int from_child_pipe = pipes[1][0];
  int to_child_pipe = pipes[0][1];

  // И закроем ненужные
  close(pipes[1][1]);
  close(pipes[0][0]);

  printf("Child's pid is: %d\n", pid);

  // Будем ждать, пока ребенок не пришлет сообщение
  if (pid > 0) {
	for (;;) {
	  int is_end;
	  read(from_child_pipe, &is_end, 1);

	  if (is_end) {
		break;
	  }
	  for (size_t i = 0; i < N; ++i) {
		printf("[parent]: shmem[%zu] = %d\n", i, shmem[i]);
	  }

	}
  }

  // Закроем дескпиторы
  close(from_child_pipe);
  close(to_child_pipe);
  wait(NULL);
  shmdt(shmem);
}
