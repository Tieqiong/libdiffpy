#!/usr/bin/env python3
"""
SConstruct - Modern SCons Build Script for a C++ Project

This script builds a shared library (or static library on Windows), runs tests,
and installs headers/libraries. It is organized into functions to maintain clarity
and modularity.
"""

import os
import sys
import platform
from SCons.Script import (DefaultEnvironment, Default, Alias, COMMAND_LINE_TARGETS,
                          AlwaysBuild, Glob, Exit, Help)

Import('env')

#------------------------------------------------------------------------------
# Configuration and Environment Setup
#------------------------------------------------------------------------------

def configure_environment():
    """
    Create the base build environment with standard tools and options.
    Modify this function to include extra tools or custom variables.
    """
    # Create an environment and import current shell environment variables.
    env = DefaultEnvironment(ENV=os.environ)

    # Set a default C++ standard and include directories.
    # Modern C++ projects typically use C++17 or later.
    if platform.system().lower() == "windows":
        env.Replace(CCFLAGS=['/std:c++17', '/W3'])
    else:
        env.Replace(CCFLAGS=['-std=c++17', '-Wall', '-Wextra'])
    
    # Setup search path for headers
    env.PrependUnique(CPPPATH=[os.path.join('#', 'include')])

    # Optionally, if you want to support different tools (like clang or Intel C++)
    tool_opt = env.get('tool', '').lower()  # You might pass this via the command line.
    if tool_opt == 'clang++':
        if not env.WhereIs('clang++'):
            print("Cannot locate clang++ compiler!")
            Exit(1)
        env.Tool('clang++')
    elif tool_opt == 'intelc':
        icpc = env.WhereIs('icpc')
        if not icpc:
            print("Cannot locate Intel C++ compiler (icpc)!")
            Exit(1)
        env.Tool('intelc')

    return env

#------------------------------------------------------------------------------
# Build Targets
#------------------------------------------------------------------------------

def build_library(env):
    """
    Build the main project library. For non-Windows builds we create a shared library,
    while on Windows we typically create a static library.
    """
    # Collect source files from the 'src' directory.
    sources = Glob('src/*.cpp')

    if platform.system().lower() == "windows":
        # Windows: build a static library
        lib = env.StaticLibrary(target='myproject', source=sources)
    else:
        # Other platforms: build a shared library
        lib = env.SharedLibrary(target='myproject', source=sources)
    
    # Make sure cleanup removes any extra files (e.g., coverage data)
    env.Clean(lib, Glob('src/*.gc*'))
    return lib

def build_tests(env):
    """
    Build test executables if the test target is requested.
    Assumes test source files are in the 'tests' directory.
    """
    test_sources = Glob('tests/*.cpp')
    if not test_sources:
        print("No tests found in 'tests/' directory.")
        return None

    # Create a test program – you can extend this to include additional test frameworks.
    test_prog = env.Program(target='run_tests', source=test_sources)
    
    # Optionally add a dependency on the library if tests use the library.
    env.Depends(test_prog, build_library(env))
    return test_prog

#------------------------------------------------------------------------------
# Installation Targets
#------------------------------------------------------------------------------

def setup_installation(env, lib):
    """
    Setup installation targets for the built library and header files.
    Modify directory names as needed.
    """
    prefix = env.get('prefix', '/usr/local')
    lib_install_dir = os.path.join(prefix, 'lib')
    include_install_dir = os.path.join(prefix, 'include', 'myproject')

    # Install the built library.
    lib_inst = env.Install(lib_install_dir, lib)

    # Install header files from the 'include' directory.
    headers = Glob('include/*.h')
    include_inst = env.Install(include_install_dir, headers)

    # Create an alias that builds everything needed for installation.
    install_alias = Alias('install', [lib_inst, include_inst])
    return install_alias

#------------------------------------------------------------------------------
# Main Build Flow
#------------------------------------------------------------------------------

def main():
    # Configure the environment.
    env = configure_environment()

    # Build the core library.
    lib = build_library(env)
    Default(lib)

    # Check for the 'test' target.
    if 'test' in COMMAND_LINE_TARGETS or 'run_tests' in COMMAND_LINE_TARGETS:
        test_prog = build_tests(env)
        if test_prog:
            Alias('test', test_prog)

    # Setup installation targets if requested.
    if 'install' in COMMAND_LINE_TARGETS:
        setup_installation(env, lib)

    # Optionally, add targets for cleaning or coverage.
    # For example, to remove coverage counters you might have:
    coverage_files = Glob('src/*.gc*') + Glob('tests/*.gc*')
    Alias('clean-coverage', [env.Clean('.', coverage_files)])
    AlwaysBuild('clean-coverage')

if __name__ == '__main__':
    main()
