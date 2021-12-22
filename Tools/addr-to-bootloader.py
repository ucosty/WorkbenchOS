#!/usr/bin/env python
#  WorkbenchOS
#  Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
#
#  SPDX-License-Identifier: GPL-3.0-only
import sys

address = int(sys.argv[1], 16)
file_address = address + 0xc17c0000
print("{:#x}".format(file_address))
