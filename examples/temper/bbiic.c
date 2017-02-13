/*
 * This file is part of temper-tools.
 *
 * Functions to "bit-bang" an I2C bus over USB or a serial port
 *
 * Copyright 2009 Roaring Penguin Software Inc.
 *
 * This software may be distributed under the terms of the GNU General
 * Public License, version 3, or (at your option) any later version.
 */

#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* Internal macros */
#define u_delay(usec) usleep(usec)
#define set_clk(fd, flag) set_dtr((fd), (flag))
#define set_data(fd, flag) set_rts((fd), (flag))

/*
 * Delay for usec microseconds
 */

void
bbiic_delay(int usec)
{
    usleep(usec);
}

static int
set_dtr(int fd, int set)
{
    int status = 0;
    if (ioctl(fd, TIOCMGET, &status) < 0) return -1;
    if (set) {
	status |= TIOCM_DTR;
    } else {
	status &= ~TIOCM_DTR;
    }
    return ioctl(fd, TIOCMSET, &status);
}

static int
set_rts(int fd, int set)
{
    int status = 0;
    if (ioctl(fd, TIOCMGET, &status) < 0) return -1;
    if (set) {
	status |= TIOCM_RTS;
    } else {
	status &= ~TIOCM_RTS;
    }
    return ioctl(fd, TIOCMSET, &status);
}

static int
get_cts(int fd)
{
    int status = 0;
    if (ioctl(fd, TIOCMGET, &status) < 0) return -1;
    return (status & TIOCM_CTS) ? 1 : 0;
}

static int
get_data(int fd)
{
    if (set_rts(fd, 1) < 0) return -1;
    return get_cts(fd);
}

static int
toggle_clock_hi_low(int fd)
{
    u_delay(10);
    if (set_clk(fd, 1) < 0) return -1;
    u_delay(10);
    if (set_clk(fd, 0) < 0) return -1;
    u_delay(10);
    return 0;
}

/* Signal an IIC START.  That's a high->low transition on DATA
   while CLK is high; then we bring CLK low. */
int
bbiic_start(int fd)
{
    if (set_data(fd, 1) < 0) return -1;
    u_delay(4);
    if (set_clk(fd, 1) < 0) return -1;
    u_delay(40);
    if (set_data(fd, 0) < 0) return -1;
    u_delay(40);
    return set_clk(fd, 0);
}

/* Signal an IIC STOP condition.  That's a low->high transition
   on DATA while CLK is high */
int
bbiic_stop(int fd)
{
    if (set_data(fd, 0) < 0) return -1;
    u_delay(4);
    if (set_clk(fd, 1) < 0) return -1;
    u_delay(40);
    if (set_data(fd, 1) < 0) return -1;
    u_delay(40);
    return 0;
}

/* Transmit a byte on the IIC bus */
int
bbiic_xmit_byte(int fd, int byte)
{
    int bitpos = 128;

    while(bitpos) {
	if (set_data(fd, (byte & bitpos ? 1 : 0)) < 0) return -1;
	if (toggle_clock_hi_low(fd) < 0) return -1;
	bitpos >>= 1;
    }

    /* Get the ACK bit */
    u_delay(50);
    if (set_clk(fd, 1) < 0) return -1;
    u_delay(50);
    return get_data(fd);
}

/* Receive a byte.  Returns -1 on error,
   otherwise the byte */
int
bbiic_recv_byte(int fd)
{
    int shift = 128;
    int byte = 0;
    int bit;

    while (shift) {
	if (toggle_clock_hi_low(fd) < 0) return -1;
	bit = get_data(fd);
	if (bit < 0) return -1;
	if (bit) byte |= shift;
	shift >>= 1;
    }

    /* Ack the byte */
    if (set_data(fd, 0) < 0) return -1;
    if (toggle_clock_hi_low(fd) < 0) return -1;

    return byte;
}

/* Transmit an address on the bus.  The address
 * is a 7-bit quantity in addr.  readwrite is a one-bit
 * flag, where 0 indicates write and 1 indicates read */
int
bbiic_xmit_addr(int fd, int addr, int readwrite)
{
    return bbiic_xmit_byte(fd, ((addr << 1) | readwrite));
}
