
static int do_getattr( const char *path, struct stat *st )
{
    // printf( "[getattr] Called\n" );
    // printf( "\tAttributes of %s requested\n", path );

    st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
    st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

    if ( strcmp( path, "/" ) == 0 )
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
    }
    else
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1024;
    }

    return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    // printf( "--> Getting The List of Files of %s\n", path );

    filler( buffer, ".", NULL, 0 ); // Current Directory
    filler( buffer, "..", NULL, 0 ); // Parent Directory

    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
    {
        filler( buffer, "client", NULL, 0 );
        filler( buffer, "server", NULL, 0 );
    }

    return 0;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    // printf( "--> Trying to read %s, %lu, %lu\n", path, offset, size );

    char client[] = "Hello World From CLIENT!\n";
    char server[] = "Hello World From SERVER!\n";
    char *selectedText = NULL;

    // ... //

    if ( strcmp( path, "/client" ) == 0 )
        selectedText = client;
    else if ( strcmp( path, "/server" ) == 0 )
        selectedText = server;
    else
        return -1;

    memcpy( buffer, selectedText + offset, size );
    return strlen( selectedText ) - offset;
}

static struct fuse_operations operations = {
        .getattr	= do_getattr,
        .readdir	= do_readdir,
        .read		= do_read,
};


void *fuse_rx_launch() {

    printf("FUSE-RX BEGIN\n");
    struct fuse_message * e = NULL;
    struct fuse_message * txe = NULL;
    FILE * file;
    char *buffer = NULL;
    int rtn;

    while(1) {
        int c;
        FILE *file;
        pthread_mutex_lock(&rx_lock);
        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
            e = TAILQ_FIRST(&fuse_rx_queue);
            // printf("recv msg in FUSE: %s\n", e->data);
            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
            free(e);
            e = NULL;
            file = fopen("/mnt/ssd_cache/test/server", "r");
            if (file) {
                pthread_mutex_lock(&tx_lock);
                txe = malloc(sizeof(struct fuse_message));
                rtn = fread(txe->data, sizeof(char), 1024, file);
                // printf("send msg in FUSE: %s\n", txe->data);
                TAILQ_INSERT_TAIL(&fuse_tx_queue, txe, nodes);
                pthread_mutex_unlock(&tx_lock);
                fclose(file);
            }

        }
        pthread_mutex_unlock(&rx_lock);
    }
}
