#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>

#define CONNECTION_STRING "CONNECTION_STRING"

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("USAGE: %s <device>\n", argv[0]);
        return 1;
    }

    char *devicePath = argv[1];
    int device = open(devicePath, O_RDWR);
    if(device == -1) {
        printf("ERROR: Couldn't open device %s. errno = %d\n", devicePath, errno);
        return errno;
    }

    printf("CONNECTED\n");

    struct termios term, restore;
    tcgetattr(STDIN_FILENO, &term);
    tcgetattr(STDIN_FILENO, &restore);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    write(device, CONNECTION_STRING, sizeof(CONNECTION_STRING));

    while(true) {
        char input;
        if(read(STDIN_FILENO, &input, 1) != 1) {
            printf("ERROR: Input\n");
            break;
        }

        printf("%c", input);

        write(device, &input, 1);
    }

    close(device);
    return 0;
}
