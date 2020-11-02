#ifndef LIBOS_HPP
#define LIBOS_HPP

#include <sys/types.h>

#include <string>

constexpr std::size_t MSGSZ = 1024;
static constexpr std::size_t SHMEM_SIZE = 1 << 16;

int create_shmem();
int open_shmem();
int create_sem();
int open_sem();

key_t get_key_impl(const std::string& path);
key_t get_key_sem();
key_t get_key_mem();

void delete_shmem(int shm_id);
void delete_sem(int sem_id);

struct msgbuf_t
{
    long mtype;
    char mtext[MSGSZ];
};

int create_or_open_mq();

std::string capture_cmd_out(const std::string& cmd);

void delete_mq(int mq_id);

#endif // LIBOS_HPP
