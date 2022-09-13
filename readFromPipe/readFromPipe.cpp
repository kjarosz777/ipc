#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <span>
#include <string>
#include <fstream>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, const char* argv[])
{
    const std::span<const char*> arguments(argv, argc);

    if (2 != arguments.size())
    {
        printf("Wrong argument count (should be 2, provided %lu)\n", arguments.size());
        return -1;
    }

    if (!std::filesystem::exists(arguments.back()))
    {
        printf("mkfifo for path: %s\n", arguments.back());
        mkfifo(arguments.back(), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    }
    else
    {
        printf("pipe already created with path: %s\n", arguments.back());
    }

    std::ifstream myfile(arguments.back());
    // myfile.open(arguments.back()); // Not needed, ifstream is RAII
    if (!myfile.is_open())
    {
        printf("error opening %s ,is_open(): %d\n", arguments.back(), myfile.is_open());
        return -1;
    }

    std::string s;
    while(std::getline(myfile, s))
    {
        printf("NEW LINE: %s\n", s.c_str());
    }
    // myfile.close(); // Not needed, ifstream is RAII

    return 0;
}

/*
int main(int argc, const char* argv[])
{
    const std::span<const char*> arguments(argv, argc);

    if (2 != arguments.size())
    {
        printf("Wrong argument count (should be 2, provided %lu)\n", arguments.size());
        return -1;
    }

    const int fd = open(arguments.back(), O_RDONLY);
    if (fd < 0)
    {
        printf("error opening %s (%d)\n", arguments.back(), fd);
        return -1;
    }

    printf("opened %s (fd: %d)\n", arguments.back(), fd);

    // make non-blocking
    // fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    // make a file descriptor for pipe
    FILE *f = fdopen(fd, "r");

    char *line;
    size_t len;

    while(getline(&line, &len, f) > 0)
    {
        printf("OLD LINE: %s", line);
    }

    close(fd);
    
    return 0;
}
*/
