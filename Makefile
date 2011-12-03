lib:
	gcc -w -I../../adt -I../indexingbasics -c -fPIC reader_nsf.c store.c 
	gcc -shared -o libnsfreader.so.1 reader_nsf.o store.o
installlib:
	sudo cp libnsfreader.so.1 /opt/lib
	sudo ln -sf /opt/lib/libnsfreader.so.1 /opt/lib/libnsfreader.so
