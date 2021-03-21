import keyboard
import os.path
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

Class Profile:

    def __init__(self):
        self.username = None
        self.password = None
        self.ff_path = None
        self.gecko_path = None
        self.addon_path = None
        self.keys = {"Exit": "escape", "Previous":"end", "Next":"page down", "Play/Pause":"insert", "Volume Up":"+", "Volume Down":"-"}

    def read_settings(self):
        '''
        Read settings from "SpotKeys_Settings.txt" and return <Settings> struct.  Assumes settings file has keyword
        followed by " = " and then desired value.
        '''
        if not os.path.isfile("SpotKeys_Settings.txt"):
            print("Settings file does not exist!")
        else:
            with open("SpotKeys_Settings.txt") as f:
                content = f.readlines()
            for line in content:
                if line.startswith("SPOTIFY_USERNAME"):
                    self.username = line.split("=")[1].strip()
                elif line.startswith("SPOTIFY_PASSWORD"):
                    self.password = line.split("=")[1].strip()
                elif line.startswith("FF_PATH"):
                    self.ff_path = line.split("=")[1].strip()
                elif line.startswith("GECKO_PATH"):
                    self.gecko_path = line.split("=")[1].strip()
                elif line.startswith("ADDON_PATH"):
                    self.addon_path = line.split("=")[1].strip()
                else:
                    print("Unexpected setting: ", line)

    def output_key_bindings(self)
        '''
        Output key bindings from <settings> using <key_id_labels>
        '''
        print("Key Bindings\n")
        for k,v in self.keys.items():
            print(k, ": ",v)

def clean_and_close(driver):
'''
Clean up hotkeys and exit.
'''
    print("\nExiting Spotkeys...\n")
    driver.quit()
    keyboard.unhook_all()
    exit()

if __name__ == "__main__":
    settings = Profile()
    settings.read_settings()
    options = Options()
    options.binary_location = settings.ff_path
    options.set_preference('media.gmp-manager.updateEnabled',True) # Needed to play DRM content
    driver = webdriver.Firefox(executable_path = settings.gecko_path, options = options)
    driver.get('https://accounts.spotify.com/en/login?continue=https:%2F%2Fopen.spotify.com%2F')
    last_url = ''
    if settings.addon_path != None:
            driver.install_addon(settings.addon_path,temporary=True)
    if settings.username != None:
            user_element = driver.find_element_by_id('login-username')
            user_element.clear()
            user_element.send_keys(settings.username)
        if settings.password != None:
            password_element = driver.find_element_by_id('login-password')
            password_element.clear()
            password_element.send_keys(settings.password)
            password_element.send_keys(Keys.RETURN)
    keyboard.add_hotkey(settings.keys["Exit"], clean_and_close(driver) )
    keyboard.add_hotkey(settings.keys["Previous"],btns[1].click())
    keyboard.add_hotkey(settings.keys["Next"],btns[3].click())
    keyboard.add_hotkey(settings.keys["Play/Pause"],btns[2].click())
    keyboard.add_hotkey(settings.keys["Volume Up"],volume_up.perform())
    keyboard.add_hotkey(settings.keys["Volume Down"],volume_down.perform())

