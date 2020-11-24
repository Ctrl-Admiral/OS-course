#ifndef LIBOS_HPP
#define LIBOS_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/un.h>

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#define SIZE 256
//#define SOCKET_SERVER "sock"
//#define SOCKET_CLIENT "cl_sock"

constexpr std::size_t MSGSZ = 1024;
static constexpr std::size_t SHMEM_SIZE = 1 << 16;
using byte_t = char;
const std::string PATH_SERVER_SOCK = "/tmp/lab_sock";
const std::string PATH_CLIENT_SOCK = "/tmp/lab_sock1";

sockaddr_un get_addr_impl(std::string path);

//template <typename T>
//T read_data(const byte_t* from)
//{
//    T ans = 0;
//    for (std::size_t shift = 0; shift != sizeof(T) * 8; shift += 8, ++from)
//        ans |= static_cast<T>(*from) << shift;
//    return ans;
//}

//template <typename T>
//void write_data(T what, byte_t* where)
//{
//    T mask = 0xff;
//    for (std::size_t shift = 0; shift != sizeof(T) * 8; mask <<= 8, ++where, shift += 8)
//        *where = (what & mask) >> shift;
//}

int create_socket();
void close_socket(int sfd);

struct Socket
{
    Socket() : sfd(create_socket()) {}

    ~Socket()
    {
        try
        {
            close_socket(sfd);
#ifndef NDEBUG
            std::cout << "[DEBUG] ~Socket" << std::endl;
#endif
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    const int sfd;
};

sockaddr_un get_server_addr();
sockaddr_un get_client_addr();

void bind_wrapper(int sfd, const sockaddr_un& addr);

void sendto_str(int sfd, struct sockaddr_un addr, std::string msg);
std::string recvfrom_str(int sfd, struct sockaddr_un& addr);

int create_shmem();
int open_shmem();
int create_sem();
int open_sem();

key_t get_key_impl(const std::string& path);
key_t get_key_sem();
key_t get_key_mem();

byte_t* attach_memory(int shm_id);
void sem_add(int sem_id, short num);

void delete_shmem(int shm_id);
void delete_sem(int sem_id);

std::string msg_rcv_str(byte_t* ptr);
void msg_snd_string(const std::string& msg, byte_t* ptr);

struct msgbuf_t
{
    long mtype;
    char mtext[MSGSZ];
};

int create_or_open_mq();

std::string capture_cmd_out(const std::string& cmd);

void delete_mq(int mq_id);

#endif // LIBOS_HPP
