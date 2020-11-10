/*
 * Вариант 5
 * Клиент2: Записать в разделяемую область памяти,
 * созданную сервером, имена файлов текущего каталога,
 * написанные на языке программирования Си.
 * Записать в стандартный файл вывода результаты обработки этого запроса.
 */

#include "libOS.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/shm.h>

std::string get_filenames_c()
{
    const std::string command ="find *.c";
    std::string msg = capture_cmd_out(command);

    return msg;
}

int main() try
{
    std::cout << "CLIENT 2 STARTED\n";

    int shm_id = open_shmem();
    int sem_id = open_sem();
    byte_t* ptr = attach_memory(shm_id);    //подключаемся к разд памяти

    sem_add(sem_id, -3);    //wait for client1

    std::string msg = get_filenames_c();
    std::cout << "...Message created\n" << std::flush;

#ifndef NDEBUG
    std::cout << "----------------------\n"
              << "Created message:\n" << msg
              << "----------------------\n"
              << std::flush;
#endif
    msg_snd_string(msg, ptr);
    std::cout << "...Message sent\n" << std::flush;

    sem_add(sem_id, +4);    //сигнал серверу, что все готово

    std::cout << "...Waiting for server.\n" << std::flush;
    sem_add(sem_id, -5);

    std::cout << "Received list of files more than 4 blocks: \n" << std::flush;
    std::cout << msg_rcv_str(ptr) << std::flush;

    if (::shmdt(ptr) == -1)
    {
        std::perror("shmdt");
        throw std::runtime_error("shmdt");
    }
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
