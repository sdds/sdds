/*  =========================================================================
    BitArray - simple 64-Bit bit array implementation

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#ifndef __BIT_ARRAY_H_INCLUDED__
#define __BIT_ARRAY_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Initialize this class
rc_t
   BitArray_init ();

//  Obtain a new instance of BitArray
BitArray_t *
   BitArray_new ();

//  Delete a instance of BitArray
BitArray_t *
   BitArray_delete (BitArray_t ** self_p);

//  Sets the bit at index to 1.
void
   BitArray_set (BitArray_t *self, unsigned int index);

//  Sets the bit at index to 0.
void
   BitArray_clear (BitArray_t *self, unsigned int index);

//  Toggles the bit at index
void
   BitArray_toggle (BitArray_t *self, unsigned int index);

//  Check if a bit at index is set. Returns true if bit is 1 and false if bit is
//  0.
bool
   BitArray_check (BitArray_t *self, unsigned int index);

//  Prints the array in 8-Bit chunks
void
   BitArray_print (BitArray_t *self);

//  Selftest of this class
void
   BitArray_test ();

#ifdef __cplusplus
}
#endif

#endif
