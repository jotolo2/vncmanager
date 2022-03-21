#!/usr/bin/env python3
import sys

def read_info_log(file) :
    buffer = []
    while True :
        byte = file.read(1)
        eof = len(byte) == 0
        if eof or byte[0] == ord(':') :
            break
        hex_byte = ''.join([chr(byte[0]), chr(file.read(1)[0])])
        buffer.append(hex_byte)
    return buffer, eof

if __name__ == '__main__':
    if (len(sys.argv) <= 2) :
        sys.exit('Need a file to read!')

    packets_file_name = sys.argv[1]
    print('Reading packets from {:s}'.format(packets_file_name));

    types_file_name = sys.argv[2]
    print('Reading types from {:s}'.format(types_file_name));

    parsed_file_name = 'parsed_vncmanager.txt'

    types_file = open(types_file_name, 'rb')
    packets_file = open(packets_file_name, 'rb')
    parsed_file = open(parsed_file_name, 'w+')

    # Read types
    types_dic = {}
    eof = False

    while(not eof) :
        typename_buffer, eof = read_info_log(types_file)
        hash_buffer, eof = read_info_log(types_file)

        # Store types and their name
        type_hash = ''.join(hash_buffer)
        type_name = ''.join([chr(int(byte, 16)) for byte in typename_buffer])
        types_dic[type_hash] = type_name

    # Read packets
    eof = False

    while(not eof) :
        side_buffer, eof = read_info_log(packets_file)
        id_buffer, eof = read_info_log(packets_file)
        dir_buffer, eof = read_info_log(packets_file)
        type_buffer, eof = read_info_log(packets_file)
        len_buffer, eof = read_info_log(packets_file)
        content_buffer, eof = read_info_log(packets_file)

        # Print packets
        if (not eof) :
            side = 'Client' if int(side_buffer[0], 16) == ord('c') else 'Server'
            _id = int(id_buffer[0], 16)
            _dir = 'send' if int(dir_buffer[0], 16) == ord('s') else 'recv'
            _type = types_dic[''.join(type_buffer)]
            _len = int.from_bytes([int(byte, 16) for byte in len_buffer], sys.byteorder)
            content = ''.join(['{} '.format(byte) for byte in content_buffer])

            parsed_file.write('(side) : {}[{}] ({})\n'.format(side, _id, _dir))
            parsed_file.write('(type) : {}\n'.format(_type))
            parsed_file.write('(len)  : {}\n'.format(_len))
            parsed_file.write('(cont) : {}\n'.format(content))
            parsed_file.write('\n')

    types_file.close()
    packets_file.close()
    parsed_file.close()
