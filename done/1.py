from functools import reduce
import os
import json


def file_to_blocks(file):
    with open(file, 'r') as f:
        data = f.read() + file
        digits = int.from_bytes(data.encode(), byteorder='big')
        to_split = str(digits)
        blocks = [to_split[i:i+32] for i in range(0, len(to_split), 32)]
        if len(blocks[-1]) != 32:
            blocks[-1] += '0' * (32-len(blocks[-1]))
    return [int(x) for x in blocks]


def hash(blocks):
    return reduce(lambda x, y: x ^ y, blocks)


def main():
    with open('studies\\prog_security\\last_state.json') as f:
        last_state = json.load(f)
        for root, dirs, files in os.walk('studies\\prog_security\\test'):
            for name in files:
                current_file = os.path.join(root, name)
                current_file_hash = hash(file_to_blocks(current_file))
                if current_file in last_state:
                    if last_state[current_file] == current_file_hash:
                        print(f'Файл {current_file} не изменился')
                    else:
                        print(f'Файл {current_file} изменился')
                else:
                    print(f'Новый файл {current_file}')

    with open('studies\\prog_security\\last_state.json', 'w') as f:
        state = {}
        for root, dirs, files in os.walk('studies\\prog_security\\test'):
            for name in files:
                state[os.path.join(root, name)] = hash(
                    file_to_blocks(os.path.join(root, name)))
        json.dump(state, f, indent=2)


if __name__ == '__main__':
    main()
