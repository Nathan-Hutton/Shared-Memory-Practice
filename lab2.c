#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>


int main(int argc, char *argv[])
{
	const char *name = "COLLATZ1";
	const int SIZE = 4096;

	int value;
	value = atoi(argv[1]);

	int shm_fd;
	void *ptr;
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd,SIZE);

	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		exit(-1);
	}

	pid_t pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Fork failed\n");
	}

	else if (pid == 0) {
		const int STR_SIZE = 128;
		char str[STR_SIZE];

		while (value != 1) {
			if (value % 2 != 0) {
				value = (value * 3) + 1;
			}
			else {
				value /= 2;
			}
			sprintf(str,"%d\n",value);
			sprintf(ptr, "%s", str);
			ptr += strlen(str);
		}
	}

	else {
		wait(NULL);
		printf("%s\n", (char *)ptr);

		// remove the shared memory segment
		if (shm_unlink(name) == -1) {
			printf("Error removing %s\n",name);
			exit(-1);
		}

		printf("Done\n");
	}

	return 0;
}
