/* ВАРИАНТ 5
 * ========= СЕРВЕР =========
 * Создать гнездо без установления соединения домена UNIX. Присвоить ему имя.
 * Записать в него информацию о количестве файлов текущего каталога.
 * Распечатать информацию, полученную от клиента.
 */

#include "libOS.hpp"

#include <iostream>
#include <stdexcept>
#include <unistd.h>

int main() try {

    std::cout << "SERVER STARTED" << std::endl;

    ::unlink(PATH_SERVER_SOCK.c_str()); // without it: bind error: already in use

    Socket server_sock{};
    sockaddr_un server_addr = get_server_addr();

    //int sockoptval = 1;
    //::setsockopt(server_sock.sfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof (sockoptval));

    bind_wrapper(server_sock.sfd, server_addr);

    std::string msg = capture_cmd_out("ls | wc -l");
#ifndef NDEBUG
    std::cout << "[DEBUG] Number of files in directory: " << msg << std::endl;
#endif

    sockaddr_un client_addr = get_client_addr();
    sendto_str(server_sock.sfd, client_addr, msg);
    std::cout << "...Message sent." << std::endl;

    std::string recv_msg = recvfrom_str(server_sock.sfd, client_addr);
    std::cout << "...Message received.\n"
              << "Number of non-point files in directory: " << recv_msg << std::endl;

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
}
