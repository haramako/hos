typedef unsigned int size_t;
int write(int fd, const void *, size_t);
void exit(int);
void shutdown();

void _start(){
	write(1, "Hello, world!\n", 14);
	exit(0);
	//write(1, "exited!\n", 8);
	shutdown();
	for(;;);
}
