iSFX is a sound board library and application that extends the functionality of 
FMOD. It has a C, C++, and a Python 3 interface. iSFX is a free and open source 
project that aims to produce a soundboard application to be used in improv
shows.

Folder Structure:
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

Code Structure:
  
  At the bottom of the stack, we have the FMOD Sound Library. A C++ wrapper
  was written around FMOD to allow features like fading-in and fading-out, 
  as well as outputting the waveform as an svg to a file. Python bindings 
  were created to allow the instantiation, destruction, and interaction with
  the C++ iSFX::Sound class. On the GUI, side you have Python at the bottom,
  keeping track of a listmodel of sounds loaded from sounds.xml. Python uses
  PyQt4 to display a QML gui to the user. The majority of code is in QML. The
  back-end takes a reasonable amount of code, but it isn't that complicated.

Building:

  In order to build iSFX, you need boost::signals2, which although header-only
  depends on a million other boost headers. Therefor it was not shipped with
  iSFX. The application itself is written in Python and QML, and doesn't need
  building. But Python imports iSFX(.so|.pyd). To build the iSFX library, cd
  to iSFX/Lib/Python, then run ./build.py. Build.py will run setup.py and 
  do some cleaning up. It will move the lib file to iSFX/Lib/Python/build as
  opposed to build/your_platform. To manually perform build.py's job, run
  setup.py (you may need to touch pyiSFX.cpp). Then find the build lib file
  and move it to Python/build.

Running:

  First, make sure iSFX was built, by checking iSFX/Lib/Python/build for a
  .so (mac/lin) file or .pyd (win) file. If there isn't, run follow the build
  instructions. Then, cd to iSFX/App and run main.py. At the beginning of
  main.py, it will copy the .pyd or .so from Lib/Python/build to the App
  folder. Then it runs the program.

Getting Started

  Once you are able to run iSFX, you need to add sounds to your library. You
  can do this by adding sounds to ../../sounds relative to main.py (or you
  can change the folder in the code, do find in project "../../sounds"). Add
  up to 130 sounds in that folder. If more than 130 sounds are found, the
  alphabetically last ones will be added, but will be cut off the display.

  Each time the program runs, it checks this folder and adds any new sounds.
  Sounds in the folder that are already in the library are ignored. It tracks
  the sounds by adding them to sounds.xml in the App folder. You can edit this
  manually if you like. Editing sounds.xml is currently the only way to remove
  a sound from the library.

  Once you have sounds in your library, iSFX will show them in the sound list.
  You can drag the sounds to reorder them by pressing and holding for about a
  second, or you can right click then drag immediately. When moving sounds 
  between columns, an overflowing column will overflow into the next column.
  The order of the sounds is preserved between program executions via the 
  sounds.xml file.

  By hovering your mouse over a sound, an edit icon will fade in. You can click
  this icon to start editing that sound. Then, at the bottom right of the app,
  there are a series of tabs, one of which is Edit. In this tab, you can change
  the display name of the sound, change the start and stop points, the fade-in
  and out duration, as well as other things. You should also see a huge 
  waveform of the sound. there should be a red and a blue line or region on the
  waveform. Those are resizable sliders, they set the start and fade-in 
  duration, and the stop and fade-out duration respectively. You can drag them
  around a resize them instead of editing the text input fields int the Edit
  tab. If you change the start position of a sound while it is playing, it will
  seek to that point if it is greater than the current playing position. There 
  is also a black line on the waveform that shows where the sound is currently.
  Due to popular demand, when you play a sound, it automatically set the edit
  focus to that sound.

  The sounds are divided up into columns. Each column is assigned an F key.
  F1 through F5 are used. Pressing them will change the focus to that column.
  The focused column is the column that has hotkeys displayed, and when you
  press the hotkey it will play the corresponding sound in the focused column.

  Other tabs: A timer tab was added to allow the user to time parts of an 
  improv show. The options tab contains options that apply to the application,
  or all the sounds. The options tab currently only contains the ability to
  turn off realtime waveform display. The options to set the default fade 
  durations will/should eventually be added, until then, they can be changed
  in SoundData.py.

Controls:
  
  F1-F5                 set the column focus.
  Esc                   stops all sounds without fading
  Space                 stops all sounds with fading
  a-z                   Plays/Stops corresponding sound in focused column
  left click            Play/Stops
  left click + hold     starts dragging
  right click           starts dragging

Dependency Limitations:
	The current version of PyQt4 does not allow the creation of QML objects
	like you can do in C++ by registering a qml type.
	
Future Development:
  
  It may be beneficial to switch to using PyQt4 for the GUI instead of using
  QML. The advantage of QML is that it would definitely be easier for a person
  used to web design to pick up. But that comes at the cost of not having many
  features implemented for you. For instance, taking a list and paginating it
  is fairly complicated (given variable height delegates). Even harder is 
  enabling drag and drop between those lists. QML does not have pagination or
  drag and drop implemented by default. Thus switching to PyQt4 for the more
  complicated GUI aspects may be a good idea.

  The rendering of SVGs takes quite a while. It may be a good idea to just
  graph the waveform with a slightly transparent pen using PyQt4 (see 
  qt's image provider).

To be implemented:
  - finish theming
  - paginated sound list, with two static columns on the left
  - categorize sounds (tags, colors, etc)
  - adding sounds in the program (have QML send PyQt4 a signal or use a context 
    object to pop up a load file window)
  - removing sounds from the program

Known Issues:
  - applying the highpass and lowpass filters while a sound is playing will
    sometimes (1 of 50) freeze the sound. It's probably FMOD's DSP filters
    fault, unless the method i'm using to connect and disconnect the filters
    is incorrect. The connect and disconnect code was taken from one of the
    examples that shipped with FMOD.
      - if you press escape, the sound will work next time you tell it to play
	- QML has awful support for text fields. After editing a text field,
	  you should press return or click out of it, to commit the text. It is
	  possible that after pressing return, your first click out of the text
	  field may not propagate to what you clicked on (e.g. the sound entry)

Potential Issues:
  - what happens when the sound file as stated in sounds.xml is not found?

If you have any questions about the code, you can contact me at
pdillon3@gatech.edu