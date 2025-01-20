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
import platform

def subdictionary(d, keyset):
    return dict(kv for kv in d.items() if kv[0] in keyset)

def getsyspaths(*names):
    pall = sum((os.environ.get(n, '').split(os.pathsep) for n in names), [])
    rv = [p for p in pall if os.path.exists(p)]
    return rv

# copy system environment variables related to compilation
DefaultEnvironment(ENV=subdictionary(os.environ, '''
    PATH CPATH CPLUS_INCLUDE_PATH LIBRARY_PATH LD_RUN_PATH
    LD_LIBRARY_PATH DYLD_LIBRARY_PATH DYLD_FALLBACK_LIBRARY_PATH
    MACOSX_DEPLOYMENT_TARGET
    '''.split())
)

# Create construction environment
env = DefaultEnvironment().Clone()

# Variables definitions below work only with 0.98.1 or later.
env.EnsureSConsVersion(0, 98, 1)

# Customizable compile variables
vars = Variables('sconsvars.py')

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

if platform.system().lower() == 'windows':
    vars.Add(PathVariable(
        'libdir',
        'installation directory for compiled programs [prefix/Library/lib]',
        pjoin(env['prefix'], 'Library', 'Lib'),
        PathVariable.PathAccept))
    vars.Add(PathVariable(
        'includedir',
        'installation directory for C++ header files [prefix/Library/include]',
        pjoin(env['prefix'], 'Library', 'include'),
        PathVariable.PathAccept))
    vars.Add(PathVariable(
        'datadir',
        'installation directory for architecture independent data [prefix/share]',
        pjoin(env['prefix'], 'Library', 'share'),
        PathVariable.PathAccept))
    
    env['ENV']['TMP'] = os.environ['TMP']
    env.Append(CPPPATH=[pjoin(env['prefix'], 'Library', 'include')])
    env.Append(LIBPATH=pjoin(env['prefix'], 'Library', 'lib'))

    env.Append(CPPDEFINES=['_USE_MATH_DEFINES'])

else:
    # Installation paths
    vars.Add(PathVariable(
        'libdir',
        'installation directory for compiled library [prefix/lib]',
        pjoin(env['prefix'], 'lib'),
        PathVariable.PathAccept))
    vars.Add(PathVariable(
        'includedir',
        'installation directory for C++ header files [prefix/include]',
        pjoin(env['prefix'], 'include'),
        PathVariable.PathAccept))
    vars.Add(PathVariable(
        'datadir',
        'installation directory for architecture independent data [prefix/share]',
        pjoin(env['prefix'], 'share'),
        PathVariable.PathAccept))

    env.Append(CPPPATH=[pjoin(env['prefix'], 'include')])
    env.Append(LIBPATH=[pjoin(env['prefix'], 'lib')])
    


vars.Add(EnumVariable(
    'build',
    'compiler settings',
    'fast', allowed_values=('fast', 'debug', 'coverage')))
vars.Add(EnumVariable(
    'tool',
    'C++ compiler toolkit to be used',
    'default', allowed_values=('default', 'intelc')))
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

env.PrependUnique(LIBPATH=getsyspaths('LIBRARY_PATH'))
env.AppendUnique(CPPDEFINES='BOOST_ALL_NO_LIB')

env['has_objcryst'] = None
btags = [env['build'], platform.machine()]
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
