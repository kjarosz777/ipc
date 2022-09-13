#include <unistd.h>
#include <fstream>

#include <signal.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../msgBuffer.hpp"

int queueid;

void signalHandler(int s){
    printf("Caught signal %d\n",s);

    // to destroy the message queue
    msgctl(queueid, IPC_RMID, NULL);

    exit(EXIT_FAILURE);
}
  
int main()
{
    signal(SIGINT, signalHandler);

    // ftok to generate unique key
    key_t key = ftok("../msgq", 65);
    if (key == -1)
    {
        printf("Cannot create msg queue for '../msgq'");
        return EXIT_FAILURE;
    }
  
    // msgget creates a message queue and returns identifier
    queueid = msgget(key, 0666 | IPC_CREAT);

    printf("Msg queue created, key: %d , queueid: %d\n", key, queueid);

    mesg_buffer message;
    
    // msgrcv to receive message
    while(msgrcv(queueid, &message, sizeof(message.mesg_text), 1, 0) != -1)
    {
        // display the message
        printf("RECEIVED MSG: %s\n", message.mesg_text);
    }
  
    printf("Queue killed, ending receiver\n");
    return 0;
}
