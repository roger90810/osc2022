#ifndef __STDLIB_H
#define __STDLIB_H

#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
void *simple_malloc(unsigned long);

#endif