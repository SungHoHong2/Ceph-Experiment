#define TOTAL_TEST_REQ 10


//static int do_getattr( const char *path, struct stat *st )
//{
//    // printf( "[getattr] Called\n" );
//    // printf( "\tAttributes of %s requested\n", path );
//
//    st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
//    st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
//    st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
//    st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
//
//    if ( strcmp( path, "/" ) == 0 )
//    {
//        st->st_mode = S_IFDIR | 0755;
//        st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
//    }
//    else
//    {
//        st->st_mode = S_IFREG | 0644;
//        st->st_nlink = 1;
//        st->st_size = 1024;
//    }
//
//    return 0;
//}
//
//static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
//{
//    // printf( "--> Getting The List of Files of %s\n", path );
//
//    filler( buffer, ".", NULL, 0 ); // Current Directory
//    filler( buffer, "..", NULL, 0 ); // Parent Directory
//
//    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
//    {
//        filler( buffer, "client", NULL, 0 );
//        filler( buffer, "server", NULL, 0 );
//    }
//
//    return 0;
//}
//
//
//static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
//{
//    // printf( "--> Trying to read %s, %lu, %lu\n", path, offset, size );
//
//    char client[] = "Hello World From CLIENT!\n";
//    char server[] = "Hello World From SERVER!\n";
//    char *selectedText = NULL;
//    struct fuse_message *e = NULL;
//
//    // ... //
//
//    if ( strcmp( path, "/client" ) == 0 ) {
//        selectedText = client;
//        pthread_mutex_lock(&tx_lock);
//        e = malloc(sizeof(struct fuse_message));
//        strcpy(e->data, selectedText);
//        TAILQ_INSERT_TAIL(&fuse_tx_queue, e, nodes);
//        // printf("send msg in FUSE: %s\n", e->data);
//        sleep(0);
//
//        av = malloc(sizeof(struct avg_node));
//        av->start_time = getTimeStamp();
//        pthread_mutex_unlock(&tx_lock);
//
//
//
//
//
//
//    } else if ( strcmp( path, "/server" ) == 0 )
//        selectedText = server;
//    else
//        return -1;
//
//    memcpy( buffer, selectedText + offset, size );
//    return strlen( selectedText ) - offset;
//}
//
//static struct fuse_operations operations = {
//        .getattr	= do_getattr,
//        .readdir	= do_readdir,
//        .read		= do_read,
//};


void *fuse_rx_launch() {
    printf("FUSE-RX BEGIN\n");
    struct fuse_message * e = NULL;
    struct fuse_message * txe = NULL;
    char *buffer = NULL;
    int rtn;
    void *msg;
    struct message *_msg;
    sleep(5);

    while(1) {


        if(total_requests<=TOTAL_TEST_REQ) {
            _msg = malloc(sizeof(struct message));
            strcpy(_msg->data, "Hello World From CLIENT!\n");

            if (rte_ring_enqueue(tx_ring, _msg) < 0) {
                printf("Failed to send message - message discarded\n");
            } else {
                av = malloc(sizeof(struct avg_node));
                av->start_time = getTimeStamp();
                av->num = total_requests;
                printf("[%ld] send msg in FUSE: %s\n", av->num, _msg->data);
                TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
                total_requests++;
            }


        }




//        pthread_mutex_lock(&tx_lock);
//        if(total_requests<=TOTAL_TEST_REQ) {
//            e = malloc(sizeof(struct fuse_message));
//            strcpy(e->data, "Hello World From CLIENT!\n");
//            TAILQ_INSERT_TAIL(&fuse_tx_queue, e, nodes);
//            av = malloc(sizeof(struct avg_node));
//            av->start_time = getTimeStamp();
//            av->num = total_requests;
//            printf("[%ld] send msg in FUSE: %s\n", av->num, e->data);
//            TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
//            total_requests++;
//        }
//        pthread_mutex_unlock(&tx_lock);


        if (rte_ring_dequeue(rx_ring, &msg) < 0){
            usleep(5);
        }else{
              _msg = (struct message *)msg;
              // printf("CHARA Received '%s'\n", _msg->data);
              av = TAILQ_FIRST(&avg_queue);
              av->end_time = getTimeStamp();
              av->interval = av->end_time - av->start_time;
              printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(_msg->data), av->interval);
              TAILQ_REMOVE(&avg_queue, av, nodes);
              free(av);
        }
    }
}


