uint64_t start_time, end_time;
int test_i=0;
double intervals[99999];

void calculateSD(double data[])
{
    float sum = 0.0, mean, standardDeviation = 0.0;
    int i;
    for(i=0; i<MAX_LOOP; ++i)
    {
        sum += data[i];
    }

    mean = sum/MAX_LOOP;
    printf("mean: %f\n",mean);


    for(i=0; i<MAX_LOOP; ++i)
        standardDeviation += pow(data[i] - mean, 2);

    printf("std: %f\n",sqrt(standardDeviation/MAX_LOOP));
}



static void *xmp_init(struct fuse_conn_info *conn,
                      struct fuse_config *cfg)
{
    (void) conn;
    cfg->use_ino = 1;

    /* Pick up changes from lower filesystem right away. This is
       also necessary for better hardlink support. When the kernel
       calls the unlink() handler, it does not know the inode of
       the to-be-removed entry and can therefore not invalidate
       the cache of the associated inode - resulting in an
       incorrect st_nlink value being reported for any remaining
       hardlinks to this inode. */
    cfg->entry_timeout = 0;
    cfg->attr_timeout = 0;
    cfg->negative_timeout = 0;

    return NULL;
}

static int xmp_getattr(const char *path, struct stat *stbuf,
                       struct fuse_file_info *fi)
{
    (void) fi;
    int res;

    res = lstat(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_access(const char *path, int mask)
{
    int res;

    res = access(path, mask);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
    int res;

    res = readlink(path, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi,
                       enum fuse_readdir_flags flags)
{
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;
    (void) flags;

    dp = opendir(path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;

    /* On Linux this could just be 'mknod(path, mode, rdev)' but this
       is more portable */
    if (S_ISREG(mode)) {
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(path, mode);
    else
        res = mknod(path, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;

    res = mkdir(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_unlink(const char *path)
{
    int res;

    res = unlink(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;

    res = rmdir(path);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
    int res;

    res = symlink(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rename(const char *from, const char *to, unsigned int flags)
{
    int res;

    if (flags)
        return -EINVAL;

    res = rename(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_link(const char *from, const char *to)
{
    int res;

    res = link(from, to);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_chmod(const char *path, mode_t mode,
                     struct fuse_file_info *fi)
{
    (void) fi;
    int res;

    res = chmod(path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid,
                     struct fuse_file_info *fi)
{
    (void) fi;
    int res;

    res = lchown(path, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_truncate(const char *path, off_t size,
                        struct fuse_file_info *fi)
{
    int res;

    if (fi != NULL)
        res = ftruncate(fi->fh, size);
    else
        res = truncate(path, size);
    if (res == -1)
        return -errno;

    return 0;
}

#ifdef HAVE_UTIMENSAT
static int xmp_utimens(const char *path, const struct timespec ts[2],
		       struct fuse_file_info *fi)
{
	(void) fi;
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int xmp_create(const char *path, mode_t mode,
                      struct fuse_file_info *fi)
{
    int res;

    res = open(path, fi->flags, mode);
    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    int res;

    res = open(path, fi->flags);
    if (res == -1)
        return -errno;

    fi->fh = res;
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
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
        if(CHARA_DEBUG) printf("[%ld] send msg in FUSE: %s\n", av->num, _msg->data);
        TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
        total_requests++;
    }


    int collect_packets = 1;
    // char* aggregated = malloc(MERGE_PACKETS* PKT_SIZE * sizeof(char));

    if(CACHE_HIT==1) {
        while (1) {
            while (rte_ring_dequeue(rx_ring, &msg) < 0) {
                usleep(5);
            }
            _msg = (struct message *) msg;
            strcat(buf, _msg->data);
            // strcpy(collected_data[collect_packets], _msg->data);
            // printf("recv msg in FUSE: %ld\n", strlen(_msg->data));
            if(CHARA_DEBUG) printf("merge msg in FUSE: %ld\n", strlen(buf));
            collect_packets++;
            if (collect_packets > MERGE_PACKETS) break;
        }
    }

    else if(CACHE_MISS==1) {
        while(rte_ring_dequeue(rx_ring, &msg) < 0){
            usleep(5);
        }

        fd = open("/data1/sungho/trash/one_gig_example", O_RDWR | O_DIRECT);
        if (fd < 0) {
            printf("Open error\n");
            return -1;
        }
        roffset = ((rand() % (1024 * 1024 * 1024)) / SECTOR) * SECTOR;
        if (file) {
            res = pread(file, buf, DATA_SIZE, roffset);
            if (res < 0 || res == 0) {
                printf("Read error %d\n", res);
                return 0;
            }
            printf("recv msg in offset: %llu in FUSE\n", roffset);
            close(fd);
        }

        strcpy(buf,"MISS\n");
    }

    av = TAILQ_FIRST(&avg_queue);
    av->end_time = getTimeStamp();
    av->interval = av->end_time - av->start_time;
    if(CHARA_DEBUG) printf("[%ld] recv msg in FUSE: %ld :: %ld\n", av->num, strlen(buf), av->interval);
    intervals[test_i] = (double)av->interval;
    TAILQ_REMOVE(&avg_queue, av, nodes);
    free(av);
    test_i++;

    if(total_requests==MAX_LOOP){
        calculateSD(intervals);
    }

    res = 26;
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;

    (void) fi;
    if(fi == NULL)
        fd = open(path, O_WRONLY);
    else
        fd = fi->fh;

    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    if(fi == NULL)
        close(fd);
    return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
    int res;

    res = statvfs(path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_release(const char *path, struct fuse_file_info *fi)
{
    (void) path;
    close(fi->fh);
    return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi)
{
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) isdatasync;
    (void) fi;
    return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int xmp_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	if (mode)
		return -EOPNOTSUPP;

	if(fi == NULL)
		fd = open(path, O_WRONLY);
	else
		fd = fi->fh;

	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	if(fi == NULL)
		close(fd);
	return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int xmp_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
        .init           = xmp_init,
        .getattr	= xmp_getattr,
        .access		= xmp_access,
        .readlink	= xmp_readlink,
        .readdir	= xmp_readdir,
        .mknod		= xmp_mknod,
        .mkdir		= xmp_mkdir,
        .symlink	= xmp_symlink,
        .unlink		= xmp_unlink,
        .rmdir		= xmp_rmdir,
        .rename		= xmp_rename,
        .link		= xmp_link,
        .chmod		= xmp_chmod,
        .chown		= xmp_chown,
        .truncate	= xmp_truncate,
#ifdef HAVE_UTIMENSAT
        .utimens	= xmp_utimens,
#endif
        .open		= xmp_open,
        .create 	= xmp_create,
        .read		= xmp_read,
        .write		= xmp_write,
        .statfs		= xmp_statfs,
        .release	= xmp_release,
        .fsync		= xmp_fsync,
#ifdef HAVE_POSIX_FALLOCATE
        .fallocate	= xmp_fallocate,
#endif
#ifdef HAVE_SETXATTR
.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif
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
            if(CHARA_DEBUG) printf("[%ld] send msg in FUSE: %s\n", av->num, _msg->data);
            TAILQ_INSERT_TAIL(&avg_queue, av, nodes);
            total_requests++;
        }

    }

}
