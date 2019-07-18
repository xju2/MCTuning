#!/usr/bin/env python
# -*- coding: utf-8 -*-

import yoda
import os
from collections import OrderedDict

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
        "title": "",
        "ratioylabel": "Tuned / Ref",
        "errorbars": False,
        "ratioymin": 0.1,
        "ratioymax": 2,
        'ratiogrid': False
    }

    if options is None:
        options = dict()
    else:
        for key in options.keys():
            if key in plotkeys:
                plotkeys[key] = options[key]

    tuned = tuned_dict.values()[0]

    output_dir = options.get('output_dir', "output")
    output_format = options.get('format', 'eps')

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

        hists_list.append(ref_hist)
        title = ref_hist.path.split('/')[-1]

        plotkeys['xlabel'] = options[title][0] if title in options else title
        plotkeys['logY'] = options[title][1] if title in options else plotkeys['logY']

        for label, yoda_handle in tuned_dict.iteritems():
            try:
                tuned_hist = yoda_handle[key]
                tuned_hist.setAnnotation("Title", label)
                hists_list.append(tuned_hist)
            except KeyError:
                print title," missed tuned data"

        if plotkeys.get('logY', False):
            out_name = output_dir+"/validation_"+title+"_LogY."+output_format
        else:
            out_name = output_dir+"/validation_"+title+"_LinearY."+output_format
        yoda.plot(hists_list, outfile=out_name, **plotkeys)
        # break



def compare_yoda_files(ref_file, tuned_dict_files, options=None):
    tuned_dict = OrderedDict()
    ref_data = yoda.read(ref_file)

    for key,value in tuned_dict_files.iteritems():
        tuned_dict[key] = yoda.read(value)

    compare_yoda(ref_data, tuned_dict, options)
