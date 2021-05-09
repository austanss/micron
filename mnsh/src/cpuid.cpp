#include "ecpuid.h"

bool cpuid_get_field(CPUID_REGISTER reg, CPUID_FIELD field)
{
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);

    switch (reg)
    {
        case ECX:
            return ecx & field;
            
        case EDX:
            return edx & field;

        default:
            return 0;
    }
}