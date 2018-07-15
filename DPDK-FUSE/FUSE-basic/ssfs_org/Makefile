COMPILER = gcc
FILESYSTEM_FILES = ssfs_0.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o ssfs_0 `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./ssfs_0 -f [mount point]'

clean:
	rm ssfs_0
