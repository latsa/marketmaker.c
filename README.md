# marketmaker.c
-- Optimized Marketmaker backend based on SuperNET.

This code was extracted from the SuperNET code base and
cleaned up for the purpose of reimplementing it in Rust.
The source code in the `sources` and `include` directories
represent the baseline for reimplementation in Rust.

This version is not meant for production use; it is meant
as a reference, so the build process is not fully automatic.

Visual Studio 2015 project files are provided to build the
project for Win64. The Debug configuration will produce a
binary for testnet and the Release version will produce a
binary for the mainnet.

CMakeFiles.txt needs completion.

If you decide to build it, you'll need to update the submodules
first. Clone the repo, then run

`git submodule update --recursive`

Then, you can cmake .. all submodules except cryptopp, for which
support for cmake has been dropped for some time ago, and CMakeFiles.txt
has been removed. For cryptopp, the Visual Studio project files are readily
included.

Additional dependencies:  zlib, OpenSSL, curl, You are your own to install these.
Additional dependency for Windows: pthreads-win32 . You can clone

`https://github.com/latsa/pthread-win32.git`

into `external\pthread-win32` if needed.

Build marketmaker.exe with Visual Studio 2015 and
projects/VS2015/marketmaker/marketmaker.sln

Run

`build/VS2015/bin/Debug/marketmaker.exe` for testnet or
`build/VS2015/bin/Release/marketmaker.exe` for mainnet

to test it.

-- TODO

 - complete the devops tasks: automate the build; add unit tests; add ci config files
 - finish consolidating the cypto code base. ( almost done )

