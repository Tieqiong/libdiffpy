MY_SCONS_HELP = """\
SCons build rules for the libdiffpy C++ library
Usage: scons [target] [var=value]

Targets:

lib                 build the shared library object [default]
install             install everything under prefix directory
install-lib         install the shared library object
install-include     install the C++ header files
install-data        install data files used by the library
alltests            build the unit test program "alltests"
test                execute unit tests (requires the cxxtest framework)
sdist               create source distribution tarball from git repository
zerocounters        remove cumulative coverage-count data

Build configuration variables:
%s
Variables can be also assigned in a user-written script sconsvars.py.
SCons construction environment can be customized in sconscript.local script.
"""

import os
from os.path import join as pjoin
from SCons.Script import *

def subdictionary(d, keyset):
    return dict(kv for kv in d.items() if kv[0] in keyset)

# copy system environment variables related to compilation
DefaultEnvironment(ENV=subdictionary(os.environ, '''
    PATH CPATH CPLUS_INCLUDE_PATH LIBRARY_PATH LD_RUN_PATH
    LD_LIBRARY_PATH DYLD_LIBRARY_PATH DYLD_FALLBACK_LIBRARY_PATH
    MACOSX_DEPLOYMENT_TARGET
    '''.split())
)

# Create construction environment
env = DefaultEnvironment().Clone()

# Variables definitions below work only with 0.98.1 or later
env.EnsureSConsVersion(0, 98, 1)

# Customizable compile variables
vars = Variables('sconsvars.py')

# Set PATHs
if 'PREFIX' in os.environ:
    vars.Add(PathVariable(
        'prefix',
        'installation prefix directory',
        os.environ['prefix']))
    vars.Update(env)
elif 'CONDA_PREFIX' in os.environ:
    vars.Add(PathVariable(
        'prefix',
        'installation prefix directory',
        os.environ['CONDA_PREFIX']))
    vars.Update(env)
else:
    vars.Add(PathVariable(
        'prefix',
        'installation prefix directory',
        '/usr/local'))
    vars.Update(env)

if env['PLATFORM'] == "win32":
    include_path = pjoin(env['prefix'], 'Library', 'include')
    lib_path = pjoin(env['prefix'], 'Library', 'lib')
    shared_path = pjoin(env['prefix'], 'Library', 'share')

    env['ENV']['TMP'] = os.environ['TMP']
else:
    include_path = pjoin(env['prefix'], 'include')
    lib_path = pjoin(env['prefix'], 'lib')
    shared_path = pjoin(env['prefix'], 'share')

vars.Add(PathVariable(
    'libdir',
    'installation directory for compiled programs',
    lib_path,
    PathVariable.PathAccept))
vars.Add(PathVariable(
    'includedir',
    'installation directory for C++ header files',
    include_path,
    PathVariable.PathAccept))
vars.Add(PathVariable(
    'datadir',
    'installation directory for architecture independent data',
    shared_path,
    PathVariable.PathAccept))

env.AppendUnique(CPPPATH=[include_path])
env.AppendUnique(LIBPATH=[lib_path])

# Customizable build variables
vars.Add(EnumVariable(
    'build',
    'compiler settings',
    'fast', allowed_values=('fast', 'debug', 'coverage')))
vars.Add(EnumVariable(
    'tool',
    'C++ compiler toolkit to be used',
    'default', allowed_values=('default', 'clangxx', 'intelc')))
vars.Add(BoolVariable(
    'enable_objcryst',
    'enable objcryst support, when installed', None))
vars.Add(BoolVariable(
    'profile',
    'build with profiling information', False))
vars.Add(
    'tests',
    'fixed-string patterns for selecting unit tests', None)
vars.Add(BoolVariable(
    'test_installed',
    'build tests using the installed library.', False))

vars.Update(env)

env.Help(MY_SCONS_HELP % vars.GenerateHelpText(env))

env['has_objcryst'] = None

btags = [env['build'], env['PLATFORM']]
if env['profile']:  btags.append('profile')
builddir = env.Dir('build/' + '-'.join(btags))

Export('env')

def GlobSources(pattern):
    """Same as Glob but also require that source node is a valid file.
    """
    rv = [f for f in Glob(pattern) if f.srcnode().isfile()]
    return rv

Export('GlobSources')

if os.path.isfile('sconscript.local'):
    env.SConscript('sconscript.local')

env.SConscript('src/SConscript', variant_dir=builddir)

# vim: ft=python
