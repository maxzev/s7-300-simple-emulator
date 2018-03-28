# s7-300-simple-emulator
This is a single-thread single-user prototype version of emulator of plc Siemens S7-300.
Emulator can interconnects with TIAPortal and correctly answer on queries. As an answer an emulator creates correct packet that consists of header and data (body). Header is made by programm, data is loaded from database. Data are the blocks that has been downloaded from real plc S7-300. Now only small part of functionality is implemented: "Go online" connection with TIAPortal and  "Load from device" (to load blocks from device). 

If you are interested in more details or need blocks itself or have any other questions you may write us: max_zev@protonmail.com
