project('camera_controller_2', 'cpp', default_options : [ 'cpp_std=c++17'])

target = get_option('target')

inc = [include_directories('src', 'libs')]

# Sources
main = 'src/main.cpp'

src = [
       'src/cameracontroller/cameramanager/CameraManager.cpp',
       'src/camera/CameraWrapper.cpp',
       'src/camera/CameraWidget.cpp',
       'src/events/Event.cpp', 
       'src/events/EventBroker.cpp', 
       'src/utils/logger.cpp',
       'src/cameracontroller/CCEvents.cpp',
       'src/EventLogger.cpp'
       ]


# Test includes
inc_test = [include_directories('tests')]

# Catch tests
catch_main = 'tests/catch_main.cpp'

# List of catch entrypoints
src_catch = ['tests/camerawrapper.cpp']

# Other stand-alone tests
tests = ['tests/random_test.cpp']
src_tests = []


# Dependencies
deps = []

deps += dependency('threads')
deps += dependency('libgphoto2')
deps += dependency('libgphoto2_port')
deps += dependency('fmt')

if meson.is_cross_build()
       sysroot = meson.get_cross_property('sys_root', '')
       deps += meson.get_compiler('cpp').find_library('http_parser', dirs: sysroot + '/usr/local/lib')
else
       deps += meson.get_compiler('cpp').find_library('http_parser')
endif

if target=='app' or target=='all'
       executable('cc2', [main, src], include_directories : inc, dependencies : deps)
endif

if target=='test' or target=='all'
       # Catch
       executable('catch', [catch_main, src, src_catch], include_directories : [inc, inc_test], dependencies : deps)


       # Generic tests
       foreach m  : tests
              name = m.split('/')[-1].split('.')[0]
              executable('test_' + name, [m, src, src_tests],  include_directories : [inc, inc_test], 
                     dependencies : deps)
       endforeach    
endif
