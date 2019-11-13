#!/usr/bin/env python

import ROOT

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help='input h5 file')
    parser.add_argument('-t', '--treename', help='input h5 file', default="DiMuonNtuple")
    args = parser.parse_args()

    file_name = args.input
    tree_name = args.treename
    chain = ROOT.TChain(tree_name, tree_name)
    chain.Add(file_name)
    nentries = chain.GetEntries()
    print("{:,} events in {}".format(nentries, file_name))
