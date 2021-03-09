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

int read_settings()
// Read settings from "SpotKeys_Settings.txt"
{
	std::ifstream settings_file;
	settings_file.open("SpotKeys_Settings.txt");
	std::string FF_PATH{};
	std::string GECKO_PATH{};
	std::string ADDON_PATH{};
	if (settings_file.is_open())
	{
		std::string line{};
		while (std::getline(settings_file, line))
		{
			if (line.find("FF_PATH") != -1)
			{
				FF_PATH = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("GECKO_PATH") != -1)
			{
				GECKO_PATH = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("ADDON_PATH") != -1)
			{
				ADDON_PATH = line.replace(0, line.find(" = ") + 3, "");
			}
			else 
			{
				std::cout << "Unexpected Setting: " << line << "\n";
			}
		}
		settings_file.close();
	}
	return 0;
}

int main(int argc, char* argv[])
{
	read_settings();
	return 0;

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

	Py_SetProgramName(program);  /* optional but recommended */
	Py_Initialize();

	PyRun_SimpleString("from selenium import webdriver\n"
		"from selenium.webdriver.firefox.options import Options\n"
		"options = Options()\n"
		"options.binary_location = r'" + GECKO_PATH + "'\n"
		"driver = webdriver.Firefox(executable_path = r'" + FF_PATH + "', options = options)\n"
		"driver.install_addon(r'"+ ADDON_PATH + "',temporary=True)\n"
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
