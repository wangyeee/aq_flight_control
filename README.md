# AutoQuad Flight Control Firmware

This is a fork/continuation of [Bill Nesbitt's AutoQuad firmware](https://github.com/bn999/autoquad) for multicopter flight control.

* [AutoQuad Project Site](http://autoquad.org/)
* [AutoQuad Documetation](http://autoquad.org/wiki/wiki/)
* [AutoQuad Forums](http://forum.autoquad.org/)
* [AutoQuad Downloads Page](http://autoquad.org/software-downloads/)
* [AutoQuad Public FTP: ftp://ftp.autoquad.org/3/334693_22529/](ftp://ftp.autoquad.org/3/334693_22529/)

### USE AT YOUR OWN RISK!!! ABSOLUTELY NO RESPONSOBILITY IS ASSUMED BY THE AUTHORS OR PUBLISHERS OF THIS FIRMWARE. SEE LICENSE FILE FOR FULL TERMS.

#### Repository Structure:

  * `master` branch is current stable version.
  * `next` branch is to integrate all proposed changes for holistic testing before being committed to `master`.
  * Numbered branches (eg. `6.8`) are for older versions.
  * All other branches are "feature" branches used for testing individual changes before integration into `next`.

Pull requests should typically be submitted against the `next` branch, unless it is an important fix for something which affects `master` as well, or some other similarly good reason.

#### Binary Distributions

Pre-compiled firmware versions can be found at the [AutoQuad public FTP site: ftp://ftp.autoquad.org/3/334693_22529/aq-firmware/forks/mpaperno/aq_flight_control/](ftp://ftp.autoquad.org/3/334693_22529/aq-firmware/forks/mpaperno/aq_flight_control/).  The structure is organized by repository branch and then hardware version.

#### Getting The Source Code

If you already have a Git client installed (or are willing to learn), the easiest method is to clone this repository to your local machine. If you do not want to deal with Git, you may also download zip archives of the necessary projects.

##### Repository Checkout & Submodule Init/Update

This repository contains [Git submodules](http://blogs.atlassian.com/2011/12/git-submodules/) (for MAVLink headers) which must be initialized and updated. This is a simple process but you will get compile errors if you don't do it.

If you use a GUI Git client ([TortoiseGit](https://tortoisegit.org/docs/tortoisegit/tgit-dug-submodules.html), [SourceTree](https://blog.sourcetreeapp.com/2012/02/01/using-submodules-and-subrepositories/), etc) then look for options such as _recursive_ during cloning and commands like "Update Submodules". This is usually an option when (or after) you do a clone/checkout command. Refer to the program's help if necessary. After checking out the code, make sure the `lib/mavlink/include` folder exists.

It is also very easy to use the command line for clone, update, and checkout.

Here's a complete example starting with fresh copy of the repo, then checking out the `next` branch, and the submodule init:

```shell
git clone https://github.com/mpaperno/aq_flight_control.git
cd aq_flight_control
git checkout next
git submodule update --init
```

If you already have a clone of the repo and you only want to do a pull of the latest changes, run something like this (example uses the `master` branch and assumes "origin" for the remote name of this GitHub repo, which is a typical default):

```shell
git checkout master
git pull --recurse-submodules origin master
git submodule update --init
```

As you may have gathered by now, the point is to run `git submodule update` after cloning or updating the code from this repository.  The `--init` option is only necessary the first time, but it doesn't hurt to include it.

##### Download Code as Zip Archives

Unfortunately GitHub makes this a bit more complicated than it should be. To download a snapshot of the current code on any branch:

1. In the default GitHub _Code_ tab view, find the _Branch_ menu and select the branch you want to download.
2. To the right of the _Branch_ menu, click the _Download ZIP_ link and save the file (it will be named something like "aq\_flight\_control\_master.zip")
3. While still in the _Code_ view, click on the `lib` folder.
4. To the right of the `mavlink` folder there is an ID like "67a140b" or similar (7 hex digits).  Click on that. It will take you to a different code repository (AutoQuad/mavlink_headers) and to a specific commit in a specific branch (this is important).
5. Now click the _Download Zip_ link on this new page. Save the file (it will be named "mavlink\_headers-" with a long string of numbers at the end).
6. Unzip the "aq\_flight\_control\_master.zip" file into wherever you want to keep the firmware source code (preferably a directory path w/out spaces).
7. Unzip the "mavlink\_headers-xxxxx.zip" file into the `lib/mavlink` folder of the firmware source code tree.  So the final result should be a `lib/mavlink/include` folder with 2 subfolders and some .h files inside.


#### Compiling The Firmware

Download [GNU ARM Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) and follow its `readme.txt` to install the toolchain on your system.

##### Building with Eclipse:

Note: You will need [Java](https://jdk.java.net/) to run Eclipse.

1. Download and install [Eclipse CDT](https://www.eclipse.org/cdt/).
2. Install the [GNU MCU Eclipse plug-ins](https://gnu-mcu-eclipse.github.io/).
3. Open Eclipse and click `File` -> `New` -> `Makefile Project with Existing Code`. In the popup window, select the project folder in `Existing Code Location`. And select `ARM Cross GCC` in the `Toolchain for Indexer Settings` list. The click `Finish`.

4. Right click the project and click `Properties`. Select `C/C++ Build` on the left, then de-select `Use default build command` and set `Build command` to `make BUILD_TYPE=Debug MAV_VER=2 PROJ_ROOT=${CWD}`.
5. (Optional) Click `C/C++ General` -> `Preprocessor Include Paths` in the Properties window. Switch to `Providers` tab. In the list of providers, click `CDT Cross GCC Built-in Compiler Settings`. Under `Command to get compiler specs`, replace the text in `Command to get compiler specs` with `arm-none-eabi-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"`. Then click `CDT GCC Build Output Parser` and replace the text in `Compiler command pattern` with `arm-none-eabi-(g?cc)|([gc]\+\+)|(clang)`.

6. Right click the project and click `Build Project` to build.

##### Building with Makefile:

1. By default a release binary is built. Run `make BUILD_TYPE=Debug` to build debug firmware.
2. To change the AQ hardware version, pass `BOARD_VER` and `BOARD_REV` arguments to `make`.  Eg. `make BOARD_VER=8 BOARD_REV=6` to build for M4 rev 6 (M4 v2). Read the `Makefile` for full list of versions and revisions available.

###### Tips for using Makefile

1. Read the Makefile comments for more options, full list of build targets, and other information.
2. Use `-jX` for faster (parallel) compilation, where "X" equals the number of CPUs/cores/threads on your computer.  If you have `make` version 4+, also add the `-O` option for better progress output during compilation. Eg. `make -j8 -O` for a quad-core CPU.
3. The `Makefile.user` file is the right place to specify build options you typically want, then you can avoid entering them on the command-line each time, or editing the main `Makefile`. You can also set any variable in the environment and `make` will use it instead of the default in the `Makefile`. Command-line options always override all other variables.
4. All directory paths are relative to the location of the `Makefile`. You can use relative or absolute paths for most options.
4. You can easily set up a local environment and specify build options using a batch or shell file. This is especially useful for Windows so you can specify a local `PATH` variable w/out having to change the system-wide `PATH` (and need to restart Windows). The order of the `PATH` entries also affects how Windows searches for commands (making it easy to, for example, override Windows' `mkidir` with GNU tools `mkdir`). Here is a basic example batch file to initiate a build (this also shows using environment variables to set all the build options):

```batchfile
@echo off
set PATH=c:\devel\gcc-arm-none-eabi\bin;c:\devel\GnuWin\bin;C:\Windows\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem
set BUILD_TYPE=Release-M4.r6
set BOARD_VER=8
set BOARD_REV=6
make bin 
```

###### Notes for Windows Users:

You will need some GNU (Unix/Linux) tools installed and in your [`PATH`](http://www.howtogeek.com/118594/how-to-edit-your-system-path-for-easy-command-line-access/). Make sure to install them on a path with no spaces (eg. `c:/GnuWin/`) There are several good sources for these, including [GnuWin32 CoreUtils](http://gnuwin32.sourceforge.net/packages/coreutils.htm) and [ezwinports](http://sourceforge.net/projects/ezwinports/files/). The following utilities are required: 

`sh, make, gawk, mv, echo, rm, mkdir, expr, zip`. 

Most distributions include an older version of 'make' (3.x). Version 4.x offers some improvements for parallel builds. Windows versions are available from [ezwinports](http://sourceforge.net/projects/ezwinports/files/) (get a "w/out guile" version) or [Equation Solution](http://www.equation.com/servlet/equation.cmd?fa=make) (64 or 32 bit version, depending on your Windows type).

Note that all directory paths used by `make` should have forward slashes (`/`) instead of typical Windows backslashes (`\`).

#### Debug in Eclipse:

1. Download and install [OpenOCD](http://openocd.org/documentation/) from this [repo](https://github.com/gnu-mcu-eclipse/openocd/releases).
2. Build the firmware in Eclipse.
3. Click `Run` -> `Debug Configurations` and double click `GDB OpenOCD Debugging`. The a new target will be created under `GDB OpenOCD Debugging`, click the new target.
4. Check the elf binary is correct in the `Main` tab.
5. Switch to `Debugger` tab, check the `Start OpenOCD locally` option and select the executable path for OpenOCD. Type `-f openocd.cfg` in the `Config options` text field. Note that the `openocd.cfg` in this repo is for ST-Link v2 only. If you are using other debug probes, the `openocd.cfg` should be revised.
6. Click `Apply` button to save the configuration. The click `Debug` button to start a debug session.
