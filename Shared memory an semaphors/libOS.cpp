#include "libOS.hpp"

#include <fcntl.h> //creat
#include <sys/msg.h> //msg queues
#include <sys/types.h> //key_t
//#include <sys/ipc.h>
#include <sys/shm.h> //shmget
#include <sys/sem.h> //semget

#include <unistd.h> //usleep
#include <cstdio> //perror
#include <stdexcept> //errors

constexpr int PROJECT_ID = 2486;
const std::string PATH = "/tmp/lab_mq";
const std::string PATH_MEM = "/tmp/lab_shmem_mem";
const std::string PATH_SEM = "/tmp/lab_shmem_sem";

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

int open_shmem()
{
    key_t key = get_key_mem();
    int shm_id = ::shmget(key, SHMEM_SIZE, 0666);
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
 * @brief Функция получения ключа очереди
 */
key_t get_key()
{
    ::creat(PATH.c_str(), 0600);
    key_t key = ::ftok(PATH.c_str(), PROJECT_ID);
    if (key == -1)
    {
        std::perror("Cannot get a key with ftok");
        throw std::runtime_error("Cannot get a key with ftok");
    }
    return key;
}

/**
 * @brief Функция для открытия или создания очереди сообщений
 * @return
 */
int create_or_open_mq()
{
    key_t key = get_key();
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
