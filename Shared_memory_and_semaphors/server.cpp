/*
 * Вариант #5
 *
 * Сервер: Создать разделяемую область памяти и набор семафоров.
 * Подождать, пока один из клиентов не пришлет информацию.
 * Среди полученных имен файлов, определить такие, размер
 * которых превышает 2 блока, и эти данные переслать через разделяемую
 * область памяти соответствующему клиенту. После обработки информации клиентами удалить РОП и НС.
 */

#include "libOS.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/shm.h>

//shared memory and sem struct
struct Resources
{
    Resources()
        : shm_id(create_shmem())
        , sem_id(create_sem())
    {
    }

    ~Resources()
    {
        try
        {
            //std::cout << "~Resources;\n";
            delete_shmem(shm_id);
            delete_sem(sem_id);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught in dtor: " << e.what() << std::endl;
        }
    }

    const int shm_id;
    const int sem_id;
};

std::string msg_handle(const std::string& msg)
{
    std::string ans = "";
    std::istringstream msgstream(msg);

    std::string line;

    std::string command = "";

#ifndef NDEBUG
    std::cout << "----------------------\n";
#endif

    while (std::getline(msgstream, line))
    {
        command = "du " + line + " | grep '[0-9]*' | head -1";
        int blocks = std::stoi(capture_cmd_out(command));

#ifndef NDEBUG
        std::cout
                  << "Block of " << line << ": " << blocks << '\n';
#endif

        if (blocks > 2) ans += (line + "\n");
    }

#ifndef NDEBUG
    std::cout << "----------------------\n";
#endif

    return ans;
}

int main()
{
    std::cout << "SERVER STARTED\n";

    Resources resources{};
    byte_t* ptr = attach_memory(resources.shm_id);

    sem_add(resources.sem_id, -1); // wait for client1 message

    std::string msg = msg_rcv_str(ptr);

#ifndef NDEBUG
    std::cout << "----------------------\n"
              << "Message from client1:\n" << msg
              << "----------------------\n"
              << std::flush;
#endif

    std::string res = msg_handle(msg);

#ifndef NDEBUG
    std::cout << "----------------------\n"
              << "Handled message:\n" << res
              << "----------------------\n"
              << std::flush;
#endif

    msg_snd_string(res, ptr);

    std::cout << "...Message sent to client1.\n" << std::flush;
    sem_add(resources.sem_id, +2);  //signal for client1

    std::cout << "...Signal to client1 sent.\n" << std::flush;

    std::cout << "...Waiting for client2.\n" << std::flush;
    sem_add(resources.sem_id, -4); //waiting for client2

    msg = msg_rcv_str(ptr);

#ifndef NDEBUG
    std::cout << "----------------------\n"
              << "Message from client1:\n" << msg
              << "----------------------\n"
              << std::flush;
#endif

    res = msg_handle(msg);

#ifndef NDEBUG
    std::cout << "----------------------\n"
              << "Handled message:\n" << res
              << "----------------------\n"
              << std::flush;
#endif

    msg_snd_string(res, ptr);
    std::cout << "...Message sent to client2.\n" << std::flush;

    sem_add(resources.sem_id, +5);
    std::cout << "...Signal to client2 sent.\n" << std::flush;

    if (::shmdt(ptr) == -1)
    {
        std::perror("shmdt");
        throw std::runtime_error("shmdt");
    }

}
