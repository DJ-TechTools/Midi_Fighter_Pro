Midi_Fighter_Pro
================

Midi_Fighter_Pro

Notes:

1) The Midi Fighter Pro project requires LUFA to build. The current make file expects LUFA version 101122 to be located 
   in the parent of the working directory
   
2) To change which version of Midi Fighter Pro is being built you should edit the makefile on lines 204 to 208. 
   Uncomment the line containing the model type you want, and ensure all other models types are commented out. 
   
   ie - to build the Cuemaster
   
   # Cuemaster (00020000)
   CDEFS += -DTRAKTOR_H
   # Beatmasher (00020001)
   # CDEFS += -DTRAKTOR_V
   # Super knobs (00020002)
   # CDEFS += -DSERATO