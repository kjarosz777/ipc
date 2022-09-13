#include <unistd.h>
#include <fstream>

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/inotify.h>
#include "../msgBuffer.hpp"

const int EVENT_SIZE = sizeof(inotify_event);
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

    const char* fileName = "toSend.txt";
    
    const int fd = inotify_init();
    if (inotify_add_watch(fd, fileName, IN_MODIFY) == -1)
    {
        fprintf(stderr, "Cannot watch '%s': %s\n", fileName, strerror(errno));
        return EXIT_FAILURE;
    }
    else
    {
        printf("Watching: '%s'\n", fileName);
    }
    
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
    message.mesg_type = 1;

    char buffer[128 * EVENT_SIZE];

    while(true)
    {
        printf("Waiting for changes in '%s'...\n", fileName);

        int length = read(fd, buffer, EVENT_SIZE);
        if (length < 0)
        {
            printf("error reading '%s', length: %d\n", fileName, length);
            return EXIT_FAILURE;
        }

        for (int i = 0; i < length; i += EVENT_SIZE)
        {
            inotify_event* event = reinterpret_cast<inotify_event*>(&buffer[i]);
            printf("Event(offset: %d, evenSize: %d): wd: %d, mask %u, len: %u\n",
                i, EVENT_SIZE, event->wd, event->mask, event->len);

            if (event->mask != IN_MODIFY)
            {
                printf("different mask detected in inotify_event: %d\n", event->mask);
                return EXIT_FAILURE;
            }

            std::ifstream myfile(fileName);
            if (!myfile.is_open())
            {
                printf("error opening '%s', is_open(): %d\n", fileName, myfile.is_open());
                return EXIT_FAILURE;
            }

            printf("Writing data to msg queue: \n");

            std::string line;
            while(std::getline(myfile, line))
            {
                strncpy(message.mesg_text, line.c_str(), BUFF_SIZE - 1); // leave last element...
                message.mesg_text[BUFF_SIZE - 1] = '\000'; // ...and add 'end' of string

                // msgsnd to send message
                if (msgsnd(queueid, &message, sizeof(message.mesg_text), 0) != -1)
                {
                    printf("SEND MSG: %s\n", message.mesg_text);
                }
                else
                {
                    printf("Queue killed, ending Sender\n");
                    return EXIT_FAILURE;
                }
            }
        }
    }
  
    return 0;
}
