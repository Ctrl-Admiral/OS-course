#include "libOS.hpp"

#include <fcntl.h> //creat
#include <sys/msg.h> //msg queues
#include <sys/types.h> //key_t
#include <sys/shm.h> //shmget
#include <sys/sem.h> //semget
#include <sys/un.h> //sockaddr_un

#include <unistd.h> //usleep
#include <string.h>
#include <cstdio> //perror
#include <stdexcept> //errors
#include <vector>
#include <cstring>

//#include <typeinfo>

constexpr int PROJECT_ID = 2486;

const std::string PATH_QUEUE = "/tmp/lab_mq";
const std::string PATH_MEM = "/tmp/lab_shmem_mem";
const std::string PATH_SEM = "/tmp/lab_shmem_sem";


int create_socket()
{
    int sfd = ::socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        std::perror("socket create error");
        throw std::runtime_error("socket create error");
    }
    return sfd;
}

void close_socket(int sfd)
{
    if (::close(sfd) == -1)
    {
        std::perror("socket close error");
        throw std::runtime_error("socket close error");
    }
}

sockaddr_un get_addr_impl(std::string path)
{
    struct sockaddr_un addr;

    std::memset(&addr, 0, sizeof (addr));
    addr.sun_family = AF_UNIX;
    ::strcpy(addr.sun_path, path.c_str());
    return addr;
}

sockaddr_un get_server_addr()
{
    return get_addr_impl(PATH_SERVER_SOCK);
}

sockaddr_un get_client_addr()
{
    return get_addr_impl(PATH_CLIENT_SOCK);
}

void bind_wrapper(int sfd, const sockaddr_un& addr)
{
    if (::bind(sfd, reinterpret_cast<const sockaddr*>(&addr), sizeof (addr)) == -1)
    {
        std::perror("bind error");
        throw std::runtime_error("bind error");
    }
}

void sendto_str(int sfd, struct sockaddr_un addr, std::string msg)
{
    socklen_t socklen = sizeof(addr);
    if (::sendto(sfd, msg.data(), msg.size(), 0, (struct sockaddr*)(&addr), socklen) == -1)
    {
        std::perror("send error");
        throw std::runtime_error("send error");
    }
}


std::string recvfrom_str(int sfd, struct sockaddr_un& addr)
{
    socklen_t socklen = sizeof(addr);
    std::string buf(sizeof(std::size_t), '\0');
    if (::recvfrom(sfd, buf.data(), buf.size(), 0, reinterpret_cast<sockaddr*>(&addr), &socklen) == -1)
    {
        std::perror("recvfrom error");
        throw std::runtime_error("recv error");
    }
    return buf;
}


/**
 * \brief читает сообщение из области памяти, на которую указывает ptr
 * \param[in] ptr
 * \return
 */
std::string msg_rcv_str(byte_t* ptr)
{
    char buf[SHMEM_SIZE];
    ::strcpy(buf, ptr);
    std::string msg(buf);
    return msg;
}

/**
 * @brief пишет сообщение в область памяти, на которую указывает ptr
 * @param[in] msg -- строка-сообщение
 * @param[in] ptr
 */
void msg_snd_string(const std::string& msg, byte_t* ptr)
{
    // избавляемся от мусора в памяти
    ::memset(ptr, 0, SHMEM_SIZE);
    ::memcpy(ptr, msg.c_str(), msg.size());
}

/**
 * @brief производит операцию над семафором из набора sem_id
 * @param[in] sem_id -- id набора семафоров
 * @param[in] num -- операция (0, -1, +1, ...)
 */
void sem_add(int sem_id, short num)
{
//     sembuf:  unsigned short int sem_num;	/* semaphore number */
//              short int sem_op;           /* semaphore operation */
//              short int sem_flg;          /* operation flag */

    struct sembuf sb = {0, num, 0};
    ::semop(sem_id, &sb, 1);
}

/**
 * @brief присоединение разделяемой области памяти
 * @param shm_id -- дескриптор, ассоциируемый с ресурсом
 * @return
 */
byte_t* attach_memory(int shm_id)
{
    const byte_t* ERROR_PTR = reinterpret_cast<const byte_t*>(-1);
    byte_t* ptr = reinterpret_cast<byte_t*>(::shmat(shm_id, nullptr, 0));
    if (ptr == ERROR_PTR)
    {
        std::perror("shmat error");
        throw std::runtime_error("shmat error");
    }
    return ptr;
}

/**
 * @brief Получение ключа ресурса
 * @param[in] path
 * @return ключ типа key_t
 */
key_t get_key_impl(const std::string& path)
{
    ::creat(path.c_str(), 0600);
    key_t key = ::ftok(path.c_str(), PROJECT_ID);
    if (key == -1)
    {
        std::perror("Cannot get a key with ftok");
        throw std::runtime_error("Cannot get a key with ftok");
    }
    return key;
}

key_t get_key_mem()
{
    return get_key_impl(PATH_MEM);
}

key_t get_key_sem()
{
    return get_key_impl(PATH_SEM);
}

/**
 * @brief cоздаtn сегмент разделяемой памяти
 * @return id сегмента
 */
int create_shmem()
{
    key_t key = get_key_mem();


    int shm_id = ::shmget(key, SHMEM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1)
    {
        std::perror("shmget error");
        throw std::runtime_error("shmget error");
    }

    return shm_id;
}

/**
 * @brief получает id существующего сегмента разделяемой памяти
 * @return id сегмента
 */
int open_shmem()
{
    key_t key = get_key_mem();
    int shm_id = ::shmget(key, SHMEM_SIZE, 0666);
    //waiting for shmem
    while (shm_id == -1)
    {
        ::usleep(10'000);
        shm_id = ::shmget(key, SHMEM_SIZE, 0666);
    }

    return shm_id;
}

int create_sem()
{
    key_t key = get_key_sem();
    int sem_id = ::semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (sem_id == -1)
    {
        std::perror("semget error");
        throw std::runtime_error("semget error");
    }

    return sem_id;
}

int open_sem()
{
    key_t key = get_key_sem();
    int sem_id = ::semget(key, 1, 0666);
    //waiting for sem
    while (sem_id == -1)
    {
        ::usleep(10'000);
        sem_id = ::semget(key, 1, 0666);
    }

    return sem_id;
}

void delete_shmem(int shm_id)
{
    if (::shmctl(shm_id, IPC_RMID, nullptr) == -1)
    {
        std::perror("shmctl delete error");
        throw std::runtime_error("shmctl delete error");
    }
}

void delete_sem(int sem_id)
{
    if (::semctl(sem_id, 0, IPC_RMID) == -1)
    {
        std::perror("semctl delete error");
        throw std::runtime_error("semctl delete error");
    }
}

/**
 * @brief Функция для удаления очереди сообщений
 * @param mq_id -- id очереди
 */
void delete_mq(int mq_id)
{
    if (::msgctl(mq_id, IPC_RMID, nullptr) != 0)
    {
        std::perror("msgctl delete mq error");
        throw std::runtime_error("msgctl delete mq error");
    }
}


/**
 * @brief Функция для открытия или создания очереди сообщений
 * @return
 */
int create_or_open_mq()
{
    key_t key = get_key_impl(PATH_QUEUE);
    int mq_id = ::msgget(key, IPC_CREAT | 0666);
    if (mq_id == -1)
    {
        std::perror("Message queue creation or open error");
        throw std::runtime_error("MQ creation or open error");
    }

    return mq_id;
}

/**
 * @brief Функция для получения вывода команды в виде std::string
 * @param cmd -- команда bash
 * @return std::string -- строка с выводом функции
 */
std::string capture_cmd_out(const std::string& cmd)
{
    std::string data;
    FILE* stream;
    const int max_buffer = 1024;
    char buffer[max_buffer];

    stream = ::popen(cmd.c_str(), "r");
    if (stream != nullptr)
    {
        while (std::feof(stream) == 0)
            if (std::fgets(buffer, max_buffer, stream) != nullptr)
                data.append(buffer);
        ::pclose(stream);
    }
    return data;
}
