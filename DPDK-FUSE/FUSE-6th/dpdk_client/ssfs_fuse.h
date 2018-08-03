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
    int fd;
    int res;
    unsigned long long roffset;
    char client[] = "Hello World From CLIENT!\n";
    char *selectedText = NULL;
    struct fuse_message *e = NULL;
    void *msg;
    struct message *_msg;



    selectedText = client;
    _msg = malloc(sizeof(struct message));
    strcpy(_msg->data, selectedText);

    if (rte_ring_enqueue(tx_ring, _msg) < 0) {
        printf("Failed to send message - message discarded\n");
    } else {
        av = malloc(sizeof(struct avg_node));
        av->start_time = getTimeStamp();
        av->num = total_requests;
        if(chara_debug) printf("[%ld] send msg in FUSE: %s\n", av->num, _msg->data);
        TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
        total_requests++;
    }

    int collect_packets = 1;
    char* aggregated = malloc(MERGE_PACKETS* PKT_SIZE * sizeof(char));

    if(cache_hit==1) {
        while (1) {
            while (rte_ring_dequeue(rx_ring, &msg) < 0) {
                usleep(5);
            }
            _msg = (struct message *) msg;
            strcat(aggregated, _msg->data);
            // strcpy(collected_data[collect_packets], _msg->data);
            // printf("recv msg in FUSE: %ld\n", strlen(_msg->data));
            if (chara_debug) printf("merge msg in FUSE: %ld\n", strlen(_msg->data));
            collect_packets++;
            if (collect_packets > MERGE_PACKETS) break;
        }

        av = TAILQ_FIRST(&avg_queue);
        av->end_time = getTimeStamp();
        av->interval = av->end_time - av->start_time;
        if (chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(aggregated), av->interval);
        intervals[test_i] = (double) av->interval;
        TAILQ_REMOVE(&avg_queue, av, nodes);
        free(av);
        test_i++;

        if (total_requests == max_loop) {
            calculateSD(intervals);
        }
        fuse_reply_buf(req, aggregated, strlen(aggregated));

    } else if(cache_miss==1) {
        while(rte_ring_dequeue(rx_ring, &msg) < 0){
            usleep(5);
        }

        _msg = (struct message *) msg;
        if(chara_debug) printf("recv msg in FUSE: %ld\n", strlen(_msg->data));

        av = TAILQ_FIRST(&avg_queue);
        av->end_time = getTimeStamp();
        av->interval = av->end_time - av->start_time;
        if(chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(_msg->data), av->interval);
        intervals[test_i] = (double)av->interval;
        TAILQ_REMOVE(&avg_queue, av, nodes);
        free(av);
        test_i++;

        if(total_requests==max_loop){
            calculateSD(intervals);
        }
        fuse_reply_buf(req, _msg->data, strlen(_msg->data));


    } else if (cache_compact == 1) {

        while(rte_ring_dequeue(rx_ring, &msg) < 0){
            usleep(5);
        }

        _msg = (struct message *) msg;
        if(chara_debug) printf("recv msg in FUSE: %ld\n", strlen(_msg->data));

        av = TAILQ_FIRST(&avg_queue);
        av->end_time = getTimeStamp();
        av->interval = av->end_time - av->start_time;
        if(chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(_msg->data), av->interval);
        intervals[test_i] = (double)av->interval;
        TAILQ_REMOVE(&avg_queue, av, nodes);
        free(av);
        test_i++;

        if(total_requests==max_loop){
            calculateSD(intervals);
        }
    }
}
