# s7-300-simple-emulator
Single-threaded single-user demonstration version of emulator of the plc Siemens S7-300.
Emulator can correctly exchange data with TIAPortal. Emulator creates s7Comm packets that consists of header and data block.  Data blocks are the blocks that has been downloaded from real plc S7-300. For now only small part of the functionality is implemented: "Go online" connection with TIAPortal and  "Load from device" (to load blocks from device). 

