#define TOTAL_TEST_REQ 100

static int do_getattr( const char *path, struct stat *st )
{

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

struct avg_node *av = NULL;

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    // printf( "--> Trying to read %s, %lu, %lu\n", path, offset, size );

    char client[] = "Hello World From CLIENT!\n";
    char server[] = "Hello World From SERVER!\n";
    char *selectedText = NULL;
    struct fuse_message *e = NULL;

    // ... //

    if ( strcmp( path, "/client" ) == 0 ) {
        selectedText = client;
        pthread_mutex_lock(&tx_lock);
        e = malloc(sizeof(struct fuse_message));
        strcpy(e->data, selectedText);
        sleep(0);

        TAILQ_INSERT_TAIL(&fuse_tx_queue, e, nodes);
        // printf("send msg in FUSE: %s\n", e->data);
        av = malloc(sizeof(struct avg_node));
        av->start_time = getTimeStamp();


        pthread_mutex_unlock(&tx_lock);


    } else if ( strcmp( path, "/server" ) == 0 )
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
    char *buffer = NULL;
    int rtn;

    while(1) {
        int c;
        pthread_mutex_lock(&rx_lock);

        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
            e = TAILQ_FIRST(&fuse_rx_queue);
            total_requests++;

            printf("recv msg in FUSE: %s :: %d\n", e->data, total_requests);
            av->end_time = getTimeStamp();
            av->interval = av->end_time - av->start_time;
            // printf("%ld\n",av->interval);


            TAILQ_INSERT_TAIL(&avg_queue, av, nodes);

            if(total_requests>TOTAL_TEST_REQ){
                avg_results();
            }


            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
            free(e);
            e = NULL;
        }
        pthread_mutex_unlock(&rx_lock);
    }
}
