/*  =========================================================================
    BitArray - simple 64-Bit bit array implementation

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#include "sDDS.h"

//  ----------------------------------------------------------------------------
//  Initialize this class

rc_t
BitArray_init ()
{
	return SDDS_RT_OK;
}


//  ----------------------------------------------------------------------------
//  Sets the bit at index to 1.

void
BitArray_set (uint64_t* self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    *self |= 1ull << index;
}


//  ----------------------------------------------------------------------------
//  Sets the bit at index to 0.

void
BitArray_clear (uint64_t* self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    *self &= ~(1ull << index);
}


//  ----------------------------------------------------------------------------
//  Toggles the bit at index

void
BitArray_toggle (uint64_t* self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    *self ^= 1ull << index;
}


//  ----------------------------------------------------------------------------
//  Check if a bit at index is set. Returns true if bit is 1 and false if bit is
//  0.

bool
BitArray_check (uint64_t* self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    return (*self >> index) & 1ull;
}


//  ----------------------------------------------------------------------------
//  Prints the array in 8-Bit chunks

void
BitArray_print (uint64_t* self)
{
    assert (self);
    int index;
    printf ("BitArray: {");
    for (index = 63; index >= 0; index--) {
        if ((index + 1) % 8 == 0)
            printf ("\n%d   ", ((index + 1) / 8) - 1);
        printf ("%d ", BitArray_check (self, index) ? 1 : 0);
    }
    printf ("\n}\n");
}


//  ----------------------------------------------------------------------------
//  Selftest of this class

void
BitArray_test ()
{
    uint64_t bitArray = 0;

    //  Setting a bit
    BitArray_set (&bitArray, 0);
    assert (bitArray == 1ull);
    BitArray_set (&bitArray, 1);
    assert (bitArray == 3ull);
    BitArray_set (&bitArray, 2);
    assert (bitArray == 7ull);
    BitArray_print (&bitArray);

    //  Clearing a bit
    BitArray_clear (&bitArray, 0);
    assert (bitArray == 6ull);
    BitArray_print (&bitArray);

    //  Toggle a bit
    BitArray_toggle (&bitArray, 0);
    assert (bitArray == 7ull);
    BitArray_toggle (&bitArray, 1);
    assert (bitArray == 5ull);
    BitArray_print (&bitArray);
}
