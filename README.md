# SteerStone-Darkorbit
A C++ Emulator which emulates 2009 version of Darkorbit (before the UI make over).

## Requirements

In order to boot the emulator, you require the following tools/libaries below.

_Click on the link to be directed to download page_

### Tools
* [CMake]
* [Visual Studio 19]

### Libaries:
* [Boost 1.66]
* [OpenSSL 1.1]
* [MySQL C 5.6.44]
  * Note; Downloading a newer version than the one provided, could lead to unintentional results.

## Installation

### Step 1:

__A.__ Create a folder and create two sub folders; Source and Build such as:

```
SteerStone/
├── Source/
├── Build/
```

__B.__ Once you've created your folder(s), clone the source into your _Source_ folder.

### Step 2:


__A.__ Open CMake, and set the source directory to your _Source_ folder and set the build directory to your _Build_ folder.  
__B.__ Click Configure and then afterwards click build.

### Step 3:
__A.__ Go into your build folder and locate the file which is named: `SteerStone-Darkorbit.sln`

__B.__ Compile the project.

__C.__  Once compiled, go into build folder and find the folder bin and you should see the compiled binaries such as:
```
SteerStone/
├── Build/bin/Release/
```

### Step 4:

__A.__ Go into your source folder, into SQL Folder and then into Base folder such as:

```
SteerStone/
├── Source/sql/Base
```

__B.__ Query the `base.sql` into your MySQL Editor (for example: [HeidiSQL]).

### Step 5:

__A.__ Go back into your build directory where the compiled binaries are. Find `game.conf.dist`.  
__B.__ Find the MySQL Section settings and set the database username/password/database.  
__C.__ Rename `Policy.conf.dist` to `Policy.conf` do the same for Game aswell.

# Contact

You can contact me on discord (Exenu#2872) or open an issue.

<!-- Markdown link & img dfn's -->
[MySQL C 5.6.44]: https://dev.mysql.com/downloads/mysql/5.6.html
[Boost 1.66]: https://www.boost.org/users/history/version_1_66_0.html
[OpenSSL 1.1]: http://slproweb.com/products/Win32OpenSSL.html
[CMake]: https://cmake.org/download/
[Visual Studio 19]: https://visualstudio.microsoft.com/vs/
[Git Extensions]: https://sourceforge.net/projects/gitextensions/
[HeidiSQL]: https://www.heidisql.com/
