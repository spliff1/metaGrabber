
# metaGrabber

This app connects to a MySQL database, grabs a bunch of radio stations with their settings, grabs their current metadata (now playing) using CURL, saves data back to the database, and then goes to sleep.
## Installation

1. Libraries: libcurl, libmysqlclient, rapidjson, libconfig
- Install via your distribution's package manager (e.g., **apt**, **yum**)
2. C++ Compiler
- Debian/Ubuntu: Install the **build-essential** package.
- RPM-based (e.g., Fedora, CentOS): Install **gcc-c++**.

**Makefile should find lib paths automatically. If that doesn't happen, you may need to edit the Makefile to fit your system.**

## Configuration

1. Database:
Create a MySQL database with the following tables: 
- **stations**: id, name, meta_type, meta_url
- **meta**: id, station, artist, title, show_name, time
**Note: artist and show_name should be nullable.**
***meta_type can be either text or JSON. If text, it will work OOB. If JSON, you'll need to place the configuration inside config.txt (see examples)**

  **SHOUTCast is supported OOB:**
  
  If you set meta_type to **text** and meta_url to **https://HOSTNAME:PORT/currentsong?sid=1**, you'll get the current metadata.
  

2. Config File:
- Rename config.example.ini to config.ini.
- Update config.ini with your database connection details.


## Building
To build the app, run **make**, then execute it with **./main**. 
There is no need to install. You should keep the config file in the same directory as the binary.

## Compatibility

This application should work on most UNIX-based systems, including macOS (with potential minor adjustments to library paths). It should also run on any arm64 device.
## Contributing

Contributions are welcome! If you find a bug or have a feature idea, please open an issue or create a pull request.

## Used By

This app is used by `radiopenet.ro` to grab metadata from various radio stations around the world and show them on the website.
