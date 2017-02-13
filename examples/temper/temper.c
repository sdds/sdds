/*
 * This file is part of temper-tools.
 *
 * Functions to read the TEMPer USB Thermometer.  This thermometer
 * may be found at http://www.pcsensor.com/index.php?_a=viewProd&productId=15
 *
 * Copyright 2009 Roaring Penguin Software Inc.
 *
 * This software may be distributed under the terms of the GNU General
 * Public License, version 3, or (at your option) any later version.
 */

#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "bbiic.h"

#define TEMPER_DEVICE_ADDRESS 0x4f

#define BAUDRATE 9600
# define CRTSCTS  020000000000

/*
 * int temper_open("/dev/ttyUSB0")
 *
 * Given the name of a USB serial port, this function opens the
 * device for communication with the TEMPer thermometer.  It returns
 * a non-negative file descriptor on success, or -1 on error.
 */
int
temper_open(char const *dev)
{
    struct termios tio;
    int fd = open(dev, O_RDWR|O_NOCTTY);
    if (fd < 0) return -1;

    if (tcgetattr(fd, &tio) < 0) {
	int save_errno = errno;
	close(fd);
	errno = save_errno;
	return -1;
    }
    tio.c_cflag |= (CLOCAL | CREAD);

    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;

    cfsetispeed(&tio, BAUDRATE);
    cfsetospeed(&tio, BAUDRATE);

    tio.c_cflag |= CRTSCTS;

    tio.c_iflag |= IGNPAR;
    tio.c_oflag &= ~OPOST;

    /* set input mode (non-canonical, no echo,...) */
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    tio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    tio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

    if (tcflush(fd, TCIFLUSH) < 0 ||
	tcsetattr(fd, TCSANOW ,&tio) < 0) {
	int save_errno = errno;
	close(fd);
	errno = save_errno;
	return -1;
    }
    return fd;
}

/*
 * int temper_init(fd)
 *
 * Given a file descriptor returned by temper_open, this
 * function initializes the thermometer.  It returns 0 on success,
 * -1 on failure.
 */
int
temper_init(int fd)
{
    if (bbiic_stop(fd) < 0) return -1;
    bbiic_delay(50);

    if (bbiic_start(fd) < 0) return -1;

    if (bbiic_xmit_addr(fd, TEMPER_DEVICE_ADDRESS, BBIIC_WRITE) < 0) return -1;
    if (bbiic_xmit_byte(fd, 0x01) < 0) return -1;
    if (bbiic_xmit_byte(fd, 0x60) < 0) return -1;
    if (bbiic_stop(fd) < 0) return -1;

    bbiic_delay(50);
    if (bbiic_start(fd) < 0) return -1;
    if (bbiic_xmit_addr(fd, TEMPER_DEVICE_ADDRESS, BBIIC_WRITE) < 0) return -1;
    if (bbiic_xmit_byte(fd, 0x00) < 0) return -1;
    if (bbiic_xmit_byte(fd, 0x00) < 0) return -1;
    if (bbiic_stop(fd) < 0) return -1;
    return 0;
}


/*
 * int temper_read(fd, temp)
 *
 * fd is a file descriptor returned by temper_open that has also been passed
 * to a successful temper_init call.  temp is a pointer to a double.
 * This function reads the temperature in degrees Celcius.  On success,
 * it stores the temperature in *temp and returns 0.  On failure, it
 * does not touch temp and returns -1.
 */
int
temper_read(int fd, double *temp)
{
    int ack;
    int msb, lsb;
    int value;

    if (bbiic_start(fd) < 0) return -1;
    ack = bbiic_xmit_addr(fd, TEMPER_DEVICE_ADDRESS, BBIIC_READ);
    if (ack < 0) return -1;
    if (ack > 0) return 1;

    msb = bbiic_recv_byte(fd);
    if (msb < 0) return -1;
    lsb = bbiic_recv_byte(fd);
    if (lsb < 0) return -1;

    /* Temperature is an 11-bit value */
    value = (msb << 3) | ((lsb & 0xE0) >> 5);

    /* And it's two's-complement */
    if (value < 1024) {
	*temp =  ((double) value) * 0.125;
    } else {
	*temp = -(2048.0 - (double) value) * 0.125;
    }
    return 0;
}
