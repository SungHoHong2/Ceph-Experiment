#define PORT "1234"  // the port users will be connecting to
#define PKT_SIZE 1024 // max number of bytes we can get at once
#define BACKLOG 10     // how many pending connections queue will hold

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


void *tcp_msg_launch(){

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
//    fcntl(new_fd, F_SETFL, O_NONBLOCK);


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


        printf("step1\n");

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);

        success = recv(new_fd, buf, PKT_SIZE-1, 0);
        if(success && strlen(buf)>24){
            printf("recv msg from POSIX: %s\n", buf);
            pthread_mutex_lock(&rx_lock);
            if(strcmp(buf, "Hello World From CLIENT!\n")==0) {
                e = malloc(sizeof(struct fuse_message));
                strcpy(e->data, buf);
                TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
            }
            pthread_mutex_unlock(&rx_lock);
        }


//        while ( (success = read(new_fd, buf, PKT_SIZE-1) > 0))
//        {
//            if(success && strlen(buf)>24) {
//                printf("recv msg in POSIX: %s\n", buf);
//
//                pthread_mutex_lock(&rx_lock);
//                if(strcmp(buf, "Hello World From CLIENT!\n")==0) {
//                    e = malloc(sizeof(struct fuse_message));
//                    strcpy(e->data, buf);
//                    TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
//                }
//                pthread_mutex_unlock(&rx_lock);
//
//                break;
//            }
//        }



        success = send(new_fd, buf, PKT_SIZE, 0);
        if(success && strlen(buf)>24){
            printf("send msg from POSIX: %s\n", buf);
        }



//        while ( (success = read(new_fd, buf, PKT_SIZE-1) > 0)) {
////            success = recv(new_fd, buf, PKT_SIZE - 1, 0);
//            if (success && strlen(buf) > 24) {
//                printf("recv msg from POSIX: %s\n", buf);
//
//                struct message *msg = (struct message *) buf;
//                pthread_mutex_lock(&rx_lock);
//
//                printf("step1-2\n");
//                if (strlen(msg->data) >= 24 && strcmp(msg->data, "Hello World From CLIENT!\n") == 0) {
//                    // fprintf(f, "recv msg in DPDK: %s\n", msg->data);
//                    e = malloc(sizeof(struct fuse_message));
//                    strcpy(e->data, msg->data);
//                    TAILQ_INSERT_TAIL(&fuse_rx_queue, e, nodes);
//                    // fflush(f);
//                }
//                pthread_mutex_unlock(&rx_lock);
//            }
//        }

//        printf("step2\n");
//
//        while(TAILQ_EMPTY(&fuse_tx_queue)){};
//
//        pthread_mutex_lock(&tx_lock);
//        if(!TAILQ_EMPTY(&fuse_tx_queue)) {
//            e = TAILQ_FIRST(&fuse_tx_queue);
//            msg = &obj;
//            strncpy(obj.data, e->data, 100);
//            data = (char*)&obj;
//
//            if (data != NULL)
//                memcpy(data, msg, sizeof(struct message));
//
//            success=send(sockfd, data, PKT_SIZE, 0);
//            if(success && strlen(data)>0){
//                printf("send msg in POSIX: %s\n",e->data);
//            }
//
//            TAILQ_REMOVE(&fuse_tx_queue, e, nodes);
//        }
//        pthread_mutex_unlock(&tx_lock);
//
//        printf("step3\n");


    }

}