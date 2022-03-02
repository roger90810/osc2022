#include "mbox.h"

void get_board_revision(unsigned int* board_revision)
{
    __attribute__((aligned(16))) unsigned int mbox[7]; // need to aligned.
    mbox[0] = 4 * 7; // buffer size in bytes (4 Bytes * 7)
    mbox[1] = REQUEST_CODE;
    // tags begin
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] = 4; // maximum of request and response value buffer's length (4 Bytes).
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    // tags end
    mbox[6] = END_TAG;

    mbox_call(mbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.

    *board_revision = mbox[5]; // it should be 0xa020d3 for rpi3 b+
}

void get_ARM_memory(unsigned int* base_addr, unsigned int* size)
{
    __attribute__((aligned(16))) unsigned int mbox[8]; // need to aligned.
    mbox[0] = 4 * 8; // buffer size in bytes (4 Bytes * 7)
    mbox[1] = REQUEST_CODE;
    // tags begin
    mbox[2] = GET_ARM_MEMORY; // tag identifier
    mbox[3] = 8; // maximum of request and response value buffer's length (8 Bytes).
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer for base address.
    mbox[6] = 0; // value buffer for size.
    // tags end
    mbox[7] = END_TAG;

    mbox_call(mbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.

    *base_addr = mbox[5];
    *size = mbox[6];
}


/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned int *mbox, const unsigned char ch)
{
    // first, clear the lower 4-bit, then set the lower 4-bit with channel number.
    unsigned int addr_channel = (((unsigned int)((unsigned long) mbox) & ~0xF) | (ch & 0xF));
    
    /* wait until we can write to the mailbox */
    do {
        asm volatile("nop");
    } while (*MBOX_STATUS & MBOX_FULL);

    // write the address of our message to the mailbox with channel identifier
    *MBOX_WRITE = addr_channel;

    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do {
            asm volatile("nop");
        } while (*MBOX_STATUS & MBOX_EMPTY);

        /* is it a response to our message? */
        if (addr_channel == *MBOX_READ)
            /* is it a valid successful response? */
            return mbox[1] == MBOX_RESPONSE;
    }
    return 0;
}