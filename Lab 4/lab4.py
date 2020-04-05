import os
from tqdm import tqdm

website_ls = [
    "www.csail.mit.edu", "www.berkeley.edu", "www.usyd.edu.au",
    "www.kyoto-u.ac.jp"
]

data_packet_ls = [56, 512, 1024]
result_ls = []

with open("result.txt", "w") as f:
    for website in tqdm(website_ls):
        for data_packet in data_packet_ls:
            command = f'ping -i 5 -c 10 -s {data_packet} {website}'
            print(f"executing {command}")
            result = os.popen(command).read().split("\n")
            print(f"result was {result}")
            f.write(f'{website}, {data_packet}, {result[-3:-1]}\n')