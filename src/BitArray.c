/*  =========================================================================
    BitArray - simple 64-Bit bit array implementation

    Copyright (c) the Contributors as noted in the AUTHORS file.

    This file is part of sDDS:
    http://wwwvs.cs.hs-rm.de.
    =========================================================================
*/

#include "sDDS.h"

struct _BitArray_t {
    uint64_t array;
};

static BitArray_t objects[SDDS_BIT_ARRAY_MAX_OBJS];
static BitArray_t objectAllocation;


//  ----------------------------------------------------------------------------
//  Initialize this class

rc_t
BitArray_init ()
{
    uint8_t index;
    for (index = 0; index < SDDS_BIT_ARRAY_MAX_OBJS; index++)
        objects[index].array = 0ull;
    objectAllocation.array = 0ull;

	return SDDS_RT_OK;
}


//  ----------------------------------------------------------------------------
//  Obtain a new instance of BitArray

BitArray_t *
BitArray_new ()
{
    uint8_t index;
    for (index = 0; index < SDDS_BIT_ARRAY_MAX_OBJS; index++) {
        if (!BitArray_check (&objectAllocation, index)) {
            BitArray_set (&objectAllocation, index);
            return &objects[index];
        }
    }
    return NULL;
}


//  ----------------------------------------------------------------------------
//  Delete a instance of BitArray

BitArray_t *
BitArray_delete (BitArray_t ** self_p)
{
    assert (self_p);
    if (*self_p) {
        BitArray_t *self = *self_p;

        uint8_t index;
        for (index = 0; index < SDDS_BIT_ARRAY_MAX_OBJS; index++) {
            if (self == &objects[index]) {
                self->array = 0ull;
                BitArray_clear (&objectAllocation, index);
            }
        }
    }
}


//  ----------------------------------------------------------------------------
//  Sets the bit at index to 1.

void
BitArray_set (BitArray_t *self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    self->array |= 1ull << index;
}


//  ----------------------------------------------------------------------------
//  Sets the bit at index to 0.

void
BitArray_clear (BitArray_t *self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    self->array &= ~(1ull << index);
}


//  ----------------------------------------------------------------------------
//  Toggles the bit at index

void
BitArray_toggle (BitArray_t *self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    self->array ^= 1ull << index;
}


//  ----------------------------------------------------------------------------
//  Check if a bit at index is set. Returns true if bit is 1 and false if bit is
//  0.

bool
BitArray_check (BitArray_t *self, unsigned int index)
{
    assert (self);
    assert (index < 64);
    return (self->array >> index) & 1ull;
}


//  ----------------------------------------------------------------------------
//  Prints the array in 8-Bit chunks

void
BitArray_print (BitArray_t *self)
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
    BitArray_t BitArray;
    BitArray.array = 0;

    //  Setting a bit
    BitArray_set (&BitArray, 0);
    assert (BitArray.array == 1ull);
    BitArray_set (&BitArray, 1);
    assert (BitArray.array == 3ull);
    BitArray_set (&BitArray, 2);
    assert (BitArray.array == 7ull);
    BitArray_print (&BitArray);

    //  Clearing a bit
    BitArray_clear (&BitArray, 0);
    assert (BitArray.array == 6ull);
    BitArray_print (&BitArray);

    //  Toggle a bit
    BitArray_toggle (&BitArray, 0);
    assert (BitArray.array == 7ull);
    BitArray_toggle (&BitArray, 1);
    assert (BitArray.array == 5ull);
    BitArray_print (&BitArray);
}
