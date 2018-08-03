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


//
//
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
//    strcpy(_msg->data, selectedText);
//
//    if (rte_ring_enqueue(tx_ring, _msg) < 0) {
//        printf("Failed to send message - message discarded\n");
//    } else {
//        av = malloc(sizeof(struct avg_node));
//        av->start_time = getTimeStamp();
//        av->num = total_requests;
//        if(chara_debug) printf("[%ld] send msg in FUSE: %s\n", av->num, _msg->data);
//        TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
//        total_requests++;
//    }
//
//
//    int collect_packets = 1;
//    // char* aggregated = malloc(MERGE_PACKETS* PKT_SIZE * sizeof(char));
//
//    if(cache_hit==1) {
//        while (1) {
//            while (rte_ring_dequeue(rx_ring, &msg) < 0) {
//                usleep(5);
//            }
//            _msg = (struct message *) msg;
//            strcat(buf, _msg->data);
//            // strcpy(collected_data[collect_packets], _msg->data);
//            // printf("recv msg in FUSE: %ld\n", strlen(_msg->data));
//            if(chara_debug) printf("merge msg in FUSE: %ld\n", strlen(buf));
//            collect_packets++;
//            if (collect_packets > MERGE_PACKETS) break;
//        }
//
//        av = TAILQ_FIRST(&avg_queue);
//        av->end_time = getTimeStamp();
//        av->interval = av->end_time - av->start_time;
//        if(chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(buf), av->interval);
//        intervals[test_i] = (double)av->interval;
//        TAILQ_REMOVE(&avg_queue, av, nodes);
//        free(av);
//        test_i++;
//
//        if(total_requests==max_loop){
//            calculateSD(intervals);
//        }
//
//    }
//
//    else if(cache_miss==1) {
//        while(rte_ring_dequeue(rx_ring, &msg) < 0){
//            usleep(5);
//        }
//
//        _msg = (struct message *) msg;
//        if(chara_debug) printf("recv msg in FUSE: %ld\n", strlen(_msg->data));
//
//        av = TAILQ_FIRST(&avg_queue);
//        av->end_time = getTimeStamp();
//        av->interval = av->end_time - av->start_time;
//        if(chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(buf), av->interval);
//        intervals[test_i] = (double)av->interval;
//        TAILQ_REMOVE(&avg_queue, av, nodes);
//        free(av);
//        test_i++;
//
//        if(total_requests==max_loop){
//            calculateSD(intervals);
//        }
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
//        strcpy(buf,_msg->data);
//
//
//    } else if (cache_compact == 1){
//
//        while(rte_ring_dequeue(rx_ring, &msg) < 0){
//            usleep(5);
//        }
//
//        _msg = (struct message *) msg;
//        if(chara_debug) printf("recv msg in FUSE: %ld\n", strlen(_msg->data));
//
//        av = TAILQ_FIRST(&avg_queue);
//        av->end_time = getTimeStamp();
//        av->interval = av->end_time - av->start_time;
//        if(chara_debug) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(buf), av->interval);
//        intervals[test_i] = (double)av->interval;
//        TAILQ_REMOVE(&avg_queue, av, nodes);
//        free(av);
//        test_i++;
//
//        if(total_requests==max_loop){
//            calculateSD(intervals);
//        }
//        strcpy(buf,_msg->data);
//    }
//    res = 26;
//    return res;
//}


struct fuse_out_header {
    uint32_t	len;
    int32_t		error;
    uint64_t	unique;
};

/* We are re-using pointers to our `struct lo_inode` and `struct
   lo_dirp` elements as inodes. This means that we must be able to
   store uintptr_t values in a fuse_ino_t variable. The following
   incantation checks this condition at compile time. */
#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 6) && !defined __cplusplus
_Static_assert(sizeof(fuse_ino_t) >= sizeof(uintptr_t),
	       "fuse_ino_t too small to hold uintptr_t values!");
#else
struct _uintptr_to_must_hold_fuse_ino_t_dummy_struct \
	{ unsigned _uintptr_to_must_hold_fuse_ino_t:
            ((sizeof(fuse_ino_t) >= sizeof(uintptr_t)) ? 1 : -1); };
#endif

struct lo_inode {
    struct lo_inode *next;
    struct lo_inode *prev;
    int fd;
    ino_t ino;
    dev_t dev;
    uint64_t nlookup;
};

struct lo_data {
    int debug;
    int writeback;
    struct lo_inode root;
};

static const struct fuse_opt lo_opts[] = {
        { "writeback",
          offsetof(struct lo_data, writeback), 1 },
        { "no_writeback",
          offsetof(struct lo_data, writeback), 0 },
        FUSE_OPT_END
};

static struct lo_data *lo_data(fuse_req_t req)
{
    return (struct lo_data *) fuse_req_userdata(req);
}

static struct lo_inode *lo_inode(fuse_req_t req, fuse_ino_t ino)
{
    if (ino == FUSE_ROOT_ID)
        return &lo_data(req)->root;
    else
        return (struct lo_inode *) (uintptr_t) ino;
}

static int lo_fd(fuse_req_t req, fuse_ino_t ino)
{
    return lo_inode(req, ino)->fd;
}

static bool lo_debug(fuse_req_t req)
{
    return lo_data(req)->debug != 0;
}

static void lo_init(void *userdata,
                    struct fuse_conn_info *conn)
{
    struct lo_data *lo = (struct lo_data*) userdata;

    if(conn->capable & FUSE_CAP_EXPORT_SUPPORT)
        conn->want |= FUSE_CAP_EXPORT_SUPPORT;

    if (lo->writeback &&
        conn->capable & FUSE_CAP_WRITEBACK_CACHE) {
        if (lo->debug)
            fprintf(stderr, "lo_init: activating writeback\n");
        conn->want |= FUSE_CAP_WRITEBACK_CACHE;
    }
}

static void lo_getattr(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi)
{
    int res;
    struct stat buf;
    (void) fi;

    res = fstatat(lo_fd(req, ino), "", &buf, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1)
        return (void) fuse_reply_err(req, errno);

    fuse_reply_attr(req, &buf, 1.0);
}

static struct lo_inode *lo_find(struct lo_data *lo, struct stat *st)
{
    struct lo_inode *p;

    for (p = lo->root.next; p != &lo->root; p = p->next) {
        if (p->ino == st->st_ino && p->dev == st->st_dev)
            return p;
    }
    return NULL;
}

static int lo_do_lookup(fuse_req_t req, fuse_ino_t parent, const char *name,
                        struct fuse_entry_param *e)
{
    int newfd;
    int res;
    int saverr;
    struct lo_inode *inode;

    memset(e, 0, sizeof(*e));
    e->attr_timeout = 1.0;
    e->entry_timeout = 1.0;

    newfd = openat(lo_fd(req, parent), name, O_PATH | O_NOFOLLOW);
    if (newfd == -1)
        goto out_err;

    res = fstatat(newfd, "", &e->attr, AT_EMPTY_PATH | AT_SYMLINK_NOFOLLOW);
    if (res == -1)
        goto out_err;

    inode = lo_find(lo_data(req), &e->attr);
    if (inode) {
        close(newfd);
        newfd = -1;
    } else {
        struct lo_inode *prev = &lo_data(req)->root;
        struct lo_inode *next = prev->next;
        saverr = ENOMEM;
        inode = calloc(1, sizeof(struct lo_inode));
        if (!inode)
            goto out_err;

        inode->fd = newfd;
        inode->ino = e->attr.st_ino;
        inode->dev = e->attr.st_dev;

        next->prev = inode;
        inode->next = next;
        inode->prev = prev;
        prev->next = inode;
    }
    inode->nlookup++;
    e->ino = (uintptr_t) inode;

    if (lo_debug(req))
        fprintf(stderr, "  %lli/%s -> %lli\n",
                (unsigned long long) parent, name, (unsigned long long) e->ino);

    return 0;

    out_err:
    saverr = errno;
    if (newfd != -1)
        close(newfd);
    return saverr;
}

static void lo_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    struct fuse_entry_param e;
    int err;

    if (lo_debug(req))
        fprintf(stderr, "lo_lookup(parent=%" PRIu64 ", name=%s)\n",
            parent, name);

    err = lo_do_lookup(req, parent, name, &e);
    if (err)
        fuse_reply_err(req, err);
    else
        fuse_reply_entry(req, &e);
}

static void lo_free(struct lo_inode *inode)
{
    struct lo_inode *prev = inode->prev;
    struct lo_inode *next = inode->next;

    next->prev = prev;
    prev->next = next;
    close(inode->fd);
    free(inode);
}

static void lo_forget(fuse_req_t req, fuse_ino_t ino, uint64_t nlookup)
{
    struct lo_inode *inode = lo_inode(req, ino);

    if (lo_debug(req)) {
        fprintf(stderr, "  forget %lli %lli -%lli\n",
                (unsigned long long) ino, (unsigned long long) inode->nlookup,
                (unsigned long long) nlookup);
    }

    assert(inode->nlookup >= nlookup);
    inode->nlookup -= nlookup;

    if (!inode->nlookup)
        lo_free(inode);

    fuse_reply_none(req);
}

static void lo_readlink(fuse_req_t req, fuse_ino_t ino)
{
    char buf[PATH_MAX + 1];
    int res;

    printf("FRISK lo_readlink\n");


    res = readlinkat(lo_fd(req, ino), "", buf, sizeof(buf));
    if (res == -1)
        return (void) fuse_reply_err(req, errno);

    if (res == sizeof(buf))
        return (void) fuse_reply_err(req, ENAMETOOLONG);

    buf[res] = '\0';


    printf("probably the data is here: %s\n", buf);

    fuse_reply_readlink(req, buf);
}

struct lo_dirp {
    int fd;
    DIR *dp;
    struct dirent *entry;
    off_t offset;
};

static struct lo_dirp *lo_dirp(struct fuse_file_info *fi)
{
    return (struct lo_dirp *) (uintptr_t) fi->fh;
}

static void lo_opendir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    int error = ENOMEM;
    struct lo_dirp *d = calloc(1, sizeof(struct lo_dirp));
    if (d == NULL)
        goto out_err;

    d->fd = openat(lo_fd(req, ino), ".", O_RDONLY);
    if (d->fd == -1)
        goto out_errno;

    d->dp = fdopendir(d->fd);
    if (d->dp == NULL)
        goto out_errno;

    d->offset = 0;
    d->entry = NULL;

    fi->fh = (uintptr_t) d;
    fuse_reply_open(req, fi);
    return;

    out_errno:
    error = errno;
    out_err:
    if (d) {
        if (d->fd != -1)
            close(d->fd);
        free(d);
    }
    fuse_reply_err(req, error);
}

static void lo_do_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                          off_t offset, struct fuse_file_info *fi, int plus)
{
    struct lo_dirp *d = lo_dirp(fi);
    char *buf;
    char *p;
    size_t rem;
    int err;

    (void) ino;

    buf = calloc(1, size);
    if (!buf)
        return (void) fuse_reply_err(req, ENOMEM);

    if (offset != d->offset) {
        seekdir(d->dp, offset);
        d->entry = NULL;
        d->offset = offset;
    }
    p = buf;
    rem = size;
    while (1) {
        size_t entsize;
        off_t nextoff;

        if (!d->entry) {
            errno = 0;
            d->entry = readdir(d->dp);
            if (!d->entry) {
                if (errno && rem == size) {
                    err = errno;
                    goto error;
                }
                break;
            }
        }
        nextoff = telldir(d->dp);
        if (plus) {
            struct fuse_entry_param e;

            err = lo_do_lookup(req, ino, d->entry->d_name, &e);
            if (err)
                goto error;

            entsize = fuse_add_direntry_plus(req, p, rem,
                                             d->entry->d_name,
                                             &e, nextoff);
        } else {
            struct stat st = {
                    .st_ino = d->entry->d_ino,
                    .st_mode = d->entry->d_type << 12,
            };
            entsize = fuse_add_direntry(req, p, rem,
                                        d->entry->d_name,
                                        &st, nextoff);
        }
        if (entsize > rem)
            break;

        p += entsize;
        rem -= entsize;

        d->entry = NULL;
        d->offset = nextoff;
    }

    fuse_reply_buf(req, buf, size - rem);
    free(buf);
    return;

    error:
    free(buf);
    fuse_reply_err(req, err);
}

static void lo_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                       off_t offset, struct fuse_file_info *fi)
{
    lo_do_readdir(req, ino, size, offset, fi, 0);
}

static void lo_readdirplus(fuse_req_t req, fuse_ino_t ino, size_t size,
                           off_t offset, struct fuse_file_info *fi)
{
    lo_do_readdir(req, ino, size, offset, fi, 1);
}

static void lo_releasedir(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    struct lo_dirp *d = lo_dirp(fi);
    (void) ino;
    closedir(d->dp);
    free(d);
    fuse_reply_err(req, 0);
}

static void lo_create(fuse_req_t req, fuse_ino_t parent, const char *name,
                      mode_t mode, struct fuse_file_info *fi)
{
    int fd;
    struct fuse_entry_param e;
    int err;

    if (lo_debug(req))
        fprintf(stderr, "lo_create(parent=%" PRIu64 ", name=%s)\n",
            parent, name);

    fd = openat(lo_fd(req, parent), name,
                (fi->flags | O_CREAT) & ~O_NOFOLLOW, mode);
    if (fd == -1)
        return (void) fuse_reply_err(req, errno);

    fi->fh = fd;

    err = lo_do_lookup(req, parent, name, &e);
    if (err)
        fuse_reply_err(req, err);
    else
        fuse_reply_create(req, &e, fi);
}

static void lo_open(fuse_req_t req, fuse_ino_t ino,
                    struct fuse_file_info *fi)
{

    printf("FRISK lo_open\n");

    int fd;
    char buf[64];

    if (lo_debug(req))
        fprintf(stderr, "lo_open(ino=%" PRIu64 ", flags=%d)\n",
            ino, fi->flags);

    /* With writeback cache, kernel may send read requests even
       when userspace opened write-only */
    if (lo_data(req)->writeback &&
        (fi->flags & O_ACCMODE) == O_WRONLY) {
        fi->flags &= ~O_ACCMODE;
        fi->flags |= O_RDWR;
    }

    /* With writeback cache, O_APPEND is handled by the kernel.
       This breaks atomicity (since the file may change in the
       underlying filesystem, so that the kernel's idea of the
       end of the file isn't accurate anymore). In this example,
       we just accept that. A more rigorous filesystem may want
       to return an error here */
    if (lo_data(req)->writeback && (fi->flags & O_APPEND))
        fi->flags &= ~O_APPEND;

    sprintf(buf, "/proc/self/fd/%i", lo_fd(req, ino));
    fd = open(buf, fi->flags & ~O_NOFOLLOW);
    if (fd == -1)
        return (void) fuse_reply_err(req, errno);

    printf("read file directory: %s\n",buf);

    fi->fh = fd;
    fuse_reply_open(req, fi);
}

static void lo_release(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{

    printf("FRISK lo_release\n");
    (void) ino;

    close(fi->fh);
    fuse_reply_err(req, 0);
}




static void lo_read(fuse_req_t req, fuse_ino_t ino, size_t size,
                    off_t offset, struct fuse_file_info *fi)
{
    struct fuse_bufvec buf = FUSE_BUFVEC_INIT(size);

    printf( "lo_read(ino=%" PRIu64 ", size=%zd, "
                                   "off=%lu)\n", ino, size, (unsigned long) offset);

    buf.buf[0].flags = FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK;
    buf.buf[0].fd = fi->fh;
    buf.buf[0].pos = offset;


    printf("CHARA: size:%ld offset:%ld\n",size, buf.buf[0].pos);
    printf("CHARA: char: %s\n",(char *)buf.buf[0].mem);








    fuse_reply_data(req, &buf, FUSE_BUF_SPLICE_MOVE);

    printf("CHARA: size:%ld offset:%ld\n",size, buf.buf[0].size);
    printf("CHARA: char: %s\n",(char *)buf.buf[0].mem);
}


static struct fuse_lowlevel_ops lo_oper = {
        .init		= lo_init,
        .lookup		= lo_lookup,
        .forget		= lo_forget,
        .getattr	= lo_getattr,
        .readlink	= lo_readlink,
        .opendir	= lo_opendir,
        .readdir	= lo_readdir,
        .readdirplus	= lo_readdirplus,
        .releasedir	= lo_releasedir,
        .create		= lo_create,
        .open		= lo_open,
        .release	= lo_release,
        .read		= lo_read,
};


void *without_fuse_launch(){

    sleep(5);

    while(1){

        sleep(1);
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

    }

}
