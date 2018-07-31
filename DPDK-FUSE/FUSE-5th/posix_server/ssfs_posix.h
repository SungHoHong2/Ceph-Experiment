void sigchld_handler(int s){
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void *tcp_recv_launch(){

    printf("POSIX-RX BEGIN\n");

    int sockfd, new_fd, numbytes;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    char buf[PKT_SIZE];
    long int success = 0;
    long int tx_throughput;
    long int rx_throughput;
    const char clr[] = { 27, '[', '2', 'J', '\0' };
    const char topLeft[] = { 27, '[', '1', ';', '1', 'H','\0' };
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    fcntl(new_fd, F_SETFL, O_NONBLOCK);


    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }


    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
    }


    while(1) {  // main accept() loop

        char* data;
        struct message obj;
        struct fuse_message * e = NULL;
        struct message *msg;

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);

        success = recv(new_fd, buf, PKT_SIZE-1, 0);
        if(success && strlen(buf)>24){
            if(chara_debug) printf("recv msg from POSIX: %s\n", buf);
            pthread_mutex_lock(&rx_lock);
            if(strcmp(buf, "Hello World From CLIENT!\n")==0) {
                e = malloc(sizeof(struct fuse_message));
                strcpy(e->data, buf);
                TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
            }
            pthread_mutex_unlock(&rx_lock);
        }

    }

}



void *tcp_send_launch(){


    sleep(5);
    printf("POSIX-TX BEGIN\n");

    int sockfd, numbytes, new_fd;
    char recv_data[PKT_SIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    socklen_t sin_size;
    long int success = 0;
    FILE * nic_file;
    char nic_str[100];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // get information of the WORKSTATION
//    if ((rv = getaddrinfo("10.218.104.170", PORT, &hints, &servinfo)) != 0) {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
//    }

    if ((rv = getaddrinfo("10.107.30.34", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure

    while(1){

        char* data;
        struct message obj;
        struct fuse_message * e = NULL;
        struct message *msg;
        void* ad = NULL;
        char* aligned_buf_r = NULL;
        int fd, nr;
        struct message** msg_objs;
        struct message** msg_obj;


        while(TAILQ_EMPTY(&fuse_rx_queue)){}

        pthread_mutex_lock(&rx_lock);
        if(!TAILQ_EMPTY(&fuse_rx_queue)) {
            e = TAILQ_FIRST(&fuse_rx_queue);
            TAILQ_REMOVE(&fuse_rx_queue, e, nodes);
            msg = &obj;

            if( cache_miss == 1 ) {
                strncpy(obj.data, "Hello World From SERVER!\n", 64);
                data = (char*)&obj;
                memcpy(data, msg, sizeof(struct message));
                success = send(sockfd, data, PKT_SIZE, 0);
                if (success && strlen(data) > 0) {
                    if(chara_debug) printf("send msg in POSIX: %s\n",msg->data);
                }
            } else {
                int c;
                if (posix_memalign(&ad, SECTOR, PKT_SIZE * MERGE_PACKETS )) {
                    perror("posix_memalign failed"); exit (EXIT_FAILURE);
                }

                aligned_buf_r = (char *)(ad);
                fd = open(raw_device, O_RDWR | O_DIRECT);
                nr = pread(fd, aligned_buf_r, PKT_SIZE * MERGE_PACKETS, 0);
                close(fd);
                if(chara_debug) printf("\t aligned_buf_r::%ld\n",strlen(aligned_buf_r));

                char *test_buff = malloc(MERGE_PACKETS * PKT_SIZE * sizeof(char));
                msg_objs = malloc(MERGE_PACKETS * sizeof(struct message*));
                for(int i=0; i<MERGE_PACKETS; i++){
                    msg_objs[i] = malloc( sizeof(struct message));
                    memcpy(msg_objs[i]->data, aligned_buf_r, PKT_SIZE);
                    if(chara_debug) printf("split msg in POSIX: %ld\n", strlen(msg_objs[i]->data));
                    aligned_buf_r+=PKT_SIZE;

                    strcat(test_buff, msg_objs[i]->data);
                    if(chara_debug) printf("merge msg in POSIX: %ld\n", strlen(test_buff));
                }

                strncpy(obj.data, test_buff, DATA_SIZE);
                data = (char*)&obj;
                if (data != NULL) {
                    memcpy(data, msg, sizeof(struct message));
                    success = send(sockfd, data, DATA_SIZE, 0);
                    if (success && strlen(data) > 0) {
                        if(chara_debug) printf("send msg in POSIX: %ld\n",strlen(msg->data));
                    }
                }
            }

        }
        pthread_mutex_unlock(&rx_lock);

    }
}