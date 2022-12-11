#ifndef __MYALUMODULE_H__
#define __MYALUMODULE_H__

#include <stdint.h>

enum operation_t
{
    OPERATION_INVALID,
    OPERATION_NOT,
    OPERATION_OR,
    OPERATION_XOR,
    OPERATION_AND,
    OPERATION_ADD,
    OPERATION_SUB
};

struct compute_t
{
    enum operation_t operation;
    uint32_t operand1;
    uint32_t operand2;
};

struct result_t
{
    uint32_t value;
    uint32_t carry;
};

#define WR_VALUE _IOW('a', 'a', struct compute *)
#define RD_VALUE _IOR('a', 'b', struct result_t *)

#endif /* __MYALUMODULE_H__ */
