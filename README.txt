iSFX is a sound board library and application that extends the functionality of 
FMOD. It has a C, C++, and a Python interface. iSFX is a free and open source 
project that aims to produce a soundboard application to be used in improv
shows.

Structure:
/iSFX                           -- root folder
  /FMOD                         -- FMOD headers, libs, and docs
    /fmodex-documentation.chm   -- FMOD Documentation
    /inc                        -- FMOD Headers
    /lib                        -- FMOD Libraries (subfolders for platforms)
    /LICENSE.TXT                -- FMOD License
  /App                          -- A Soundboard application
  /Lib                          -- A library extending FMOD's capabilities
    /C++                        -- Lib iSFX C++
    /Python                     -- Lib iSFX Python
  /test                         -- test code / useful snippets