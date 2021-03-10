#define PY_SSIZE_T_CLEAN
#ifdef _DEBUG
	#undef _DEBUG
	#include <python.h>
	#define _DEBUG
#else
	#include <python.h>
#endif

#include <iostream> // for std:cout
#include <fstream> // for std::ifstream
#include <iterator> // for std::ssize
#include <string>
#include <sstream>
#include <Windows.h>

int register_key(int id, UINT vk)
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

struct Profile
{
	std::string username{};
	std::string password{};
	std::string ff_path{};
	std::string gecko_path{};
	std::string addon_path{};
};

Profile read_settings()
// Read settings from "SpotKeys_Settings.txt" and return <Settings> struct
// Sanitize input?
{
	Profile settings{};
	std::ifstream settings_file;
	settings_file.open("SpotKeys_Settings.txt");
	if (settings_file.is_open())
	{
		std::string line{};
		while (std::getline(settings_file, line))
		{
			if (line.find("USERNAME") != -1)
			{
				settings.username = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("PASSWORD") != -1)
			{
				settings.password = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("FF_PATH") != -1)
			{
				settings.ff_path = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("GECKO_PATH") != -1)
			{
				settings.gecko_path = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("ADDON_PATH") != -1)
			{
				settings.addon_path = line.replace(0, line.find(" = ") + 3, "");
			}
			else 
			{
				std::cout << "Unexpected Setting: " << line << "\n";
			}
		}
		settings_file.close();
	}
	return settings;
}

int main(int argc, char* argv[])
{
	Profile settings{ read_settings() };

	wchar_t* program = Py_DecodeLocale(argv[0], NULL);
	if (program == NULL) 
	{
		fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
		exit(1);
	}

	constexpr int keys[]{ VK_ESCAPE, VK_LEFT, VK_RIGHT, VK_NUMPAD0, VK_DOWN, VK_UP };

	for (int key : keys)
	{
		register_key(key,key);
	}

	Py_SetProgramName(program);  /* optional but recommended */
	Py_Initialize();


	
	std::stringstream ss;
	ss << "from selenium import webdriver\n"
		<< "from selenium.webdriver.firefox.options import Options\n"
		<< "from selenium.webdriver.common.keys import Keys\n"
		<< "options = Options()\n"
		<< "options.binary_location = r'" << settings.ff_path << "'\n"
		<< "options.set_preference('media.gmp-manager.updateEnabled',True)\n" // Needed to play DRM content
		<< "driver = webdriver.Firefox(executable_path = r'" << settings.gecko_path << "', options = options)\n"
		<< "driver.install_addon(r'" << settings.addon_path << "',temporary=True)\n"
		<< "driver.get('https://accounts.spotify.com/en/login?continue=https:%2F%2Fopen.spotify.com%2F')\n"
		<< "user_element = driver.find_element_by_id('login-username')\n"
		<< "user_element.clear()\n"
		<< "user_element.send_keys('" << settings.username << "')\n"
		<< "password_element = driver.find_element_by_id('login-password')\n"
		<< "password_element.clear()\n"
		<< "password_element.send_keys('" << settings.password << "')\n"
		<< "password_element.send_keys(Keys.RETURN)\n";

	std::string py_program{ ss.str() };
	const char* c_py_program{ py_program.c_str() };
	PyRun_SimpleString(c_py_program);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0) // Error check is suggested here
	{
		PyRun_SimpleString("btns = driver.find_elements_by_xpath('''//div[@class='player-controls__buttons']//button''')\n");
		switch (msg.wParam)
		{
			case VK_ESCAPE:
				std::cout << "Exiting SpotKeys...";
				PyRun_SimpleString("driver.quit()\n");
				if (Py_FinalizeEx() < 0)
				{
					exit(120);
				}
				PyMem_RawFree(program);
				return 0;
			case VK_LEFT:
				std::cout << "VK_LEFT Pressed";
				PyRun_SimpleString("btns[1].click()\n");
				break;
			case VK_RIGHT:
				std::cout << "VK_RIGHT Pressed";
				PyRun_SimpleString("btns[3].click()\n");
				break;
			case VK_NUMPAD0:
				std::cout << "VK_NUMPAD0 Pressed";
				PyRun_SimpleString("btns[2].click()\n");
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
