#include <fcntl.h> //creat
#include <sys/msg.h> //msg queues
#include <sys/types.h> //key_t

#include <cstdio> //perror
#include <stdexcept> //errors

constexpr int PROJECT_ID = 2486;
const std::string PATH = "/tmp/lab_mq";

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
