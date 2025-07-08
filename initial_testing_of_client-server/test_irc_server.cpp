# include <iostream>
# include <cstdlib>
# include <cstring>
# include <string>
# include <cerrno>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <poll.h>
# include <signal.h>
# include <netdb.h>
# include <stdio.h>

# define PORT "3490"
# define BACKLOG 10

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int status, sockfd, acceptfd, yes = 1;
    struct addrinfo hints, *servinfo, *temp;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    char str[INET6_ADDRSTRLEN];

    //Getting the address information
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        std::cerr << "Error : " << gai_strerror(status) << std::endl;
        exit(1);
    }


    //Loop through the addresses of servinfo to bind to the socket we can
    for (temp = servinfo; temp != NULL; temp = servinfo->ai_next)
    {
        //Getting the fd of socket
        if ((sockfd = socket(temp->ai_family, temp->ai_socktype, 
                temp->ai_protocol)) == -1)
        {
            std::cerr << "Error : " << strerror(errno) << std::endl;
            continue ;
        }

        //Adding option to reuse the socket which is in cleaning process
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            freeaddrinfo(servinfo);
            exit(1);
        }
        
        //Binding -> assotiating the socket with the port
        if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        {
            std::cerr << "Error : " << strerror(errno) << std::endl;
            close(sockfd);
            continue ;
        }

        //If we reach this point, it means that the socket is assotiated to the port correctly
        break ;
    }

    //Free the address information as all is done with this struct
    freeaddrinfo(servinfo);
    
    //Additional check that we didn't reach the end of for cycle
    if (temp == NULL)
    {
        std::cerr << "Error : failed to bind" << std::endl;
        exit(1);
    }


    //Listening
    if (listen(sockfd, BACKLOG) == -1)
    {
        std::cerr << "Error : " << strerror(errno) << std::endl;
        close(sockfd);
        exit(1);
    }

    std::cout << "Server : waiting for connections..." << std::endl;

    //while (1)
    {
        //Accepting the connection
        addr_size = sizeof(client_addr);
        acceptfd = accept(sockfd, reinterpret_cast<struct sockaddr*>(&client_addr),
            &addr_size);
        if (acceptfd == -1)
        {
            std::cerr << "Error of accept : " << strerror(errno) << std::endl;
            //continue ;
        }

        inet_ntop(client_addr.ss_family, get_in_addr(reinterpret_cast<struct sockaddr*>(&client_addr)),
                str, sizeof(str));
        std::cout << "Server : got connection from " << str << std::endl;

        if (!fork())
        {
            close(sockfd);
            if (send(acceptfd, "Hello World!", 13, 0) == -1)
                std::cerr << strerror(errno) << std::endl;
            std::cout << "Server : message is sent to " << str << std::endl;
            close(acceptfd);
            exit(0);
        }
        close(acceptfd);
    }

    //Close the socket
    close(sockfd);
    return (0);
}