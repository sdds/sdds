/*
 * This file is part of temper-tools.
 *
 * Header files for the "bit-banging" IIC implementation.
 *
 * Copyright 2009 Roaring Penguin Software Inc.
 *
 * This software may be distributed under the terms of the GNU General
 * Public License, version 3, or (at your option) any later version.
 */

extern int bbiic_start(int);
extern int bbiic_stop(int);
extern int bbiic_xmit_byte(int, int);
extern int bbiic_xmit_addr(int, int, int);
extern int bbiic_recv_byte(int);
extern void bbiic_delay(int);

#define BBIIC_WRITE 0
#define BBIIC_READ  1
