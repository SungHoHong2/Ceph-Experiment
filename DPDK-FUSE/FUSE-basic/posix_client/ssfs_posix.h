#define PORT "1234"  // the port users will be connecting to
#define PKT_SIZE 1024 // max number of bytes we can get at once
#define BACKLOG 10     // how many pending connections queue will hold

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *tcp_msg_launch(){

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


    // get information of the server
    if ((rv = getaddrinfo("10.218.111.252", PORT, &hints, &servinfo)) != 0) {
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
        char send_data[PKT_SIZE];
        memset( send_data, '*', PKT_SIZE * sizeof(char));
        success=send(sockfd, send_data, PKT_SIZE, 0);

        if(success && strlen(send_data)>0){
            printf("sent something!");
        }

        success=recv(sockfd, recv_data, PKT_SIZE-1, 0);

        if(success && strlen(recv_data)>0){
           printf("received something!");
        }
    }
}