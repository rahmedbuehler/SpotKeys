# SpotKeys
A script that allows simple manipulation of Spotify's webplayer (play, pause, next, previous, volume up, and volume down) with keyboard shortcuts regardless of focus.

## Set Up

### Option 1 - MS Windows Only

1. Install dependencies:
   - Geckodriver 0.29+
   - Firefox 86+

2. Add path to Geckodriver and Firefox to SpotKeys_Settings.txt as well as path to any desired addon. For example,

   > FF_PATH = C:\Program Files (x86)\Mozilla Firefox\firefox.exe

   > GECKO_PATH = C:\Python\geckodriver.exe

   > ADDON_PATH = C:\Users\USER\AppData\Roaming\Mozilla\Firefox\Profiles\o3m64jnd.default-1234567890123\extensions\ADDON.xpi

   Spotify username and password are optional.
  
3. Run SpotKeys.exe.

### Option 2

1. Install dependencies:
   - Python 3.8+
   - Keyboard 0.13.5+ (Python Package)
   - Selenium 3.141+ (Python Package)
   - Geckodriver 0.29+
   - Firefox 86+

2. Place geckodriver.exe in the same folder as SpotKeys.py; add path to Firefox to SpotKeys_Settings.txt as well as path to any desired addon. For example,

   > FF_PATH = C:\Program Files (x86)\Mozilla Firefox\firefox.exe

   > ADDON_PATH = C:\Users\USER\AppData\Roaming\Mozilla\Firefox\Profiles\o3m64jnd.default-1234567890123\extensions\ADDON.xpi

   Spotify username and password are optional.

3. Run SpotKeys.py.
    - On Linux, the keyboard Python package needs root access in order to function.
        > sudo -E PATH=$PATH python3 SpotKeys.py
