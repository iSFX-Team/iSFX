from distutils.core import setup, Extension
from sys import platform

if platform[0:3] == 'win':
  boost_path = 'C:\\Program Files (x86)\\boost_1_47'
  fmod_path = 'C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Programmers API Win' + platform[-2:] # 32 or 64

if platform == 'darwin':
  module1 = Extension('iSFX',
                      include_dirs = ['../../FMOD/inc',
                                      '/usr/local/include',
                                      '../C++'],
                      libraries = ['fmodex'],
                      library_dirs = ['../../FMOD/lib'],
                      sources = ['pyiSFX.cpp'])
                      
elif platform == 'linux':
  module1 = Extension('iSFX',
                      include_dirs = ['../../FMOD/inc',
                                      '/usr/local/include',
                                      '../C++'],
                      libraries = ['fmodex'],
                      library_dirs = ['../../FMOD/lib'],
                      sources = ['pyiSFX.cpp'])
                      
elif platform == 'win64' or platform == 'win32':
  module1 = Extension('iSFX',
                      include_dirs = ['../../FMOD/inc',
                                      'C:/Program Files (x86)/boost/boost_1_47',
                                      '../C++'],
                      libraries = ['fmodex'+platform[-2:]+'_vc'],
                      extra_link_args = ["/LIBPATH:"+fmod_path+"\\api\\lib",
                                         "/LIBPATH:"+boost_path+"\\lib",
                                         "/LIBPATH:"+boost_path+"\\lib64"],
                      extra_compile_args = ['/EHsc'],
                      library_dirs = [boost_path+'\\lib',
                                      boost_path+'\\lib64'],
                      sources = ['pyiSFX.cpp'])
else:
  print ("Error: unknown platform \"" + platform + "\"")
                      

setup (name = 'iSFX',
       version = '1.0',
       description = 'iSFX',
       ext_modules = [module1])
