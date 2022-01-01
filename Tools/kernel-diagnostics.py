#  WorkbenchOS
#  Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
#
#  SPDX-License-Identifier: GPL-3.0-only
import re

import binascii
from flask import Flask, send_from_directory, request, jsonify
from qmp import *
from struct import *

# set the project root directory as the static folder, you can set others.
app = Flask(__name__, static_url_path='/web')

monitor = QEMUMonitorProtocol(('127.0.0.1', 4444))
monitor.connect()


def convert_list_to_map(lst):
    return {lst[i][0]: lst[i][2] for i in range(0, len(lst))}


def parse_general_register(values, name):
    register = re.findall(r"{}=([0-9a-f]+)".format(name), values)
    return register[0]


def parse_descriptor_pointer(values, name):
    found = re.findall(r"{}=\s+([0-9a-f]+) ([0-9a-f]+)".format(name), values)
    register = found[0]
    return {
        'base': register[0],
        'limit': register[1]
    }


# Example: ES =0010 0000000000000000 00000000 00009300 DPL=0 DS   [-WA]
def parse_selector_register(values, name):
    found = re.findall(
        r"{}=([0-9a-f]+) ([0-9a-f]+) ([0-9a-f]+) ([0-9a-f]+) ([^\s]+) ([^\s]+)[ ]*([^\s]+)?".format(name), values)
    register = found[0]
    return {
        'selector': register[0],
        'base': register[1],
        'limit': register[2],
        'flags': register[3],
        'dpl': register[4],
        'type': register[5],
        'access_rights': register[6],
    }


def parse_qwords(line):
    results = re.findall(r"[0-9a-f]+: (.*)$", line)
    response = []

    if len(results) == 0:
        return response

    line_bytes = results[0].split(' ')
    for line_byte in line_bytes:
        got_bytes = bytes.fromhex(line_byte[2:])
        response += got_bytes

    return response


def get_registers():
    arguments = {'command-line': 'info registers'}
    output = monitor.cmd('human-monitor-command', arguments)

    registers_map = {'RAX': parse_general_register(output['return'], 'RAX'),
                     'RBX': parse_general_register(output['return'], 'RBX'),
                     'RCX': parse_general_register(output['return'], 'RCX'),
                     'RDX': parse_general_register(output['return'], 'RDX'),
                     'RSI': parse_general_register(output['return'], 'RSI'),
                     'RDI': parse_general_register(output['return'], 'RDI'),
                     'RBP': parse_general_register(output['return'], 'RBP'),
                     'RSP': parse_general_register(output['return'], 'RSP'),
                     'R8': parse_general_register(output['return'], 'R8 '),
                     'R9': parse_general_register(output['return'], 'R9 '),
                     'R10': parse_general_register(output['return'], 'R10'),
                     'R11': parse_general_register(output['return'], 'R11'),
                     'R12': parse_general_register(output['return'], 'R12'),
                     'R13': parse_general_register(output['return'], 'R13'),
                     'R14': parse_general_register(output['return'], 'R14'),
                     'R15': parse_general_register(output['return'], 'R15'),
                     'RIP': parse_general_register(output['return'], 'RIP'),
                     'CS': parse_selector_register(output['return'], 'CS '),
                     'DS': parse_selector_register(output['return'], 'DS '),
                     'ES': parse_selector_register(output['return'], 'ES '),
                     'FS': parse_selector_register(output['return'], 'FS '),
                     'GS': parse_selector_register(output['return'], 'GS '),
                     'LDT': parse_selector_register(output['return'], 'LDT'),
                     'TR': parse_selector_register(output['return'], 'TR '),
                     'GDT': parse_descriptor_pointer(output['return'], 'GDT'),
                     'IDT': parse_descriptor_pointer(output['return'], 'IDT'),
                     'CR0': parse_general_register(output['return'], 'CR0'),
                     'CR2': parse_general_register(output['return'], 'CR2'),
                     'CR3': parse_general_register(output['return'], 'CR3'),
                     'CR4': parse_general_register(output['return'], 'CR4'),
                     'DR0': parse_general_register(output['return'], 'DR0'),
                     'DR1': parse_general_register(output['return'], 'DR1'),
                     'DR2': parse_general_register(output['return'], 'DR2'),
                     'DR3': parse_general_register(output['return'], 'DR3'),
                     'DR6': parse_general_register(output['return'], 'DR6'),
                     'DR7': parse_general_register(output['return'], 'DR7'),
                     'EFER': parse_general_register(output['return'], 'EFER'),
                     }

    return registers_map


def get_virtual_memory(address, length):
    arguments = {'command-line': 'x/{}xb {}'.format(length, address)}
    output = monitor.cmd('human-monitor-command', arguments)
    memory_contents = output['return']
    qwords = []

    lines = memory_contents.split('\r\n')
    for line in lines:
        qwords += parse_qwords(line)
    return qwords


def get_physical_memory(address, length):
    arguments = {'command-line': 'xp/{}xb {}'.format(length, address)}
    output = monitor.cmd('human-monitor-command', arguments)
    memory_contents = output['return']
    qwords = []

    lines = memory_contents.split('\r\n')
    for line in lines:
        qwords += parse_qwords(line)
    return qwords


# struct PACKED InterruptDescriptor {
#     uint64_t offset : 16;
#     uint64_t segment_selector : 16;
#     uint64_t reserved : 8;
#     uint64_t type : 4;
#     uint64_t reserved_1 : 1;
#     uint64_t descriptor_privilege_level : 2;
#     uint64_t present : 1;
#     uint64_t offset_2 : 16;
#     uint32_t offset_3;
#     uint32_t reserved_2;
# };
def parse_idt_entry(entry):
    # print(len(entry), entry)
    entry_packed = bytes(entry)
    offset, segment, reserved, flags, offset_2, offset_3, reserved_2 = unpack('<HHccHII', entry_packed)
    offset_combined = offset | (offset_2 << 16) | (offset_3 << 32)
    type = flags[0] & 0xf
    descriptor_privilege_level = (flags[0] >> 5) & 0x3
    present = (flags[0] >> 7) & 0x1

    return {
        'offset': hex(offset_combined),
        'segment_selector': segment,
        'descriptor_privilege_level': descriptor_privilege_level,
        'present': present,
        'type': type,
    }


@app.route('/web/<filename>')
def static_files(filename):
    return send_from_directory('web', filename)


@app.route('/api/cpu/registers')
def registers():
    return get_registers()


@app.route('/api/kernel/pml4')
def pml4():
    regs = get_registers()
    pml4_address = "0x" + regs['CR3']
    return jsonify(get_physical_memory(pml4_address, 512 * 8))


@app.route('/api/kernel/idt')
def idt():
    regs = get_registers()
    idt_address = "0x" + regs['IDT']['base']
    idt_size = int(regs['IDT']['limit'], 16)
    # idt_entry_count = idt_size / 16
    idt_entries = get_virtual_memory(idt_address, idt_size)
    result = []
    for i in range(0, 20):
        entry_start = i * 16
        entry_end = entry_start + 16
        result.append(parse_idt_entry(idt_entries[entry_start:entry_end]))
    return jsonify(result)


@app.route('/api/memory/virtual/<address>')
def memory(address):
    length = request.args.get('length')
    if length is None:
        length = 1
    return jsonify(get_virtual_memory(address, length))


@app.route('/api/query-commands')
def query_commands():
    return monitor.cmd("query-commands")


if __name__ == "__main__":
    app.run()
