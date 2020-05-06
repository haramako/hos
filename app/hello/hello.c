typedef unsigned int size_t;
int write(int fd, const void *, size_t);
void exit(int);

void _start(){
	//for(;;);
	//*((int*)0x0000200000000000) = 1;
	write(1, "Hello, world!\n", 14);
	//*((int*)0x0000200000000000) = 1;
	//exit(0);
	for(;;);
}
