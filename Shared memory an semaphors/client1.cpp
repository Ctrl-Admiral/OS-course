/*
 * Вариант 5
 * Клиент1: Записать в разделяемую область памяти, созданную сервером,
 * имена текстовых файлов текущего каталога. Записать в стандартный файл вывода
 * результаты обработки сервера и время последнего отсоединения процесса от РОП.
 */

#include "libOS.hpp"
#include <cstring> //memset, memcpy
#include <stdexcept>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/shm.h>
#include <semaphore.h>


std::string get_filenames()
{
    const std::string command ="file * | awk -F\":\" '/ASCII/{print $1}'";
    std::string msg = capture_cmd_out(command);

    return msg;
}


int main() try
{
    std::cout << "CLIENT 1 STARTED\n";

    //std::vector<std::string> message = get_filenames();
    std::string msg = get_filenames();
    std::cout << "...Message created.\n" << std::flush;

#ifndef NDEBUG
    std::cout << "Message:\n" << msg << std::flush;
#endif

    int shm_id = open_shmem();
    int sem_id = open_sem();
    byte_t* ptr = attach_memory(shm_id);    //подключаемся к разд памяти

    snd_msg_string(msg, ptr);
    std::cout << "...Message sent.\n" << std::flush;

    sem_add(sem_id, +1); // Отправил сообщение? сигнал об этом серверу

    std::cout << "...Waiting server\n";

    sem_add(sem_id, -2);    // ждем-с от сервера

    std::cout << "Received message: \n" << std::flush;
    std::cout << msg_rcv_str(ptr) << std::flush;

    //sem_add(sem_id, +3); //сигнал клиенту2, что он может приступать

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
