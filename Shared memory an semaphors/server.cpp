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
#include <iostream>

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

int main()
{

}
