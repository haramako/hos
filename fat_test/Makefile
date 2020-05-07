default:
	rm -f test.fat
	mformat -t 256 -h 1 -s 64 -C -i test.fat ::
	mcopy -i test.fat hoge.txt fuga.txt ::

test:
	gcc -std=c11 -o fat_test *.c
	./fat_test

