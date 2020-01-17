#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    char buf[256];

    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(13);
    addr.sin_addr.s_addr = inet_addr("150.254.32.131");
    
    connect(fd, (struct sockaddr*)& addr, sizeof(addr));
    int i = read(fd, buf, 256);
    close(fd);
    write(1, buf, i);   //wypisanie na out

    return 0;
}
