#!/usr/bin/env python

# change format of HepData: ATLAS_2014_I1268975 to match those generated from Rivet

from __future__ import print_function
import yoda
import sys
import re
import math

def ATLAS_2014_I1268975(in_file, out_file):
    data = yoda.read(in_file)
    new_data = []
    pre_fix = '/REF/ATLAS_2014_I1268975'
    for i in range(2):
        # two jet alg. atkt4 and atkt6
        for j in range(6):
            # six y* bins
            index = "{}/d{:0=2}-x01-y0{}".format(pre_fix, i+1, j+1)
            ref_index_number = i*6 + j + 1
            ref_index = "{}/d{:0=2}-x01-y01".format(pre_fix, ref_index_number)
            try:
                scatter2D = data[ref_index]
            except KeyError:
                print(ref_index,"not found")
                continue

            scatter2D.setAnnotation('Path', index)
            new_data.append(scatter2D)

    yoda.write(new_data, out_file)

def ATLAS_2017_I1519428(in_file, out_file):
    """
    Downloaded table:
        1. change indexes [0-8] to [1-9]
        2. provide another output that contains data only i.e. y01
        3. divide luminosity, lumi = 37 fb^{-1} = 37 x 10^{12} mb^{-1}.
    """
    data = yoda.read(in_file)
    lumi = 3.7e4 # in unit of 1/pb
    lumi_err = 0.032*lumi
    new_rivet = []
    new_ref = []
    for key, value in data.iteritems():

        # change Key
        index = int(re.search('d0(.+?)-x', key).group(1))
        new_key = key.replace('d0{}'.format(index), 'd0{}'.format(index+1))
        value.setAnnotation('Path', new_key)

        if index < 2:
            # scale to cross section of di-jet distributions

            # scale each bin only for 
            for point in value.points:
                bin_width = point.xErrs.minus + point.xErrs.plus
                if abs(point.yErrs.minus) < 1E-5:
                    # Use poisson error for observed events
                    y_err = math.sqrt(point.y)

                    # if y-axis is zero, err = 0
                    # else: error propagation
                    if abs(point.y) < 1E-5:
                        xs_err = 0.
                    else:
                        xs_err = point.y/lumi * math.sqrt(1/point.y + lumi_err**2/lumi**2)

                    xs_err /= bin_width
                    point.yErrs = (xs_err, xs_err)
                else:
                    point.yErrs = (point.yErrs.minus/lumi, point.yErrs.plus/lumi)

                point.y = point.y / lumi / bin_width
        else:
            pass
            # scale bin_width for chi2 distributions
            #for p in value.points:
            #    bin_width = p.xErrs.minus + p.xErrs.plus
            #    p.y = p.y / bin_width

        new_rivet.append(value)

        if "y01" in key:
            new_ref.append(value)

    yoda.write(new_rivet, out_file)
    yoda.write(new_ref, "ATLAS_2017_I1519428_ref.yoda")


def ATLAS_2017_I1635274(in_file, out_file):
    """
    Divided out the bin-width for jet pT distributions
    add Poisson error on data
    """
    data = yoda.read(in_file)
    new_rivet = []

    scaled_by_bin_width = ['d01', 'd03', 'd05', 'd07', 'd09']
    to_be_scaled_by_bin_width = ['d02', 'd04', 'd06', 'd08', 'd10', 'd11']

    for key, value in data.iteritems():
        short_key = key[25:28]
        # print(key, short_key)

        for p in value.points:
            bin_width = p.xErrs.minus + p.xErrs.plus
            if abs(p.yErrs.minus) < 1E-5:
                if short_key in scaled_by_bin_width:
                    ## such distributions have already scaled by bin-width
                    y_err = math.sqrt(p.y*bin_width)/bin_width
                else:
                    y_err = math.sqrt(p.y)
                p.yErrs = (y_err, y_err)
            else:
                p.yErrs = (p.yErrs.minus, p.yErrs.plus)

        if short_key in to_be_scaled_by_bin_width:
            # scale by bin-width
            for p in value.points:
                bin_width = p.xErrs.minus + p.xErrs.plus
                p.yErrs = (p.yErrs.minus/bin_width, p.yErrs.plus/bin_width)
                p.y = p.y / bin_width

        new_rivet.append(value)
    yoda.write(new_rivet, out_file)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(sys.argv[0],"data.yoda newdata.yoda")
        exit(1)

    #ATLAS_2017_I1519428(*sys.argv[1:])
    ATLAS_2017_I1635274(*sys.argv[1:])
