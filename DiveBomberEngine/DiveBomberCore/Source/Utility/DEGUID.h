#pragma once
//#pragma warning(disable:26800)
#include <combaseapi.h>
//#pragma warning(default:26800)

__inline bool operator<(REFGUID guidOne, REFGUID guidOther)
{
    unsigned long data1, data2;

    //Swap to the correct order to be compared
    for (int i = 0; i < 4; i++)
    {
        data1 = ((unsigned long*)&guidOne)[i];
        data2 = ((unsigned long*)&guidOther)[i];
        if (data1 != data2)
            return data1 < data2;
    } // Next i

    return false;
}