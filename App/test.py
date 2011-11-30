import iSFX
from time import sleep
from threading import Thread

system = iSFX.System()

def Update():
  while True:
    system.Update()
    sleep(0.015)
    
sound = iSFX.Sound(system, "../../sounds/lightsbassnectar.mp3", 0, "")

def LoadingChanged(l):
  if l == False:
    print("PLAY")
    sound.play()
         
sound.onLoadingChanged(LoadingChanged)
sound.load()

t = Thread(target=Update)
t.run()