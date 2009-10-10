# Top level targets that are defined in subsidiary SConscripts
#
# lib               -- build shared library object
# install           -- install everything under prefix directory
# install-include   -- install C++ header files
# install-lib       -- install shared library object

import os

# copy system environment variables related to compilation
DefaultEnvironment(ENV={
        'PATH' : os.environ['PATH'],
        'PYTHONPATH' : os.environ.get('PYTHONPATH', ''),
        'LD_LIBRARY_PATH' : os.environ.get('LD_LIBRARY_PATH', ''),
    }
)


# Create construction environment
env = DefaultEnvironment().Clone()

# Variables definitions below work only with 0.98 or later.
env.EnsureSConsVersion(0, 98)

# Customizable compile variables
vars = Variables('sconsvars.py')

vars.Add('tests', 'Custom list of unit test sources', None)
vars.Add(EnumVariable('build',
    'compiler settings', 'debug',
    allowed_values=('debug', 'fast')))
vars.Add(BoolVariable('profile',
    'build with profiling information', False))
vars.Add(PathVariable('prefix',
    'installation prefix directory', '/usr/local'))
vars.Update(env)
vars.Add(PathVariable('libdir',
    'object code library directory [prefix/lib]',
    env['prefix'] + '/lib'))
vars.Add(PathVariable('includedir',
    'installation directory for C++ header files [prefix/include]',
    env['prefix'] + '/include'))
vars.Update(env)
env.Help(vars.GenerateHelpText(env))

builddir = env.Dir('build/' + env['build'])

Export('env')

env.SConscript('src/SConscript', variant_dir=builddir, duplicate=0)

# vim: ft=python
