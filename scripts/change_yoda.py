#!/usr/bin/env python

# change format of HepData: ATLAS_2014_I1268975 to match those generated from Rivet

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
                print ref_index,"not found"
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

        # scale each bin
        for point in value.points:
            if abs(point.yErrs.minus) < 1E-5:
                # if origin error is zero, use poisson error
                y_err = math.sqrt(point.y)
                if abs(point.y) < 1E-5:
                    xs_err = 0.
                else:
                    xs_err = point.y/lumi * math.sqrt(1/point.y + lumi_err**2/lumi**2)
                point.yErrs = (xs_err, xs_err)
            else:
                point.yErrs = (point.yErrs.minus/lumi, point.yErrs.plus/lumi)

            point.y = point.y / lumi

        new_rivet.append(value)

        if "y01" in key:
            new_ref.append(value)

    yoda.write(new_rivet, out_file)
    yoda.write(new_ref, "ATLAS_2017_I1519428_ref.yoda")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0],"data.yoda newdata.yoda"
        exit(1)

    ATLAS_2017_I1519428(*sys.argv[1:])
