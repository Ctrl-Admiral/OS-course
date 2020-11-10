/* ВАРИАНТ 5
 * ========= КЛИЕНТ =========
 * Создать гнездо без установления соединения домена UNIX.
 * Прочитать сообщение из серверного гнезда.
 * Уменьшить прочитанное значение на число, равное количеству файлов, имя которых начинается с точки
 * и передать это значение в гнездо сервера.
 */

#include <libOS.hpp>
#include <iostream>
#include <stdexcept>

int main() try {

    std::cout << "CLIENT STARTED" << std::endl;


}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
