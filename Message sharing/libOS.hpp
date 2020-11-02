#ifndef LIBOS_HPP
#define LIBOS_HPP

#include <sys/types.h>

#include <string>

constexpr std::size_t MSGSZ = 1024;

struct msgbuf_t
{
    long mtype;
    char mtext[MSGSZ];
};

int create_or_open_mq();

std::string capture_cmd_out(const std::string& cmd);

void delete_mq(int mq_id);

#endif // LIBOS_HPP
