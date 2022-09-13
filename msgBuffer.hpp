#ifndef MSGBUFFER_HPP_
#define MSGBUFFER_HPP_

const int BUFF_SIZE = 10;

// structure for message queue
struct mesg_buffer {
    long mesg_type;
    char mesg_text[BUFF_SIZE];
};

#endif // MSGBUFFER_HPP_