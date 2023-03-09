#include <windows.h>
#include "pch.h"
#include <iostream>
using namespace std;
int main()
{
    if (OpenClipboard(NULL))
    {
        UINT format = 0;
        while ((format = EnumClipboardFormats(format)) != 0)
        {
            std::cout << "Format: " << format << std::endl;
        }
        CloseClipboard();
    }
    return 0;
}
