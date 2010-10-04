;CFLAGS = `pkg-config fuse --cflags --libs`
CFLAGSFUSE = -lfuse -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 
CFLAGSGLIB = -lglib-2.0
OBJECTS = init_descriptors.o main.o ecma.o iso_readdir.o log.o findinode.o file_read.o 
CC = gcc
TARGET = isomount
INCLUDEGLIB = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

isomount: $(OBJECTS)
	$(CC) $(CFLAGSFUSE) $(CFLAGSGLIB) $(INCLUDEGLIB) $(OBJECTS) -o $(TARGET) -g
main.o: iso_fs.h globals.h main.c
	$(CC) -c $(CFLAGSFUSE) $(CFLAGSGLIB) $(INCLUDEGLIB) main.c -g
init_descriptors.o: iso_fs.h globals.h init_descriptors.c 
	$(CC) -c init_descriptors.c -g
ecma.o: ecma.c
	$(CC) -c ecma.c -g
#log_pvd.o: iso_fs.h
#	$(CC) -c log_pvd.c -g
iso_readdir.o: iso_fs.h globals.h iso_readdir.c
	$(CC) -c $(CFLAGSFUSE) $(CFLAGSGLIB) $(INCLUDEGLIB) iso_readdir.c -g
log.o: log.c log.h params.h
	$(CC) -c $(CFLAGSFUSE) log.c -g
findinode.o: iso_fs.h globals.h findinode.c
	gcc -c findinode.c -lglib2.0 -I/usr/include/glib-2.0/ -I/usr/lib/glib-2.0/include/ -lfuse -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26
file_read.o: iso_fs.h globals.h file_read.c
	gcc -c file_read.c $(CFLAGSFUSE) $(CFLAGSGLIB) $(INCLUDEGLIB) 
clean:
	rm $(OBJECTS) -rvf
