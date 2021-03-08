#define PY_SSIZE_T_CLEAN
#ifdef _DEBUG
	#undef _DEBUG
	#include <python.h>
	#define _DEBUG
#else
	#include <python.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

// Save cookie after logging in once? https://stackoverflow.com/questions/48880646/python-selenium-use-a-browser-that-is-already-open-and-logged-in-with-login-cre
// Option to read user/password from .txt file

int register_arrow(int id, UINT vk)
// Register hot key <vk> with <id>
{
	if (RegisterHotKey(NULL, id, MOD_NOREPEAT, vk))
	{
		return 0;
	}
	else
	{
		std::cout << "Error code " << GetLastError();
		return 1;
	}
}

void read_settings()
// Read settings from "SpotKeys_Settings.txt"
{
	std::ifstream settings_file;
	settings_file.open("SpotKeys_Settings.txt");
	if (settings_file.is_open())
	{
		std::string line{};
		while (std::getline(settings_file, line))
		{
			std::cout << line << '\n';
		}
		settings_file.close();
	}
	return 0;
}

int main(int argc, char* argv[])
{
	wchar_t* program = Py_DecodeLocale(argv[0], NULL);
	if (program == NULL) 
	{
		fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
		exit(1);
	}

	constexpr int arrows[5]{ VK_ESCAPE, VK_LEFT, VK_RIGHT, VK_DOWN, VK_UP };
	for (int i{ 0 }; i < 5; ++i)
	{
		register_arrow(i, arrows[i]);
	}

	read_settings()

	Py_SetProgramName(program);  /* optional but recommended */
	Py_Initialize();

	PyRun_SimpleString("from selenium import webdriver\n"
		"from selenium.webdriver.firefox.options import Options\n"
		"options = Options()\n"
		"options.binary_location = r''\n"
		"driver = webdriver.Firefox(executable_path = r'', options = options)\n"
		"driver.install_addon(r'',temporary=True)\n"
		"driver.get('https://open.spotify.com/')\n");

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		std::cout << "\nButton " << msg.wParam << " Pressed";
		if (msg.wParam == 0)
		{
			break;
		}
	}

	PyRun_SimpleString("driver.quit()\n");
	if (Py_FinalizeEx() < 0)
	{
		exit(120);
	}
	PyMem_RawFree(program);
	return 0;
}
