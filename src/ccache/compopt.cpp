// Copyright (C) 2010-2025 Joel Rosdahl and other contributors
//
// See doc/AUTHORS.adoc for a complete list of contributors.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "compopt.hpp"

#include <ccache/util/format.hpp>

// The option it too hard to handle at all.
int TOO_HARD = 1 << 0;

// The option it too hard for the direct mode.
int TOO_HARD_DIRECT = 1 << 1;

// The option takes a separate argument, e.g. "-D FOO=1".
int TAKES_ARG = 1 << 2;

// The option takes a concatenated argument, e.g. "-DFOO=1".
int TAKES_CONCAT_ARG = 1 << 3;

// The argument to the option is a path that may be rewritten if base_dir is
// used.
int TAKES_PATH = 1 << 4;

// The option only affects preprocessing; not included in the input hash in
// preprocessor mode.
int AFFECTS_CPP = 1 << 5;

// The option only affects compilation; not passed to the preprocessor.
int AFFECTS_COMP = 1 << 6;

struct CompOpt
{
  std::string_view name;
  int type;
};

const CompOpt compopts[] = {
  {"--Werror",                TAKES_ARG | AFFECTS_COMP                               }, // nvcc
  {"--analyzer-output",       TOO_HARD                                               }, // Clang
  {"--compiler-bindir",       AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"--compiler-options",      AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"--config",                TAKES_ARG                                              }, // Clang
  {"--em-config",             TAKES_ARG                                              }, // emcc
  {"--gcc-toolchain=",        TAKES_CONCAT_ARG | TAKES_PATH                          }, // Clang
  {"--include",               AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"--libdevice-directory",   AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"--offload-compress",      AFFECTS_COMP                                           }, // Clang
  {"--output-directory",      AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"--param",                 TAKES_ARG                                              },
  {"--save-temps",            TOO_HARD                                               },
  {"--save-temps=cwd",        TOO_HARD                                               },
  {"--save-temps=obj",        TOO_HARD                                               },
  {"--serialize-diagnostics", TAKES_ARG | TAKES_PATH                                 },
  {"--specs",                 TAKES_ARG                                              },
  {"-A",                      TAKES_ARG                                              },
  {"-AI",                     TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH              }, // msvc
  {"-B",                      TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH              },
  {"-D",                      AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG             },
  {"-E",                      TOO_HARD                                               },
  {"-EP",                     TOO_HARD                                               }, // msvc
  {"-F",                      AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-FI",                     AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH}, // msvc
  {"-FU",                     AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH}, // msvc
  {"-Fp",                     AFFECTS_CPP | TAKES_CONCAT_ARG | TAKES_PATH            }, // msvc
  {"-G",                      TAKES_ARG                                              },
  {"-I",                      AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-L",                      TAKES_ARG                                              },
  {"-M",                      TOO_HARD                                               },
  {"-MF",                     TAKES_ARG                                              },
  {"-MJ",                     TAKES_ARG | TOO_HARD                                   },
  {"-MM",                     TOO_HARD                                               },
  {"-MQ",                     TAKES_ARG                                              },
  {"-MT",                     TAKES_ARG                                              },
  {"-Tc",                     TAKES_ARG | TAKES_PATH | TOO_HARD                      }, // msvc
  {"-Tp",                     TAKES_ARG | TAKES_PATH | TOO_HARD                      }, // msvc
  {"-U",                      AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG             },
  {"-V",                      TAKES_ARG                                              },
  {"-Wa,",                    TAKES_CONCAT_ARG | AFFECTS_COMP                        },
  {"-Werror",                 AFFECTS_COMP                                           }, // don't exit with error when preprocessing
  {"-Wl,",                    TAKES_CONCAT_ARG | AFFECTS_COMP                        },
  {"-Wno-error",              AFFECTS_COMP                                           },
  {"-Xassembler",             TAKES_ARG | TAKES_CONCAT_ARG | AFFECTS_COMP            },
  {"-Xclang",                 TAKES_ARG                                              },
  {"-Xcompiler",              AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"-Xlinker",                TAKES_ARG | TAKES_CONCAT_ARG | AFFECTS_COMP            },
  {"-Xpreprocessor",          AFFECTS_CPP | TAKES_ARG                                },
  {"-Yc",                     AFFECTS_CPP | TAKES_CONCAT_ARG | TAKES_PATH            }, // msvc
  {"-Yu",                     AFFECTS_CPP | TAKES_CONCAT_ARG | TAKES_PATH            }, // msvc
  {"-all_load",               AFFECTS_COMP                                           },
  {"-analyze",                TOO_HARD                                               }, // Clang
  {"-arch",                   TAKES_ARG                                              },
  {"-ast-dump",               AFFECTS_COMP                                           }, // Clang
  {"-ast-dump-all",           AFFECTS_COMP                                           }, // Clang
  {"-ast-dump-all=",          AFFECTS_COMP | TAKES_CONCAT_ARG                        }, // Clang
  {"-ast-dump-decl-types",    AFFECTS_COMP                                           }, // Clang
  {"-ast-dump-filter",        AFFECTS_COMP | TAKES_ARG                               }, // Clang
  {"-ast-dump-lookups",       AFFECTS_COMP                                           }, // Clang
  {"-ast-dump=",              AFFECTS_COMP | TAKES_CONCAT_ARG                        }, // Clang
  {"-ast-list",               AFFECTS_COMP                                           }, // Clang
  {"-ast-merge",              TOO_HARD | TAKES_ARG                                   }, // Clang
  {"-ast-print",              AFFECTS_COMP                                           }, // Clang
  {"-ast-view",               TOO_HARD                                               }, // Clang
  {"-aux-info",               TAKES_ARG                                              },
  {"-b",                      TAKES_ARG                                              },
  {"-bind_at_load",           AFFECTS_COMP                                           },
  {"-bundle",                 AFFECTS_COMP                                           },
  {"-ccbin",                  AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"-emit-pch",               AFFECTS_COMP                                           }, // Clang
  {"-emit-pth",               AFFECTS_COMP                                           }, // Clang
  {"-external:I",
   AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH                           }, // msvc
  {"-fbuild-session-file=",   TAKES_CONCAT_ARG | TAKES_PATH                          },
  {"-fmodule-header",         TOO_HARD                                               },
  {"-fmodule-map-file=",      TAKES_CONCAT_ARG | TAKES_PATH                          },
  {"-fmodules-cache-path=",   TAKES_CONCAT_ARG | TAKES_PATH                          },
  {"-fmodules-ts",            TOO_HARD                                               },
  {"-fno-working-directory",  AFFECTS_CPP                                            },
  {"-fplugin=libcc1plugin",   TOO_HARD                                               }, // interaction with GDB
  {"-frepo",                  TOO_HARD                                               },
  {"-ftime-trace",            TOO_HARD                                               }, // Clang
  {"-fworking-directory",     AFFECTS_CPP                                            },
  {"-gcc-toolchain",          TAKES_ARG | TAKES_PATH                                 }, // Clang
  {"-gen-cdb-fragment-path",  TAKES_ARG | TOO_HARD                                   }, // Clang
  {"-gtoggle",                TOO_HARD                                               },
  {"-idirafter",              AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-iframework",             AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-imacros",                AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-imsvc",                  AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-imultilib",              AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-include",                AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-include-pch",            AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-include-pth",            AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-install_name",           TAKES_ARG                                              }, // Darwin linker option
  {"-iprefix",                AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-iquote",                 AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-isysroot",               AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-isystem",                AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-ivfsoverlay",            TAKES_ARG | TAKES_PATH                                 },
  {"-ivfsstatcache",          TAKES_ARG                                              },
  {"-iwithprefix",            AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH},
  {"-iwithprefixbefore",
   AFFECTS_CPP | TAKES_ARG | TAKES_CONCAT_ARG | TAKES_PATH                           },
  {"-ldir",                   AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"-link",                   TOO_HARD                                               }, // msvc
  {"-nolibc",                 AFFECTS_COMP                                           },
  {"-nostdinc",               AFFECTS_CPP                                            },
  {"-nostdinc++",             AFFECTS_CPP                                            },
  {"-odir",                   AFFECTS_CPP | TAKES_ARG                                }, // nvcc
  {"-pie",                    AFFECTS_COMP                                           },
  {"-prebind",                AFFECTS_COMP                                           },
  {"-preload",                AFFECTS_COMP                                           },
  {"-rdynamic",               AFFECTS_COMP                                           },
  {"-remap",                  AFFECTS_CPP                                            },
  {"-save-temps",             TOO_HARD                                               },
  {"-save-temps=cwd",         TOO_HARD                                               },
  {"-save-temps=obj",         TOO_HARD                                               },
  {"-specs",                  TAKES_ARG                                              },
  {"-stdlib=",                AFFECTS_CPP | TAKES_CONCAT_ARG                         },
  {"-trigraphs",              AFFECTS_CPP                                            },
  {"-u",                      TAKES_ARG | TAKES_CONCAT_ARG                           },
  {"-v",                      AFFECTS_COMP                                           },
  {"-wrapper",                TAKES_ARG | TOO_HARD                                   },
  {"-z",                      TAKES_ARG | TAKES_CONCAT_ARG | AFFECTS_COMP            },
};

static int
compare_compopts(const void* key1, const void* key2)
{
  const CompOpt* opt1 = static_cast<const CompOpt*>(key1);
  const CompOpt* opt2 = static_cast<const CompOpt*>(key2);
  return opt1->name.compare(opt2->name);
}

static int
compare_prefix_compopts(const void* key1, const void* key2)
{
  const CompOpt* opt1 = static_cast<const CompOpt*>(key1);
  const CompOpt* opt2 = static_cast<const CompOpt*>(key2);
  return opt1->name.substr(0, opt2->name.length()).compare(opt2->name);
}

static const CompOpt*
find(std::string_view option)
{
  CompOpt key{option, 0};
  void* result = bsearch(
    &key, compopts, std::size(compopts), sizeof(compopts[0]), compare_compopts);
  return static_cast<CompOpt*>(result);
}

static const CompOpt*
find_prefix(std::string_view option)
{
  CompOpt key{option, 0};
  void* result = bsearch(&key,
                         compopts,
                         std::size(compopts),
                         sizeof(compopts[0]),
                         compare_prefix_compopts);
  return static_cast<CompOpt*>(result);
}

// Used by unittest/test_compopt.cpp.
bool compopt_verify_sortedness_and_flags();

// For test purposes.
bool
compopt_verify_sortedness_and_flags()
{
  for (size_t i = 0; i < std::size(compopts); ++i) {
    if (compopts[i].type & TOO_HARD && compopts[i].type & TAKES_CONCAT_ARG) {
      PRINT(stderr,
            "type (TOO_HARD | TAKES_CONCAT_ARG) not allowed, used by {}\n",
            compopts[i].name);
      return false;
    }

    if (i == 0) {
      continue;
    }

    if (compopts[i - 1].name >= compopts[i].name) {
      PRINT(stderr,
            "compopt_verify_sortedness: {} >= {}\n",
            compopts[i - 1].name,
            compopts[i].name);
      return false;
    }
  }
  return true;
}

bool
compopt_affects_cpp_output(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & AFFECTS_CPP);
}

bool
compopt_affects_compiler_output(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & AFFECTS_COMP);
}

bool
compopt_too_hard(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & TOO_HARD);
}

bool
compopt_too_hard_for_direct_mode(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & TOO_HARD_DIRECT);
}

bool
compopt_takes_path(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & TAKES_PATH);
}

bool
compopt_takes_arg(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & TAKES_ARG);
}

bool
compopt_takes_concat_arg(std::string_view option)
{
  const CompOpt* co = find(option);
  return co && (co->type & TAKES_CONCAT_ARG);
}

bool
compopt_prefix_affects_cpp_output(std::string_view option)
{
  // Prefix options have to take concatenated args.
  const CompOpt* co = find_prefix(option);
  return co && (co->type & TAKES_CONCAT_ARG) && (co->type & AFFECTS_CPP);
}

bool
compopt_prefix_affects_compiler_output(std::string_view option)
{
  // Prefix options have to take concatenated args.
  const CompOpt* co = find_prefix(option);
  return co && (co->type & TAKES_CONCAT_ARG) && (co->type & AFFECTS_COMP);
}

std::optional<std::string_view>
compopt_prefix_takes_path(std::string_view option)
{
  // Prefix options have to take concatenated args.
  const CompOpt* co = find_prefix(option);
  if (co && (co->type & TAKES_CONCAT_ARG) && (co->type & TAKES_PATH)) {
    return option.substr(co->name.length());
  } else {
    return std::nullopt;
  }
}
