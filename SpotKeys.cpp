// SpotKeys.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <Windows.h>

int register_arrow(int id, UINT vk)
{
    if (RegisterHotKey(NULL, id, MOD_NOREPEAT, vk))
    {
        std::cout << "Hotkey "<< vk <<" registered.\n";
        return 0;
    }
    else
    {
        std::cout << "Error code " << GetLastError();
        return 1;
    }
}

int main()
{
    int id = 0;
    int arrows[4]{};
    arrows[0] = VK_LEFT;
    arrows[1] = VK_RIGHT;
    arrows[2] = VK_DOWN;
    arrows[3] = VK_UP;
    for (int i{ 0 }; i < 4; ++i)
    {
        register_arrow(i, arrows[i]);
    }
    
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        std::cout << "\nButton " << msg.wParam <<" Pressed";
        //if (msg.message == WM_HOTKEY)
            //std::cout << " -> WM_HOTKEY received";
    }
    return 0;
}
