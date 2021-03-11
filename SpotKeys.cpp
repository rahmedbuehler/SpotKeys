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

enum Key_ID {
	key_id_exit,
	key_id_previous,
	key_id_next,
	key_id_play,
	key_id_volume_up,
	key_id_volume_down,
	num_key_ids
};

struct Profile
{
	std::string username{};
	std::string password{};
	std::string ff_path{"C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe"};
	std::string gecko_path{ "C:\\Python\\geckodriver.exe" };
	std::string addon_path{};
	int keys[num_key_ids]{ VK_ESCAPE, VK_HOME, VK_END, VK_INSERT, VK_PRIOR, VK_NEXT };
};

Profile read_settings()
{
	// Read settings from "SpotKeys_Settings.txt" and return <Settings> struct
	Profile settings{};
	std::ifstream settings_file;
	settings_file.open("SpotKeys_Settings.txt");
	if (settings_file.is_open())
	{
		std::string line{};
		while (std::getline(settings_file, line))
		{
			if (line.find("SPOTIFY_USERNAME") != -1)
			{
				settings.username = line.replace(0, line.find(" = ") + 3, "");
			}
			else if (line.find("SPOTIFY_PASSWORD") != -1)
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

int main(int argc, char* argv[])
{
	Profile settings{ read_settings() };
	for (int key_id{ 0 }; key_id < num_key_ids; ++key_id)
	{
		register_key(key_id, settings.keys[key_id]);
	}

	wchar_t* program = Py_DecodeLocale(argv[0], NULL);
	if (program == NULL) 
	{
		fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
		exit(1);
	}
	Py_SetProgramName(program);
	Py_Initialize();
	
	std::stringstream ss;
	ss << "from selenium import webdriver\n"
		<< "from selenium.webdriver.firefox.options import Options\n"
		<< "from selenium.webdriver.common.keys import Keys\n"
		<< "from selenium.webdriver.common.action_chains import ActionChains\n"
		<< "options = Options()\n"
		<< "options.binary_location = r'" << settings.ff_path << "'\n"
		<< "options.set_preference('media.gmp-manager.updateEnabled',True)\n" // Needed to play DRM content
		<< "driver = webdriver.Firefox(executable_path = r'" << settings.gecko_path << "', options = options)\n"
		<< "driver.get('https://accounts.spotify.com/en/login?continue=https:%2F%2Fopen.spotify.com%2F')\n"
		<< "last_url = ''\n";
	if (settings.addon_path != "")
	{
		ss << "driver.install_addon(r'" << settings.addon_path << "',temporary=True)\n";
	}
	if (settings.username != "")
	{
		ss << "user_element = driver.find_element_by_id('login-username')\n"
			<< "user_element.clear()\n"
			<< "user_element.send_keys('" << settings.username << "')\n";
		if (settings.password != "")
		{
			ss << "password_element = driver.find_element_by_id('login-password')\n"
				<< "password_element.clear()\n"
				<< "password_element.send_keys('" << settings.password << "')\n"
				<< "password_element.send_keys(Keys.RETURN)\n";
		}
	}

	std::string py_program{ ss.str() };
	const char* c_py_program{ py_program.c_str() };
	PyRun_SimpleString(c_py_program);

	MSG msg = { 0 };
	BOOL msg_code;
	while ((msg_code = GetMessage(&msg, NULL, 0, 0)) != 0 )
	{
		// Error from GetMessage
		if (msg_code == -1) 
		{
			PyRun_SimpleString("driver.quit()\n");
			Py_FinalizeEx();
			PyMem_RawFree(program);
			return -1;
		}
		// Renew element tracking when url changes
		PyRun_SimpleString("if last_url != driver.current_url:\n"
			"\tbtns = driver.find_elements_by_xpath('''//div[@class='player-controls__buttons']//button''')\n"
			"\tvolume_slider = driver.find_elements_by_css_selector('button.middle-align.progress-bar__slider')[1]\n"//Currently 4 such Elements; want the 2nd
			"\tvolume_up = ActionChains(driver)\n"
			"\tvolume_up.drag_and_drop_by_offset(volume_slider,10,0)\n"
			"\tvolume_down = ActionChains(driver)\n"
			"\tvolume_down.drag_and_drop_by_offset(volume_slider,-10,0)\n");
		switch (msg.wParam)
		{
			case key_id_exit: 
				std::cout << "Exiting SpotKeys...";
				PyRun_SimpleString("driver.quit()\n"); //Exit
				if (Py_FinalizeEx() < 0)
				{
					exit(120);
				}
				PyMem_RawFree(program);
				return 0;
			case key_id_previous:
				PyRun_SimpleString("btns[1].click()\n"); // Previous
				break;
			case key_id_next:
				PyRun_SimpleString("btns[3].click()\n"); // Next
				break;
			case key_id_play:
				PyRun_SimpleString("btns[2].click()\n"); // Play/Pause
				break;
			case key_id_volume_up:
				PyRun_SimpleString("volume_up.perform()\n"); // Volume Up
				break;
			case key_id_volume_down:
				PyRun_SimpleString("volume_down.perform()\n"); // Volume Down
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
