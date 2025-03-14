#include <iostream>
#include <filesystem>
#include <getopt.h>
#include <cmath>

namespace fs = std::filesystem;

#ifdef DSIZE_VERSION
const char* version = DSIZE_VERSION;
#else
const char* version = "unknown";
#endif

struct HumanReadable
{
    std::uintmax_t size{};

    template<typename Os> friend Os& operator<<(Os& os, HumanReadable hr)
    {
        int i{};
        double mantissa = hr.size; // NOLINT(*-narrowing-conversions)
        for (; mantissa >= 1024.0; mantissa /= 1024.0, ++i) // NOLINT(*-flp30-c)
        {}
        os << std::ceil(mantissa * 10.0) / 10.0 << "BKMGTPE"[i];
        return i ? os << "B (" << hr.size << ')' : os;
    }
};

void printVersion(const char *progname) {
    std::cout << progname << " " << version << std::endl;
    std::cout << "Copyright (C) 2025 Jacob Freeman\n\n";
    std::cout << "This program is freely obtainable at https://github.com/Jacoblightning/DirSize.\nIf you paid for this program. You have been scammed.\n\n";
    std::cout <<
    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
    "This is free software: you are free to change and redistribute it.\n"
    "There is NO WARRANTY, to the extent permitted by law." << std::endl;
}

void printHelp(const char *progname) {
    std::cout <<
    "Usage: " << progname <<  " [OPTION]... [DIRECTORY]...\n"
    "Get the size of a directory recursively.\n\n"
    "  -h\tprint this help\n"
    "  -f\tfollow directory symlinks\n"
    "  -v\tbe more verbose\n"
    "  -V\toutput version information and exit\n\n"
    ""
    ""
    ""
    ""
    << std::endl;
}

void folderSize(const char* folder, const bool verbose, const bool follow_symlinks, const char *progname) {
    if (!std::filesystem::exists(folder)) {
        std::cerr << progname << ": cannot access '" << folder << "': No such file or directory" << std::endl;
        return;
    }
    if (!std::filesystem::is_directory(folder)) {
        std::cerr << progname << ": cannot access '" << folder << "': Not a directory" << std::endl;
        return;
    }
    uint64_t size = 0;

    for (
        const auto& iter
        :
        fs::recursive_directory_iterator(
            folder,
            (follow_symlinks?fs::directory_options::follow_directory_symlink:fs::directory_options::none)
            | fs::directory_options::skip_permission_denied // Fix race condition
            )
    )
    {
        try {
            if (iter.is_symlink() && ! follow_symlinks) {
                if (verbose)
                    std::cout << "Not following symlink: " << iter.path() << std::endl;
            } else if (iter.is_directory()) {
                if (verbose)
                    std::cout << "Traversing into directory: " << iter.path() << std::endl;
            } else {
                size += iter.file_size();
                if (verbose)
                    std::cout << "Found file: " << iter.path() << ". Size: " << HumanReadable{iter.file_size()} << std::endl;
            }
        } catch (fs::filesystem_error& e) {
            std::cerr << "Error on file/dir: " << iter.path() << ": " << e.what() << std::endl;
        }
    }

    std::cout << folder << ": " << HumanReadable{size} << std::endl;
}

int main(const int argc, char **argv) {
    const char *prog = argv[0];

    int opt;

    bool help = false;
    bool follow_symlink = false;
    bool verbose = false;
    bool version = false;
    // TODO: Optional human readable
    // TODO: Handle block and char devices

    int errorcode = 0;

    // TODO: Switch to getopt_long
    while ((opt = getopt(argc, argv, "hfvV")) != -1) {
        switch (opt) {
            case 'h':
                help = true;
                break;
            case 'f':
                follow_symlink = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'V':
                version = true;
                break;
            default:
                break;
        }
    }

    if (version) {
        printVersion(prog);
        return 0;
    }

    if (help) {
        printHelp(prog);
        return 0;
    }

    {
        for (int i = optind; i < argc; i++) {
            folderSize(argv[i], verbose, follow_symlink, prog);
        }
    }
    return errorcode;
}