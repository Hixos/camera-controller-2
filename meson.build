project('camera_controller_2', 'cpp')

incdirs_common = [include_directories('src')]
incdirs_x86 = incdirs_common
incdirs_arm = incdirs_common

incdirs_x86 += [include_directories('/usr/include/gphoto2')]
incdirs_arm += [include_directories('libraries/gphoto2')]

src_common = ['src/main.cpp', 
       'src/events/Event.cpp', 
       'src/events/EventBroker.cpp', 
       'src/camera/CameraController.cpp',
       'src/camera/CameraWrapper.cpp']

src_x86 = src_common
src_arm = src_common

deps_common = []
deps_common += dependency('threads')

# X86 Dependecies
deps_x86 = deps_common

deps_x86 += meson.get_compiler('cpp').find_library('exif')
deps_x86 += meson.get_compiler('cpp').find_library('gphoto2_port')
deps_x86 += meson.get_compiler('cpp').find_library('gphoto2')

# ARM Dependencies
deps_arm = deps_common

libsdir = meson.source_root() / 'libraries'

deps_arm += meson.get_compiler('cpp').find_library('exif', dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('gphoto2_port', 
                                                dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('gphoto2', dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('wiringPi', dirs: libsdir)
deps_arm += meson.get_compiler('cpp').find_library('ltdl', dirs: libsdir)


executable('cc2-x86', src_x86, include_directories : incdirs_x86, 
           dependencies : deps_x86, native : true)
#executable('cc2-arm', src_arm, include_directories : incdirs_arm, dependencies : deps_arm)
