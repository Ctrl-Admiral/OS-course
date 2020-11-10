/* ВАРИАНТ 5
 * ========= СЕРВЕР =========
 * Создать гнездо без установления соединения домена UNIX. Присвоить ему имя.
 * Записать в него информацию о количестве файлов текущего каталога.
 * Распечатать информацию, полученную от клиента.
 */

#include <libOS.hpp>
#include <iostream>
#include <stdexcept>

int main() try {

    std::cout << "SERVER STARTED" << std::endl;


}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
