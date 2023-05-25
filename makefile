

main: src/carousel.c src/fake_os.c src/fake_process.c src/linked_list.c src/scheduler.c 
	gcc -o main src/carousel.c src/fake_os.c src/fake_process.c src/linked_list.c src/scheduler.c 