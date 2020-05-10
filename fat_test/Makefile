default:
	rm -f test.fat
	cp ../app/hello/hello.elf fd
	mformat -t 256 -h 1 -s 64 -C -i test.fat ::
	mcopy -i test.fat fd/* ::

test:
	gcc -std=c11 -o fat_test *.c
	./fat_test

