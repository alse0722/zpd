from functools import reduce
import json


def file_to_blocks(file):
    with open(file, 'r', encoding='utf-8') as f:
        data = f.read()
        digits = int.from_bytes(data.encode(), byteorder='big')
        to_split = str(digits)
        blocks = [to_split[i:i+32] for i in range(0, len(to_split), 32)]
        if len(blocks[-1]) != 32:
            blocks[-1] += '0' * (32-len(blocks[-1]))
    return [int(x) for x in blocks]


def hash(blocks):
    return reduce(lambda x, y: x ^ y, blocks)


def main():
    with open('studies\\prog_security\\last_state_run.json', encoding='utf-8') as f:
        last_state = json.load(f)
        loc = __file__
        current_file_hash = hash(file_to_blocks(loc))
        if last_state[loc] == current_file_hash:
            print(f'Исполняемый файл не изменился')
        else:
            print(f'Исполняемый файл изменился')

    with open('studies\\prog_security\\last_state_run.json', 'w', encoding='utf-8') as f:
        loc = __file__
        state = {}
        state[loc] = hash(file_to_blocks(loc))
        json.dump(state, f, indent=2)


if __name__ == '__main__':
    main()
