#!/bin/usr/env python

if __name__ =="__main__":
    import h5py
    import numpy as np
    import matplotlib.pyplot as plt
    import argparse

    parser = argparse.ArgumentParser(description='plot weights in a ntuple')
    add_arg = parser.add_argument
    add_arg('file_name', type=str, help='input file')
    add_arg('outname', type=str, help='prefix of the output file')
    args = parser.parse_args()

    file_name = args.file_name
    outname = args.outname

    f = h5py.File(file_name, 'r')
    weight = f['event']['weight'].value
    min_val = np.min(weight)
    max_val = np.min(weight)
    plt.hist(weight, bins=100, range=(min_val, max_val), log=True)
    plt.savefig(outname)
