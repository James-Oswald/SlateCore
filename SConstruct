import os
import sys

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

env_options = {
    "PLATFORM" : "mingw",
    "CC"    : "x86_64-w64-mingw32-gcc",
    "CXX"   : "x86_64-w64-mingw32-g++",
    "LD"    : "x86_64-w64-mingw32-ld",
    "AR"    : "x86_64-w64-mingw32-gcc-ar",
    "CPPFLAGS" : "-std=c++20",
    "SHLIBSUFFIX" : ".dll"
}

# tweak this if you want to use different folders, or more folders, to store your source code in.
env = Environment(**env_options)
env.VariantDir('scons_build', 'src', duplicate=0)
env.Append(CPPPATH=["include", "lib/rapidjson/include"])
sources = Glob("scons_build/*.cpp")
library = env.SharedLibrary("scons_build/slateCore", source=sources)

Default(library)