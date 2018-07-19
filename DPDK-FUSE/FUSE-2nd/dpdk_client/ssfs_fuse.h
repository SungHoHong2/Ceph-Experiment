
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


        printf("send msg in FUSE: %s\n", client);
        char *data;
        struct rte_mbuf *rm[1];
        rm[0] = rte_pktmbuf_alloc(test_pktmbuf_pool);
        data = rte_pktmbuf_append(rm[0], 30);
        memset(data, client, rte_pktmbuf_pkt_len(rm[0]));
        rte_prefetch0(rte_pktmbuf_mtod(rm[0], void *));
        l2fwd_mac_updating(rm[0], 0);
        rte_eth_tx_burst(0, 0, rm, 1);
        rte_pktmbuf_free(rm[0]);



//        struct rte_mbuf *rm[1];
//        char* data;
//        struct message obj;
//        struct fuse_message * e = NULL;
//        struct message *msg;
//
//            msg = &obj;
//            strncpy(obj.data, client, 100);
//            printf("send msg in DPDK: %s\n",e->data);
//            data = rte_pktmbuf_append(pkts_burst[0], sizeof(struct message));
//
//            if (data != NULL)
//                rte_memcpy(data, msg, sizeof(struct message));
//
//            rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[0], void *));
//            l2fwd_mac_updating(pkts_burst[0], portid);
//            rte_eth_tx_burst(portid, 0, pkts_burst, 1);



//        selectedText = client;
//        pthread_mutex_lock(&tx_lock);
//        e = malloc(sizeof(struct fuse_message));
//        strcpy(e->data, selectedText);
//        TAILQ_INSERT_TAIL(&fuse_tx_queue, e, nodes);
//        printf("send msg in FUSE: %s\n", e->data);
//
//
//        av = malloc(sizeof(struct avg_node));
//        av->start_time = getTimeStamp();
//        pthread_mutex_unlock(&tx_lock);


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
        // printf("fuse_rx_queue checking : %d\n", TAILQ_EMPTY(&fuse_rx_queue));
        pthread_mutex_lock(&rx_lock);
        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
            e = TAILQ_FIRST(&fuse_rx_queue);
            total_requests++;

            printf("recv msg in FUSE: %s :: %d\n", e->data, total_requests);
            av->end_time = getTimeStamp();
            av->interval = av->end_time - av->start_time;
            TAILQ_INSERT_TAIL(&avg_queue, av, nodes);

            if(total_requests>2){
                avg_results();
            }

            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
            free(e);
            e = NULL;
        }
        pthread_mutex_unlock(&rx_lock);
    }
}
