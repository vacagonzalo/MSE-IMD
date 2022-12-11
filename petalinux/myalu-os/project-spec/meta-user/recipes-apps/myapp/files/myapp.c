#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "myalumodule.h"

int fd = NULL;
const char *dev_name = "/dev/myalu_device";

struct compute_t compute = {
    .operation = OPERATION_ADD,
    .operand1 = 5,
    .operand2 = 5};

struct result_t result = {
    .value = 0,
    .carry = 0};

int main(int argc, char **argv)
{
    printf("Testing myalumodule!\n");
    system("modprobe myalumodule");

    /* Time for the module to create the device */
    sleep(1);

    fd = open(dev_name, O_RDONLY);
    if (-1 == fd)
    {
        printf("could not open %s\n", dev_name);
        return fd;
    }

    printf("5 + 5 = (");
    ioctl(fd, WR_VALUE, &compute);

    /* Time for the alu to compute */
    sleep(1);

    ioctl(fd, RD_VALUE, &result);
    printf("carry: %d | result: %d)\n\r", result.carry, result.value);

    close(fd);
    return 0;
}
