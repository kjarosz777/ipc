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

    std::ofstream myfile;
    myfile.open(arguments.back());
    if (!myfile.is_open())
    {
        printf("error opening %s ,is_open(): %d\n", arguments.back(), myfile.is_open());
        return -1;
    }

    for (int i = 0; i < 100; ++i)
    {
        // myfile << i << ": Writing this to a file.\n";
        // myfile.close();

        std::string msg("NEW" + std::to_string(i) + ": Writing this to a pipe.\n");

        myfile << msg;

        printf("%s", msg.c_str());
    }

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

    const int fd = open(arguments.back(), O_WRONLY);
    if (fd < 0)
    {
        printf("error opening %s (%d)\n", arguments.back(), fd);
        return -1;
    }

    printf("opened %s (fd: %d)\n", arguments.back(), fd);

    for (int i = 0; i < 100; ++i)
    {
        std::string msg("OLD" + std::to_string(i) + ": Writing this to a pipe.\n");

        size_t res = write(fd, msg.c_str(), msg.size());
        printf("%d: OLD write returned %ld\n", i, res);
    }

    close(fd);

    return 0;
}
*/
