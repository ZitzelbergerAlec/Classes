#define _BSD_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *mount_shmem(char *path, int object_size)
{
	int shmem_fd;
	void *addr;

	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if(shmem_fd == -1){
		fprintf(stdout, "Failed to open shared memory object\n");
		exit(EXIT_FAILURE);
	}

	if(ftruncate(shmem_fd, object_size) == -1){
		fprintf(stdout, "Failed to resize shared memory object\n");
		exit(EXIT_FAILURE);
	}

	/* map the shared memory object */
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);

	if(addr == MAP_FAILED){
		fprintf(stdout, "Failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}
	
	return addr;
}

int main(int argc, char **argv){
	unsigned char *bitmap;
	int bitmap_size = 340000000 / 8 + 1;
	
	int *perfect_numbers;
	int perfect_number_count = 10;

	int object_size = 1024 * 1024 * 60; //6 megabytes

	void *addr = mount_shmem("/merrickd_example", object_size);

	/* everything is done relative to addr.
	BE VERY CAREFUL WITH POINTER ARITHMETIC! It doesn't necessarily work the way you expect.
	It works relative to type of pointer, eg int vs char pointers.
	*/

	bitmap = (unsigned char*) addr;
	perfect_numbers = (int *) (bitmap + bitmap_size);
	
	
	perfect_numbers[0] = 6;
	perfect_numbers[1] = 28;
	perfect_numbers[2] = 4096;
	perfect_numbers[3] = 8128;
	perfect_numbers[4] = 33550336;
	
	/*
	fprintf(stdout, "%d\n%d\n%d\n%d\n%d\n", 
		perfect_numbers[0],
		perfect_numbers[1],
		perfect_numbers[2],
		perfect_numbers[3],
		perfect_numbers[4]
	);
	*/
	if(shm_unlink("/merrickd_example") == -1){
		printf("Error deleting shared memory object");
		exit(EXIT_FAILURE);
	}
		

	return 0;
}
