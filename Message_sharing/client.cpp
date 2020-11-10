//Клиент. Создать очередь сообщений. Передать в эту очередь полное имя текущего каталога
//и список файлов текущего каталога, в которых встречается подстрока «define».

#include "libOS.hpp"
#include <iostream>
#include <sys/msg.h>

/**
 * @brief функция отправки сообщения в очередь
 * @param mq_id -- id очереди сообщения
 * @param msg_content -- содержание сообщения
 */
void send_msg(int mq_id, const std::string& msg_content)
{
    msgbuf_t msg;
    msg.mtype = 1;

    if (msg_content.size() + 1 > MSGSZ)
    {
        delete_mq(mq_id);
        throw std::runtime_error("Too big message :(");
    }

    std::copy(msg_content.begin(), msg_content.end(), msg.mtext);
    msg.mtext[msg_content.size()] = 0; //nullterm

    if (::msgsnd(mq_id, &msg, msg_content.size() + 1, IPC_NOWAIT) != 0)
    {
        std::perror("Message senging error");
        throw std::runtime_error("Message sending error :(");
    }
}

int main() try
{
    std::cout << "CLIENT STARTED\n";

    int mq_id = create_or_open_mq();
    std::cout << "...Message queue created" << std::endl;

    std::string list_filenames = capture_cmd_out("{ pwd & grep -rl \"define\"; }");
    std::cout << "...Message done" << std::endl;

#ifndef NDEBUG
    std::cout << list_filenames << '\n';
#endif

    send_msg(mq_id, list_filenames);
    std::cout << "...Message sent\n";

    return 0;
}
catch (const std::exception& e)
{
   std::cerr << e.what() << std::endl;
}
