#!/usr/bin/env python
"""
Inspects each loaded module, and reports whether the PEHeader
contains the relevant information indicating it is compatible
with Vista's ASLR implementation (DLLCharacteristics).

Rhys Kidd <rhyskidd@gmail.com> (c) 2007
"""
import immlib
from immutils import *
import struct

DESC = """Inspect modules for Vista ASLR compatibility"""

def main(args): 
    imm=immlib.Debugger()
    allmodules=imm.getAllModules()
    table=imm.createTable('ASLR /dynamicbase Table',[ 'Base', 'Name', 'DLLCharacteristics', 'Enabled?' ])
    for key in allmodules.keys():
        state = ""
        mod=imm.getModule(key)
        mzbase=mod.getBaseAddress()
        peoffset=struct.unpack('<L',imm.readMemory(mzbase+0x3c,4))[0]
        pebase=mzbase+peoffset
        flags=struct.unpack('<H',imm.readMemory(pebase+0x5e,2))[0]
        if (flags&0x0040)!=0:
            state = "ASLR Aware (/dynamicbase)"
        table.add(flags,["%08x"%mzbase, key, "0x%04x"%flags, state])
    return "Done!"      