#!/usr/bin/env python
# python 2.7.13

import json
import sys

import math
import pandas as pd

import yoda

from parameter import Parameter

import pyDOE


def least_runs(n):
    # n is the number of parameters
    # return number of coefficient to be fitted
    return 1 + n + n*(n+1)/2

class TuneMngr:
    """
    Manage tuned prameters and sampling of these parameters
    """
    def __init__(self, json_file):
        # avoid any duplicated variables from the input
        # two variables are the same, if they share the
        # same name
        self.para_list = set([])

        data = json.load(open(json_file))
        for value in data["variables"]:
            self.para_list.add(Parameter(**value))

        self.DOE = data.get("DOE", "one-to-one")
        if self.DOE.lower() != "factorial" and \
           self.DOE.lower() != "one-to-one":
            print "I don't know how to do: ", self.DOE
            print "but it's OK, I will just use Factorial"


        self.summary()

    def minimum_runs_for_Prof(self):
        return least_runs(len(self.para_list))

    def runs_from_DOE(self):
        return 1

    def summary(self):
        print "\nBegin of parameter summary"
        print "  Design of Exp.:", self.DOE
        print "  Total parameters: {}".format(len(self.para_list))
        for para in sorted(self.para_list, key=lambda para:para.id_):
            print "\t",para

        print "End of parameter summary"

    def append_factors(self, para):
        if len(new_list) > 0:
            para.run_values = para.values*reduce(lambda x, y: x*y, [len(z.values) for z in new_list])
        else:
            para.run_values = para.values

    def append_one2one(self, para):
        """use the values provided by the parameter itself.
        The ones with shorter value-list are filled by their nominal values
        """
        para.run_values = para.values + [para.nominal]*(self.max_len - len(para.values))


    def generate(self):
        if self.DOE.lower() == "one-to-one":
            self.max_len = max([len(para.values) for para in self.para_list])
            map(self.append_one2one, self.para_list)

        elif self.DOE.lower() == "factorial":
            index_array = pyDOE.fullfact([len(para.values) for para in self.para_list])
            for ip, para in enumerate(self.para_list):
                para.run_values = [para.values[int(x)] for x in index_array[:, ip]]
        else:
            print "I do nothing."

        data = dict([(para.nickname, para.run_values) for para in self.para_list])
        self.df = pd.DataFrame(data=data)
        print "\n****Generated List of Parameters***"
        print self.df
        print "\n"
        return self.df.shape

    def get_config(self, irun):
        return "\n".join([para.config(self.df[para.nickname].iloc[irun])\
                          for para in self.para_list if para.type_ == "pythia"])

    def get_tune(self, irun):
        return "\n".join([para.prof_config(self.df[para.nickname].iloc[irun]) for para in self.para_list])

    def update_nickname(self, detector_hists):
        for para in self.para_list:
            if para.type_ == "pythia":
                continue
            hist2D = detector_hists.get(para.name, None)
            if hist2D is not None:
                bin_2d = hist2D.binIndexAt(para.other_opt['eta'], para.other_opt['pT'])
                para.nickname = para.nickname+"_bin"+str(bin_2d)

    def update_detector(self, irun, detector_hists):
        new_hists = {}
        for key,value in detector_hists.iteritems():
            new_hists[key] = value.clone()

        for para in self.para_list:
            if para.type_ == "pythia":
                continue

            hist2D = new_hists.get(para.name, None)
            if hist2D is not None:
                bin_2d = hist2D.binIndexAt(para.other_opt['eta'], para.other_opt['pT'])
                # print "INFO: ",hist2D.bin(bin_2d).volume, hist2D.bin(bin_2d).height
                hist2D.fillBin(bin_2d, -1*hist2D.bin(bin_2d).volume)
                hist2D.fillBin(bin_2d, self.df[para.nickname].iloc[irun])
                # print "After: ",hist2D.bin(bin_2d).volume, hist2D.bin(bin_2d).height
                # para.nickname = para.nickname+"_bin"+str(bin_2d)
                # print "bin index: ", hist2D.binIndexAt(para.other_opt['eta'], para.other_opt['pT'])
                # print "set to: ", self.df[para.nickname].iloc[irun]
            else:
                print para.name,"is not in detector configuration"

        return new_hists

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0]," json"
        exit(1)

    print least_runs(1)
    print least_runs(5)
    print least_runs(6)
    print least_runs(10)
    print least_runs(100)
    print least_runs(500)
    print least_runs(1000)

    #sys.exit(0)
    tune = TuneMngr(sys.argv[1])

    tune.generate()
    irun = 0
    while irun < 1:
        try:
            print "--------begin config------------"
            print tune.get_config(irun)
            print "--------end config------------\n"
            print "--------begin tune------------"
            print tune.get_tune(irun)
            print "--------end tune------------\n"
        except IndexError:
            break
        irun += 1
