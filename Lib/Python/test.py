import curses
import iSFX

system = iSFX.System()
sound = iSFX.Sound(system, "../../sounds/Bassnectar-CozzaFrenzy.mp3")
sound.play()

stdscr = curses.initscr()
curses.noecho()
stdscr.timeout(10)

position = 0;

def PrintPos(p):
  global position
  position = p
  stdscr.addstr(0,0, str(int(p/60000)).zfill(2)+":"+str(int((p%60000)/1000)).zfill(2)+"."+str(p%1000).ljust(3, '0') )
  
sound.setPositionCallback(PrintPos)

while 1:
  c = stdscr.getch()
  #stdscr.clear()
  system.Update()
  if (c == -1):
    continue
  if (chr(c) == 'p'):
    sound.play()
  elif (chr(c) == 's'):
    sound.stop()
  elif (chr(c) == 'u'):
    sound.unpause()
  elif (chr(c) == 'q'):
    break
  elif (chr(c) == 'f'):
    sound.fade(-1.0, 1000)
  elif (chr(c) == 'g'):
    sound.fade(1.0, 1000)
  elif (chr(c) == 'r'):
    sound.fade(-1.0, 1000, then="pause")
  elif (chr(c) == 't'):
    sound.fade(-1.0, 1000, then="stop")
  elif (chr(c) == ']'):
    sound.setPositionMs(position+1000)
  elif (chr(c) == '['):
    sound.setPositionMs(position-1000)
curses.endwin()