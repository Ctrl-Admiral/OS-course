/* ВАРИАНТ 5
 * ========= КЛИЕНТ =========
 * Создать гнездо без установления соединения домена UNIX.
 * Прочитать сообщение из серверного гнезда.
 * Уменьшить прочитанное значение на число, равное количеству файлов, имя которых начинается с точки
 * и передать это значение в гнездо сервера.
 */

#include "libOS.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h> //unlink

int main() try {

    std::cout << "CLIENT STARTED" << std::endl;

    ::unlink(PATH_CLIENT_SOCK.c_str());
    Socket client_sock{};
    sockaddr_un client_addr = get_client_addr();
    bind_wrapper(client_sock.sfd, client_addr);

    sockaddr_un server_addr = get_server_addr();
    std::string rcv_msg = recvfrom_str(client_sock.sfd, server_addr);
    std::cout << "...Message received." << std::endl;

#ifndef NDEBUG
    std::cout << "[DEBUG] Number of files (received): " << rcv_msg << std::endl;
#endif

    int point_files = std::stoi(capture_cmd_out("ls -lad .[!.]* | wc -l"));
#ifndef NDEBUG
    std::cout << "[DEBUG] Number of point files: " << point_files << std::endl;
#endif
    std::string msg = std::to_string(std::stoi(rcv_msg) - point_files);
    sendto_str(client_sock.sfd, server_addr, msg);
#ifndef NDEBUG
    std::cout << "[DEBUG] Message done: " << msg << std::endl;
#endif
    std::cout << "...Message sent." << std::endl;

}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
