


//static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
//                    struct fuse_file_info *fi)
//{
//    int fd;
//    int res;
//    unsigned long long roffset;
//    char client[] = "Hello World From CLIENT!\n";
//    char *selectedText = NULL;
//    struct fuse_message *e = NULL;
//    void *msg;
//    struct message *_msg;
//
//    selectedText = client;
//    _msg = malloc(sizeof(struct message));
//
//    pthread_mutex_lock(&tx_lock);
//        e = malloc(sizeof(struct fuse_message));
//        strcpy(e->data, selectedText);
//        TAILQ_INSERT_TAIL(&fuse_tx_queue, e, nodes);
//        av = malloc(sizeof(struct avg_node));
//        av->start_time = getTimeStamp();
//        av->num = total_requests;
//        if(chara_debug) printf("[%ld] send msg in FUSE: %s\n", av->num, e->data);
//        TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
//        total_requests++;
//    pthread_mutex_unlock(&tx_lock);
//
//    while(TAILQ_EMPTY(&fuse_rx_queue));
//
//    pthread_mutex_lock(&rx_lock);
//    if(!TAILQ_EMPTY(&fuse_rx_queue)) {
//        e = TAILQ_FIRST(&fuse_rx_queue);
//        strcpy(_msg->data, e->data);
//        test_i++;
//        TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
//
//        av = TAILQ_FIRST(&avg_queue);
//        av->end_time = getTimeStamp();
//        av->interval = av->end_time - av->start_time;
//        if(chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(e->data), av->interval);
//        intervals[test_i] = (double)av->interval;
//        TAILQ_REMOVE(&avg_queue, av, nodes);
//        free(e);
//        e = NULL;
//        free(av);
//        av = NULL;
//    }
//
//    pthread_mutex_unlock(&rx_lock);
//
//
//    if(total_requests==max_loop){
//        calculateSD(intervals);
//    }
//
//
//    if(cache_miss==1){
//
//        void *rbuf;
//        res = posix_memalign(&rbuf, SECTOR, PKT_SIZE*4);
//
//        fd = open("/data1/sungho/trash/one_gig_example", O_RDWR | O_DIRECT);
//        if (fd < 0) {
//            printf("Open error\n");
//            return -1;
//        }
//
//        roffset = ((rand() % (1024 * 1024 * 1024)) / SECTOR) * SECTOR;
//        if (fd) {
//            res = pread(fd, rbuf, PKT_SIZE * 4, roffset);
//            if (res < 0 || res == 0) {
//                printf("Read error %d\n", res);
//                return 0;
//            }
//            if(chara_debug) printf("recv msg in offset: %llu in FUSE\n", roffset);
//        }
//        close(fd);
//
//        strcpy(buf,_msg->data);
//        res = 26;
//    }
//
//    if(cache_hit==1 || cache_compact == 1){
//        strcpy(buf,_msg->data);
//        res = 26;
//    }
//
//    return res;
//}


uint64_t start_time, end_time;
int test_i=0;
double intervals[99999];

void calculateSD(double data[])
{
    float sum = 0.0, mean, standardDeviation = 0.0;
    int i;
    for(i=0; i<max_loop; ++i)
    {
        sum += data[i];
    }

    mean = sum/max_loop;
    printf("mean: %f\n",mean);


    for(i=0; i<max_loop; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    printf("std: %f\n",sqrt(standardDeviation/max_loop));
}

static void lo_read(fuse_req_t req, fuse_ino_t ino, size_t size,
                    off_t offset, struct fuse_file_info *fi)
{





    struct fuse_bufvec buf = FUSE_BUFVEC_INIT(size);
    buf.buf[0].flags = FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK;
    buf.buf[0].fd = fi->fh;
    buf.buf[0].pos = offset;
    fuse_reply_data(req, &buf, FUSE_BUF_SPLICE_MOVE);
}
