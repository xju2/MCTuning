#!/usr/bin/env python
from __future__ import print_function
import yoda
import sys
import os

def get_sum(in_yoda):
    if type(in_yoda) == yoda.core.Profile1D:
        in_yoda = in_yoda.mkScatter()

    if type(in_yoda) == yoda.core.Scatter2D:
        return sum([p.y*(p.xMax-p.xMin) for p in in_yoda.points])
    else: ### yoda.core.Hist1D
        return float(in_yoda.sumW())


def plot(inputs, options=None):
    mpl = yoda.mplinit()
    if len(inputs) < 2:
        print("require at least two inputs")
        return None


    plotkeys = {
        "xlabel": "",
        "ylabel": "Events / Bin",
        "logY": True,
        "title": "",
        "ratioylabel": "MC / Ref",
        "errorbars": False,
        "ratioymin": 0.8,
        "ratioymax": 1.2,
        'ratiogrid': False,
        'LegendFontSize': 'large'
    }

    if options is None:
        options = dict()
    else:
        for key in options.keys():
            if key in plotkeys:
                plotkeys[key] = options[key]

    output_dir = options['output_dir'] if 'output_dir' in options else 'validation'
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # find variables in the second file
    data_list = [yoda.read(f[1]) for f in inputs]
    tune_data = data_list[1]

    for key in sorted(tune_data.keys()):
        hists_list = []

        ## loop over all input files
        print("process", key)
        for i,in_file in enumerate(inputs):
            data = data_list[i]
            if i==0:
                try:
                    hist = data['/REF'+key]
                except KeyError:
                    hist = data[key]

                hist.setAnnotation('ratioref', True)
            else:
                hist = data[key]

            hist.setAnnotation('Title', in_file[0])
            if type(hist) == yoda.core.Profile1D:
                hist = hist.mkScatter()
            hist.setAnnotation('nEvents', get_sum(hist))

            hists_list.append(hist)

        title = key.split('/')[-1]
        plotkeys['xlabel'] = options[title][0] if title in options else title
        plotkeys['logY'] = options[title][1] if title in options else plotkeys['logY']
        if title in options and len(options[title]) > 2:
            plotkeys['ylabel'] = options[title][2]

        if plotkeys.get('logY', False):
            out_name = output_dir+"/validation_"+title+"_LogY.pdf"
        else:
            out_name = output_dir+"/validation_"+title+"_LinearY.pdf"

        yoda.plot(hists_list, outfile=out_name, **plotkeys)
        del hists_list
        mpl.pyplot.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        #print(os.path.basename(sys.argv[0]), " ref_data tuned_data")
        print(os.path.basename(sys.argv[0]), " config.json")
        exit(1)

    import json
    input_dict = json.load(open(sys.argv[1]))
    plot(**input_dict)
