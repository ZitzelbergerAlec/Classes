#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LAUNCHER_NODE           "/dev/launcher0"
#define LAUNCHER_FIRE           0x10
#define LAUNCHER_STOP           0x20
#define LAUNCHER_UP             0x02
#define LAUNCHER_DOWN           0x01
#define LAUNCHER_LEFT           0x04
#define LAUNCHER_RIGHT          0x08
#define LAUNCHER_UP_LEFT        (LAUNCHER_UP | LAUNCHER_LEFT)
#define LAUNCHER_DOWN_LEFT      (LAUNCHER_DOWN | LAUNCHER_LEFT)
#define LAUNCHER_UP_RIGHT       (LAUNCHER_UP | LAUNCHER_RIGHT)
#define LAUNCHER_DOWN_RIGHT     (LAUNCHER_DOWN | LAUNCHER_RIGHT)

#define SEC 1000

static void launcher_cmd(int fd, int cmd)
{
        int retval = 0;

        retval = write(fd, &cmd, 1);
        if (retval < 0) {
                fprintf(stderr, "Could not send command to " LAUNCHER_NODE
                        " (error %d)\n", retval);
        } else if (LAUNCHER_FIRE == cmd) {
                usleep(5000000);
        }
}

int main(int argc, char **argv)
{
        char c;
        int fd;
        int cmd = LAUNCHER_STOP;
        char *dev = LAUNCHER_NODE;
        unsigned int duration = 50;

        fd = open(dev, O_RDWR);
        if (fd == -1) {
                perror("Couldn't open file: %m");
                exit(1);
        }

	system("/bin/stty raw");
        while ((c = getchar()) != 'q') {
                switch (c) {
               case 'a':
                        cmd = LAUNCHER_LEFT;
                        break;
                case 'd':
                        cmd = LAUNCHER_RIGHT;
                        break;
                case 'w':
                        cmd = LAUNCHER_UP;
                        break;
                case 's':
                        cmd = LAUNCHER_DOWN;
                        break;
                case ' ': 
                        cmd = LAUNCHER_FIRE;
                        break;
                case 'e':       
                        cmd = LAUNCHER_STOP;
                        break;
                }
		launcher_cmd(fd, cmd);
		usleep(duration *SEC);
		launcher_cmd(fd, LAUNCHER_STOP);
        }

        close(fd);
	system("/bin/stty cooked");
        return EXIT_SUCCESS;
}

