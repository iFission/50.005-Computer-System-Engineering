import os
from subprocess import Popen, PIPE
import asyncio
website_ls = [
    "www.csail.mit.edu", "www.berkeley.edu", "www.usyd.edu.au",
    "www.kyoto-u.ac.jp"
]

data_packet_ls = [56, 512, 1024]
result_ls = []


def ping(command):
    print(f"executing {command}")
    result = os.popen(command).read().split("\n")
    print(f"result was {result}")
    return True


def parse_command():
    for website in website_ls:
        for data_packet in data_packet_ls:
            yield f'ping -i 5 -c 10 -s {data_packet} {website}'
            # f.write(f'{website}, {data_packet}, {result[-3:-1]}\n')


def main():
    proc_ls = [Popen(command.split()) for command in parse_command()]
    for proc in proc_ls:
        proc.wait()


if __name__ == "__main__":
    main()