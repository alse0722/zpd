import os

dev = "0f1b6eef-4e80-4337-8030-5624a4bc3be7"


def get_mount_device():
    path = os.path.abspath(__file__)
    current_mount = f'df -h {path} | grep /dev'
    device = os.popen(current_mount)
    device_s = device.read()
    device_s = device_s.split(' ')[0]
    gruuid = device_s.split('/')[-1]
    out = f'ls -l /dev/disk/by-uuid/ | grep {gruuid}'
    stream = os.popen(out)
    output = stream.read().strip()
    getuuid = output.split(' ')[-3]
    return getuuid


def main():
    mount = "\"" + get_mount_device() + "\""
    if dev is None:
        os.system(f"sed -i'' '3 s/dev = None/dev = {mount}/' /home/sunshard/SomeRandom/studies/prog_security/3.py"
                  )
        print('Выполнена привязка к носителю')
    else:
        current_dev = "\"" + dev + "\""
        if mount == current_dev:
            print('Файл находится на привязанном носителе')
        else:
            print('Файл был перемещен с привязанного носителя')


if __name__ == '__main__':
    main()
