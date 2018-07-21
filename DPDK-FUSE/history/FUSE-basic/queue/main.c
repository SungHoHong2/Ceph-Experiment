#include <stdio.h>
#include <unistd.h>
#include <sys/queue.h>

#include <stdlib.h>
#include <string.h>


struct node
{
    char message[1024];
    TAILQ_ENTRY(node) nodes;
};

int main (int arc, char * argv [])
{
    // This macro creates the data type for the head of the queue
    // for nodes of type 'struct node'
    TAILQ_HEAD(head_s, node) head;


    // Initialize the head before use
    TAILQ_INIT(&head);

    //
    // put some nodes in the queue
    //

    struct node * e = NULL;
    int c = 0;

    e = malloc(sizeof(struct node));
    strcpy(e->message, "howdy1");
    TAILQ_INSERT_TAIL(&head, e, nodes);

    e = malloc(sizeof(struct node));
    strcpy(e->message, "howdy2");
    TAILQ_INSERT_TAIL(&head, e, nodes);


    e = malloc(sizeof(struct node));
    strcpy(e->message, "howdy3");
    TAILQ_INSERT_TAIL(&head, e, nodes);


    e = malloc(sizeof(struct node));
    strcpy(e->message, "howdy4");
    TAILQ_INSERT_TAIL(&head, e, nodes);


    // free the elements from the queue
    while (!TAILQ_EMPTY(&head))
    {
        e = TAILQ_FIRST(&head);
        printf("%s\n", e->message);
        TAILQ_REMOVE(&head, e, nodes);
        free(e);
        e = NULL;
    }


    e = malloc(sizeof(struct node));
    strcpy(e->message, "howdy5");
    TAILQ_INSERT_TAIL(&head, e, nodes);


    e = malloc(sizeof(struct node));
    strcpy(e->message, "howdy6");
    TAILQ_INSERT_TAIL(&head, e, nodes);


    // print the queue
    TAILQ_FOREACH(e, &head, nodes)
    {
        printf("%s\n", e->message);
    }


    int cc;
    FILE *file;
    file = fopen("test.txt", "r");
    if (file) {
        while ((cc = getc(file)) != EOF)
            printf("%c",(char)cc);

        fclose(file);
    }


    return EXIT_SUCCESS;
}