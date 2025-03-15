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

struct DS_options_t{
    bool help = false;
    bool version = false;
    bool follow_symlink = false;
    bool verbose = false;
    bool nohuman = false;
    int include_others = 0;
};

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
    "  -h, --help            \tprint this help\n"
    "  -v, --verbose         \tbe more verbose\n"
    "  -f, --follow-symlinks \tfollow directory symlinks\n"
    "  -m, --machine-readable\toutput machine readable format\n"
    "      --include-others  \tinclude block and character devices\n"
    ""
    ""
    "  -V, --version         \toutput version information and exit\n\n"
    "NOTES: Verbose output is sent to stderr.\n\n"
    << std::endl;
}

void folderSize(const char* folder, const char *progname, const DS_options_t& DS_options) {
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
            (DS_options.follow_symlink?fs::directory_options::follow_directory_symlink:fs::directory_options::none)
            | fs::directory_options::skip_permission_denied // Fix race condition
            )
    )
    {
        try {
            if (iter.is_symlink() && ! DS_options.follow_symlink) {
                if (DS_options.verbose)
                    std::cerr << "Not following symlink: " << iter.path() << std::endl;
            } else if (iter.is_directory()) {
                if (DS_options.verbose)
                    std::cerr << "Traversing into directory: " << iter.path() << std::endl;
            } else {
                size += iter.file_size();
                if (DS_options.verbose)
                    std::cerr << "Found file: " << iter.path() << ". Size: " << HumanReadable{iter.file_size()} << std::endl;
            }
        } catch (fs::filesystem_error& e) {
            std::cerr << "Error on file/dir: " << iter.path() << ": " << e.what() << std::endl;
        }
    }

    if (DS_options.nohuman)
        std::cout << folder << ":" << size << std::endl;
    else
        std::cout << folder << ": " << HumanReadable{size} << std::endl;
}

int main(const int argc, char **argv) {
    const char *prog = argv[0];

    int opt;

    DS_options_t DS_options;

    const option long_options[] = {
        {"help",             no_argument, nullptr,         'h'},
        {"machine-readable", no_argument, nullptr,         'm'},
        {"verbose",          no_argument, nullptr,         'v'},
        {"version",          no_argument, nullptr,         'V'},
        {"follow-symlinks",  no_argument, nullptr,         'f'},
        {"include-others",   no_argument, &DS_options.include_others, 1},
        {nullptr,            0,           nullptr, 0}
    };

    // TODO: Handle block and char devices

    int errorcode = 0;

    while ((opt = getopt_long(argc, argv, "hfvmV", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'h':
                DS_options.help = true;
                break;
            case 'f':
                DS_options.follow_symlink = true;
                break;
            case 'v':
                DS_options.verbose = true;
                break;
            case 'V':
                DS_options.version = true;
                break;
            case 'm':
                DS_options.nohuman = true;
                break;
            default:
                break;
        }
    }

    if (DS_options.version) {
        printVersion(prog);
        return 0;
    }

    if (DS_options.help || optind == argc) {
        printHelp(prog);
        return 0;
    }

    {
        for (int i = optind; i < argc; i++) {
            folderSize(argv[i], prog, DS_options);
        }
    }
    return errorcode;
}