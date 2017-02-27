/****************************************************************************
 * Copyright (C) 2017 RheinMain University of Applied Sciences              *
 *                                                                          *
 * This file is part of:                                                    *
 *      _____  _____   _____                                                *
 *     |  __ \|  __ \ / ____|                                               *
 *  ___| |  | | |  | | (___                                                 *
 * / __| |  | | |  | |\___ \                                                *
 * \__ \ |__| | |__| |____) |                                               *
 * |___/_____/|_____/|_____/                                                *
 *                                                                          *
 * This Source Code Form is subject to the terms of the Mozilla Public      *
 * License, v. 2.0. If a copy of the MPL was not distributed with this      *
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.                 *
 ****************************************************************************/

/**
 * @file      BitArray.h
 * @author    TODO
 * @copyright MPL 2 
 * @see       https://github.com/sdds/sdds
 * simple 64-Bit bit array implementation
 */

#ifndef __BIT_ARRAY_H_INCLUDED__
#define __BIT_ARRAY_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Initialize this class
rc_t
BitArray_init();

//  Sets the bit at index to 1.
void
BitArray_set(uint64_t* self, unsigned int index);

//  Sets the bit at index to 0.
void
BitArray_clear(uint64_t* self, unsigned int index);

//  Toggles the bit at index
void
BitArray_toggle(uint64_t* self, unsigned int index);

//  Check if a bit at index is set. Returns true if bit is 1 and false if bit is
//  0.
bool
BitArray_check(uint64_t* self, unsigned int index);

#ifdef UTILS_DEBUG
//  Prints the array in 8-Bit chunks
void
BitArray_print(uint64_t* self);
#endif

#ifdef SDDS_SELFTEST
//  Selftest of this class
void
BitArray_test();
#endif

#ifdef __cplusplus
}
#endif

#endif