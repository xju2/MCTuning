#!/usr/bin/env python

# change format of HepData: ATLAS_2014_I1268975 to match those generated from Rivet

import yoda
import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0],"data.yoda newdata.yoda"
        exit(1)

    data = yoda.read(sys.argv[1])
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

    yoda.write(new_data, sys.argv[2])
