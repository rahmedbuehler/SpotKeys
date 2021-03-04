#define PY_SSIZE_T_CLEAN
#include <Python.h>
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

int keys()
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

int main(int argc, char* argv[])
{
    wchar_t* program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);  /* optional but recommended */
    Py_Initialize();
    PyRun_SimpleString("from time import time,ctime\n"
        "print('Today is', ctime(time()))\n");
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
    return 0;
}
