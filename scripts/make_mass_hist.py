#!/usr/bin/env python
"""
Read a list of ROOT files and produce histograms
"""
from __future__ import print_function
from __future__ import division

import ROOT
from root_numpy import root2array, fill_hist
import numpy as np

import multiprocessing as mp
import glob

def make_hists(event_info, hist_opts, outname, weight_name=None):
    outfile = ROOT.TFile.Open(outname, 'recreate')
    histograms = []

    for hist_opt in hist_opts:
        #print(hist_opt)
        if(len(hist_opt) < 5):
            print(hist_opt, "not enough info")
            continue

        h_type, h_name, h_low, h_high, h_nbins = hist_opt[:5]
        if len(hist_opt) > 5:
            h_xlabel, h_ylabel = hist_opt[5:7]
        else:
            h_xlabel, h_ylabel = None, None

        if 'TH1' in h_type:
            h1 = getattr(ROOT, h_type)(h_name, h_name, h_nbins, h_low, h_high)
            if h_xlabel:
                h1.SetXTitle(h_xlabel)
                h1.SetYTitle(h_ylabel)
        else:
            print("Not implemented")
            continue

        if weight_name:
            fill_hist(h1, event_info[h_name], weights=event_info[weight_name])
        else:
            fill_hist(h1, event_info[h_name])

        histograms.append(h1)

    for hist in histograms:
        hist.Write()

    outfile.Close()

if __name__ == "__main__":
    import argparse
    import os
    import yaml

    parser = argparse.ArgumentParser(description='mass histogram for Hmumu')
    add_arg = parser.add_argument
    add_arg('ntuple', type=str, help='ntuple')
    add_arg('bdt', type=str, help='BDT ntuple')
    add_arg('outname', type=str, help='output name')
    add_arg('-s', '--scale', type=float, help='scale the events',
           default=1.0)

    args = parser.parse_args()
    file_name = args.ntuple
    if not os.path.exists(file_name):
        print("{} not there".format(file_name))
        exit(1)

    bdt_file_name = args.bdt
    if not os.path.exists(bdt_file_name):
        print("{} not there".format(bdt_file_name))
        exit(1)

    out_name = args.outname
    scale = args.scale
    print(scale)

    tree_name = 'DiMuonNtuple'
    w_name = 'weight'
    mass_name = 'Muons_Minv_MuMu_Fsr_nearOnly'
    category_name = 'Event_XGB_fJVT_Category'

    selections = 'Muons_PT_Lead > 27 && Muons_PT_Sub > 15'
    branches =['Muons_PT_Lead', 'Muons_PT_Sub', mass_name, w_name]
    branches2 = [category_name]

    event = root2array(file_name, tree_name, branches)
    event2 = root2array(bdt_file_name, tree_name, branches2)
    print("{:,}, {:,}".format(event.shape[0], event2.shape[0]))
    mask = (event['Muons_PT_Lead'] > 27) & (event['Muons_PT_Sub'] > 15)

    fout = ROOT.TFile.Open(out_name, 'recreate')
    for icat in range(12):
        sel = (event2[category_name][mask] == icat+1)
        title = "DYHist_{}".format(icat+1)
        hist = ROOT.TH1F(title, title, 4000, 0, 400)
        fill_hist(hist, event[mass_name][mask][sel],
                  weights=event[w_name][mask][sel]*scale)
        hist.Write()

    fout.Close()

