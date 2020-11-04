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

int main() try
{

//    int shm_id = open_shmem();
//    int sem_id = open_sem();
//    byte_t* ptr = attach_memory(shm_id);    //подключаемся к разд памяти

//    sem_add(sem_id, -3);    //wait for client1

//    sem_add(sem_id, +4);    //сигнал серверу, что все готово

//    if (::shmdt(ptr) == -1)
//    {
//        std::perror("shmdt");
//        throw std::runtime_error("shmdt");
//    }
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
