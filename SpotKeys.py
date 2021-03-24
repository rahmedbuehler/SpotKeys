import keyboard
import os
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

class Profile:

    def __init__(self):
        self.username = None
        self.password = None
        self.ff_path = None
        self.gecko_path = None
        self.addon_path = None
        self.keys = {"Exit": "escape", "Previous":"alt+left", "Next":"alt+right", "Play/Pause":"alt+space", "Volume Up":"alt+up", "Volume Down":"alt+down"}

    def read_settings(self):
        '''
        Read settings from "SpotKeys_Settings.txt" and return <Profile> object.  Assumes settings file has keyword
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

    def output_key_bindings(self):
        '''
        Output key bindings from <keys> dictionary.
        '''
        print("Key Bindings")
        for k,v in self.keys.items():
            print(f"\t{k}: {v}")

class Hotkey_Tracker:

    def __init__(self):
        self.value = None

    def fire(self, new_value):
        '''
        Set <value> to <new_value>, unhooking hotkeys if this is "Exit".
        '''
        self.value = new_value
        if self.value == "Exit":
            keyboard.unhook_all()

    def clear(self):
        '''
        Reset <value> to <None> so long as it isn't "Exit"
        '''
        if self.value != "Exit":
            self.value = None

if __name__ == "__main__":
    settings = Profile()
    settings.read_settings()
    print("Starting SpotKeys...\n")
    settings.output_key_bindings()
    tracker = Hotkey_Tracker()
    keyboard.add_hotkey(settings.keys["Exit"], tracker.fire, args=["Exit"], suppress=True)
    keyboard.add_hotkey(settings.keys["Previous"], tracker.fire, args=["Previous"], suppress=True)
    keyboard.add_hotkey(settings.keys["Next"], tracker.fire, args=["Next"], suppress=True)
    keyboard.add_hotkey(settings.keys["Play/Pause"], tracker.fire, args=["Play/Pause"], suppress=True)
    keyboard.add_hotkey(settings.keys["Volume Up"], tracker.fire, args=["Volume Up"], suppress=True)
    keyboard.add_hotkey(settings.keys["Volume Down"], tracker.fire, args=["Volume Down"], suppress=True)

    original_uid = int(os.getenv("SUDO_UID"))
    os.setreuid(original_uid,original_uid) # Switch to non-root user (required by Firefox)
    options = Options()
    options.binary_location = settings.ff_path
    options.set_preference('media.gmp-manager.updateEnabled',True) # Needed to play DRM content
    driver = webdriver.Firefox(options = options)
    driver.get('https://accounts.spotify.com/en/login?continue=https:%2F%2Fopen.spotify.com%2F')

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

    last_url = ''
    initialized = False
    while tracker.value != "Exit":
        if last_url != driver.current_url:
            btns = driver.find_elements_by_xpath('''//div[@class='player-controls__buttons']//button''')
            volume_slider = driver.find_elements_by_css_selector('button.middle-align.progress-bar__slider')
            if len(btns) > 3 and len(volume_slider) > 1:
                volume_slider = volume_slider[1] # Currently 4 such Elements; want the 2nd
                volume_up = ActionChains(driver)
                volume_up.drag_and_drop_by_offset(volume_slider,10,0)
                volume_down = ActionChains(driver)
                volume_down.drag_and_drop_by_offset(volume_slider,-10,0)
                initialized = True
            else:
                initialized = False
        if initialized:
            if tracker.value == "Previous":
                btns[1].click()
            elif tracker.value == "Next":
                btns[3].click()
            elif tracker.value == "Play/Pause":
                btns[2].click()
            elif tracker.value == "Volume Up":
                volume_up.perform()
            elif tracker.value == "Volume Down":
                volume_down.perform()
            tracker.clear()
    driver.quit()
    print("\nExiting SpotKeys...\n")
