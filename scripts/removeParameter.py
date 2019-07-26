#!/usr/bin/env python
__author__ = 'xiangyang ju'

import glob
import os
import sys
import subprocess

def remove(par_list):
    for dir_ in glob.glob('*'):
        file_ = os.path.join(dir_, 'used_params')
        new_out = ""
        with open(file_) as f:
            for line in f:
                if line.split()[0] in par_list:
                    continue
                new_out += line

        subprocess.call(['cp', file_, file_+'_bak'])
        with open(file_, 'w') as f:
            f.write(new_out)


if __name__ == "__main__":
    remove(['crRange'])
