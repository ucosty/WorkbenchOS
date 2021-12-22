#!/usr/bin/env python
#  WorkbenchOS
#  Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
#
#  SPDX-License-Identifier: GPL-3.0-only
import sys
import pefile

pe = pefile.PE(sys.argv[1])

for section in pe.sections:
    print(section.Name, hex(section.VirtualAddress), hex(section.Misc_VirtualSize), section.SizeOfRawData)
