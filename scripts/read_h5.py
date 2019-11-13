#!/usr/bin/env python

if __name__ == "__main__":
    import h5py
    import numpy as np

    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='input h5 file')
    parser.add_argument('-n', '--numEvts', help='number of events', default=-1)
    args = parser.parse_args()

    file_name = args.input
    numEvts = args.numEvts

    f = h5py.File(file_name, 'r')
    tot_evts = f['event']['weight'].shape[0]
    print("{} contains {:,} events".format(file_name, tot_evts))
    if tot_evts < numEvts:
        raise("Not enough events")
    if numEvts < 0:
        numEvts = tot_evts
    print("Processing {} events".format(numEvts))

    tot_trials = np.sum(f['event']['trials'])
    tot_weight = np.sum(f['event']['weight'])
    xsection = tot_weight/tot_trials
    print("Cross section is: {} [pb]".format(xsection))

    for ievt in range(1):
        start = f['event']['start'][ievt]
        nparticles = f['event']['nparticles'][ievt]
        weight = f['event']['weight'][ievt]/tot_trials
        print("processing {}, weight {}".format(ievt, weight))
        for ip in range(nparticles):
            pid = f['particle']['id'][start+ip]
            px = f['particle']['px'][start+ip]
            py = f['particle']['py'][start+ip]
            pz = f['particle']['pz'][start+ip]
            m = f['particle']['m'][start+ip]
            print(pid, px, py, pz, m)
