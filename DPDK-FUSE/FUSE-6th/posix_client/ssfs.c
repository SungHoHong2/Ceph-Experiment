#include "ssfs_common.h"
#include "ssfs_posix.h"
#include "ssfs_fuse.h"
#include "ssfs_fuse_else.h"


int main( int argc, char **argv )
{

    printf("CHECKING ARGUEMENTS\n");
    if(argc>1){
        int i =0;
        for(i=0;i<argc; i++){
            if(strcmp(argv[i],"DEBUG")==0){
                chara_debug = 1;
            }
            if(strcmp(argv[i],"CACHE_HIT")==0){
                cache_hit = 1;
            }
            if(strcmp(argv[i],"CACHE_MISS")==0){
                cache_miss = 1;
            }

            if(strcmp(argv[i],"CACHE_COMPACT")==0){
                cache_compact = 1;
            }

            if(strcmp(argv[i],"TEN")==0){
                max_loop = 10;
            }
            if(strcmp(argv[i],"HUNDRED")==0){
                max_loop = 100;
            }
            if(strcmp(argv[i],"THOUSAND")==0){
                max_loop = 1000;
            }

        }
    }

    TAILQ_INIT(&avg_queue);
    TAILQ_INIT(&posix_queue);
    TAILQ_INIT(&fuse_tx_queue);
    TAILQ_INIT(&fuse_rx_queue);

    if (pthread_mutex_init(&tx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    if (pthread_mutex_init(&rx_lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    printf("FUSE-POSIX BEGIN\n");
    pthread_t threads[2];
    int rc = pthread_create(&threads[0], NULL, tcp_send_launch, NULL);
        rc = pthread_create(&threads[1], NULL, tcp_recv_launch, NULL);


    printf("FUSE-POSIX-CLIENT BEGIN");
    if(cache_hit == 1) printf(" WITH CACHE_HIT");
    if(max_loop == 10) printf(" 10 REQ");
    if(max_loop == 100) printf(" 100 REQ");

    umask(0);
    int fuse_argc = 3;
    char** fuse_argv;
    fuse_argv = malloc(fuse_argc * sizeof(char*));
    for (size_t i = 0; i < fuse_argc; i += 1)
        fuse_argv[i] = malloc(255 * sizeof(char));

    fuse_argv[0]="./ssfs";
    fuse_argv[1]="-f";
    fuse_argv[2]= argv[2];
//    fuse_main(fuse_argc, fuse_argv, &xmp_oper, NULL);
//    printf("FUSE-CLIENT END\n");

    struct fuse_args args = FUSE_ARGS_INIT(fuse_argc, fuse_argv);
    struct fuse_session *se;
    struct fuse_cmdline_opts opts;
    struct lo_data lo = { .debug = 0,
            .writeback = 0 };
    int ret = -1;

    lo.root.next = lo.root.prev = &lo.root;
    lo.root.fd = -1;

    if (fuse_parse_cmdline(&args, &opts) != 0)
        return 1;
    if (opts.show_help) {
        printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
        fuse_cmdline_help();
        fuse_lowlevel_help();
        ret = 0;
        goto err_out1;
    } else if (opts.show_version) {
        printf("FUSE library version %s\n", fuse_pkgversion());
        fuse_lowlevel_version();
        ret = 0;
        goto err_out1;
    }

    if (fuse_opt_parse(&args, &lo, lo_opts, NULL)== -1)
        return 1;

    lo.debug = opts.debug;
    lo.root.fd = open("/", O_PATH);
    lo.root.nlookup = 2;
    if (lo.root.fd == -1)
        err(1, "open(\"/\", O_PATH)");

    se = fuse_session_new(&args, &lo_oper, sizeof(lo_oper), &lo);
    if (se == NULL)
        goto err_out1;

    if (fuse_set_signal_handlers(se) != 0)
        goto err_out2;

    if (fuse_session_mount(se, opts.mountpoint) != 0)
        goto err_out3;

    fuse_daemonize(opts.foreground);

    if (opts.singlethread)
        ret = fuse_session_loop(se);
    else
        ret = fuse_session_loop_mt(se, opts.clone_fd);

    fuse_session_unmount(se);
    err_out3:
    fuse_remove_signal_handlers(se);
    err_out2:
    fuse_session_destroy(se);
    err_out1:
    free(opts.mountpoint);
    fuse_opt_free_args(&args);

    while (lo.root.next != &lo.root)
        lo_free(lo.root.next);
    if (lo.root.fd >= 0)
        close(lo.root.fd);

    return ret ? 1 : 0;
}