#!/usr/bin/env python
# -*- coding: utf-8 -*-

import yoda
import os
import sys

def compare_yoda(ref, tuned_dict, options=None):
    """
    compare ref data with and tuned ones
    """
    if len(tuned_dict.values()) < 1:
        return

    plotkeys = {
        "xlabel": "",
        "ylabel": "Events / Bin",
        "logY": True,
        "title": "validation",
        "ratioylabel": "Tuned / Ref",
        "errorbars": True,
        "ratioymin": 0.1,
        "ratioymax": 2,
        'ratiogrid': True
    }

    if options is None:
        options = dict()

    tuned = tuned_dict.values()[0]

    output_dir = options.get('output_dir', "validation")

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for key in sorted(tuned.keys()):
        hists_list = []
        try:
            ref_hist = ref['/REF'+key]
            ref_hist.setAnnotation("ratioref", True)
            ref_hist.setAnnotation("Title", "Ref. data")
        except KeyError:
            print "/REF"+key+" not found"
            break

        title = ref_hist.path.split('/')[-1]
        hists_list.append(ref_hist)
        plotkeys['xlabel'] = title

        for label, yoda_handle in tuned_dict.iteritems():
            tuned_hist = yoda_handle[key]
            # print tuned_hist
            if tuned_hist:
                tuned_hist.setAnnotation("Title", label)
                hists_list.append(tuned_hist)
            else:
                print title," missed tuned data"

        out_name = output_dir+"/validation_"+title+".pdf"
        #yoda.plotting.plot(hists_list, outfile=out_name, plotkeys=plotkeys)
        yoda.plot(hists_list, outfile=out_name, **plotkeys)

def compare_yoda_files(ref_file, tuned_dict_files, options=None):
    tuned_dict = {}
    ref_data = yoda.read(ref_file)

    for key,value in tuned_dict_files.iteritems():
        tuned_dict[key] = yoda.read(value)

    compare_yoda(ref_data, tuned_dict, options)

if __name__ == "__main__":
    if len(sys.argv) < 3: 
        print sys.argv[0]," ref_data tuned_data [label]"

    if len(sys.argv) > 3:
        label = sys.argv[3]
    else:
        label = "tuned"

    input_dict = {
        "ref_file": sys.argv[1],
        "tuned_dict_files": {
            label:  sys.argv[2]
        }
    }

    compare_yoda_files(**input_dict)
