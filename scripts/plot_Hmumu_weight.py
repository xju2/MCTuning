#!/usr/bin/env python

from root_numpy import root2array, fill_hist
import numpy as np
import ROOT
ROOT.gROOT.SetBatch(True)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='plot weights in a ntuple')
    add_arg = parser.add_argument
    add_arg('file_name', type=str, help='input file')
    add_arg('outname', type=str, help='prefix of the output file')
    add_arg('-w', '--w_name', type=str, help='weight name', default='McEventWeight')
    add_arg('-t', '--tree_name', type=str, help='tree name', default='DiMuonNtuple')

    args = parser.parse_args()
    file_name = args.file_name
    outname = args.outname
    tree_name = args.tree_name
    w_name = args.w_name

    event = root2array(file_name, tree_name, branches=[w_name])
    array = event[w_name]
    max_weight = np.max(array)
    min_weight = np.min(array)
    n_tot_evts = array.shape[0]
    print("Total {:,} events".format(n_tot_evts))
    n_evt_zeroweight = array[array==0].shape[0]
    print("\t {:,} ({:.1f}%) events with weight == zero".format(
        n_evt_zeroweight,
        n_evt_zeroweight*100/n_tot_evts))

    print("weight range: [{} - {}]".format(min_weight, max_weight))
    mean = np.mean(array)
    std = np.std(array)

    outfile = ROOT.TFile.Open("{}.root".format(outname), "recreate")
    h1 = ROOT.TH1F("h1", " ", 100, min_weight, max_weight)
    h2 = ROOT.TH1F("h2", " ", 100, min_weight, mean+std)
    h3 = ROOT.TH1F("h3", " ", 100, min_weight, mean+3*std)
    h5 = ROOT.TH1F("h5", " ", 100, min_weight, mean+5*std)
    fill_hist(h1, array)
    fill_hist(h2, array)
    fill_hist(h3, array)
    fill_hist(h5, array)

    canvas = ROOT.TCanvas("canvas", "canvas", 600, 600)
    h2.Draw()
    canvas.SaveAs("{}_1sigma.pdf".format(outname))
    h3.Draw()
    canvas.SaveAs("{}_3sigma.pdf".format(outname))
    canvas.SetLogy()
    h1.Draw()
    canvas.SaveAs("{}_full.pdf".format(outname))
    h5.Draw()
    canvas.SaveAs("{}_5sigma.pdf".format(outname))

    outfile.Write()
    outfile.Close()

