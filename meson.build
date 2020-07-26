project('camera_controller_2', 'cpp', default_options : [ 'cpp_std=c++17'])

arch = get_option('arch')
target = get_option('target')

inc_common = [include_directories('src')]

# Additional architecture-specific includes
inc_x86 = [include_directories('/usr/include/gphoto2')]
inc_arm = [include_directories('libraries/gphoto2')]

# Test includes
inc_test = [include_directories('libraries/catch2'),
              include_directories('tests')]

main = 'src/main.cpp'

src_common = [
       'src/camera/CameraController.cpp',
       'src/camera/wrapper/CameraWrapper.cpp',
       'src/camera/wrapper/CameraWidget.cpp',
       'src/events/Event.cpp', 
       'src/events/EventBroker.cpp', 
       'src/utils/logger.cpp',
       'src/CCEvents.cpp'
       ]

# Catch tests
catch_main = 'tests/catch_main.cpp'
src_catch = ['tests/camerawrapper.cpp']

# Generic tests
tests = ['tests/random_test.cpp']
src_tests = []

# Additional architecture-specific sources
src_x86 = []
src_arm = []

src_x86_tests = []
src_arm_tests = []

deps_common = [dependency('threads')]

# X86 Dependecies
deps_x86 = []

deps_x86 += meson.get_compiler('cpp').find_library('exif')
deps_x86 += meson.get_compiler('cpp').find_library('gphoto2_port')
deps_x86 += meson.get_compiler('cpp').find_library('gphoto2')

# ARM Dependencies
deps_arm = []

libsdir = meson.source_root() / 'libraries'

deps_arm += meson.get_compiler('cpp').find_library('exif', dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('gphoto2_port', 
                                                dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('gphoto2', dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('wiringPi', dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('ltdl', dirs: libsdir)

if target=='app' or target=='all'
       if arch=='x86' or arch=='all'
              executable(
                     'cc2-x86', 
                     [main, src_common, src_x86], 
                     include_directories : [inc_common, inc_x86], 
                     dependencies : [deps_common, deps_x86], 
                     native : true)
       endif
       if arch=='arm' or arch=='all'
              executable(
                     'cc2-arm',  
                     [main, src_common, src_arm], 
                     include_directories : [inc_common, inc_arm], 
                     dependencies : [deps_common, deps_arm])
       endif
endif

if target=='test' or target=='all'
       # Catch
       if arch=='x86' or arch=='all'
              executable('catch' + '-x86', 
                     [catch_main, src_common, src_x86, src_catch], 
                     include_directories : [inc_common, inc_x86, inc_test], 
                     dependencies : [deps_common, deps_x86], native : true)
       endif
       if arch=='arm' or arch=='all'
              executable('catch' + '-arm',  
                     [catch_main, src_common, src_arm, src_catch], 
                     include_directories : [inc_common, inc_arm, inc_test], 
                     dependencies : [deps_common, deps_arm])
       endif

       # Generic tests
       foreach m  : tests
              name = m.split('/')[-1].split('.')[0]
              if arch=='x86' or arch=='all'
                     executable('test-' + name + '-x86', 
                            [m, src_common, src_x86, src_tests, src_x86_tests], 
                            include_directories : [inc_common, inc_x86, inc_test], 
                            dependencies : [deps_common, deps_x86], native : true)
              endif
              if arch=='arm' or arch=='all'
                     executable('test-' + name + '-arm',  
                            [m, src_common, src_arm, src_tests, src_arm_tests], 
                            include_directories : [inc_common, inc_arm, inc_test], 
                            dependencies : [deps_common, deps_arm])
              endif
       endforeach    
endif
