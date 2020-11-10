//СЕРВЕР
//Выбрать из очереди все сообщения.
//Отсортировать список файлов из очереди по времени создания и
//записать эту информацию в стандартный файл вывода.
//Определить идентификатор процесса, который последним передал
//сообщение в очередь и максимальную длину очереди сообщений в байтах.
//Удалить очередь сообщений.

#include <libOS.hpp>
#include <fcntl.h> //open
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h> //close

#include <algorithm> //std::tie
#include <iostream>
#include <sstream> //stringstream
#include <stdexcept>
#include <string>
#include <vector>

struct filedata
{
    std::size_t datebirth_nsec;
    std::string name;
};

bool operator<(const filedata& lhs, const filedata& rhs)
{
    return std::tie(lhs.datebirth_nsec, lhs.name) < std::tie(rhs.datebirth_nsec, rhs.name);
}

bool operator==(const filedata& lhs, const filedata& rhs)
{
    return std::tie(lhs.datebirth_nsec, lhs.name) == std::tie(rhs.datebirth_nsec, rhs.name);
}

struct MessageQueue
{
    MessageQueue()
        : id(create_or_open_mq())
    {
    }

    ~MessageQueue()
    {
        try
        {
            delete_mq(id);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    const int id;
};

std::string msg_rcv(int mq_id)
{
    msgbuf_t msg;
    if (::msgrcv(mq_id, &msg, sizeof(msg.mtext) + 1, 1, 0) == -1)
    {
        std::perror(msg.mtext);
        throw std::runtime_error("Message receiving error");
    }

    return std::string(reinterpret_cast<char*>(msg.mtext));
}

std::vector<filedata> sort_filenames_by_date(const std::string& msg)
{
    std::vector<filedata> files_with_birthdates;
    std::istringstream filestream(msg);
    std::string line;

    struct statx stx;
    std::getline(filestream, line);
    int dirfd = ::open(line.c_str(), 0);

    while (std::getline(filestream, line))
    {

        if (::statx(dirfd, line.c_str(), 0, STATX_BTIME, &stx) != 0)
        {
            std::perror(line.c_str());
            ::close(dirfd);
            throw std::runtime_error("Statx error");
        }
        filedata fdata = {stx.stx_btime.tv_nsec, line};
        files_with_birthdates.emplace_back(fdata);
    }
    ::close(dirfd);

    std::sort(files_with_birthdates.begin(), files_with_birthdates.end());
    return files_with_birthdates;
}

int main() try
{
    std::cout << "SERVER STARTED\n";
    MessageQueue mq;

    std::string msg = msg_rcv(mq.id);
    std::cout << "...Message received\n";

#ifndef NDEBUG
    std::cout << "Message: \n" << msg << '\n';
#endif

    std::vector<filedata> filenames_sorted = sort_filenames_by_date(msg);

    std::cout << "\nSorted by birthdate list of files:\n";
    for (const auto& [btime, filename] : filenames_sorted)
    {
        std::cout << filename << '\n';
    }

    struct msqid_ds buf;
    msgctl(mq.id, IPC_STAT, &buf);
    std::cout << "\nLast send PID: " << buf.msg_lspid << '\n';
    std::cout << "\nMax queue length: " << buf.msg_qbytes << '\n';
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
