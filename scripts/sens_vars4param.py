#! /usr/bin/env python

"""\
%prog datadir

Gradient calculation to display sensitivity to parameter shifts

%prog [<ipolfile>=ipol.dat]

"""


import matplotlib, os
matplotlib.use(os.environ.get("MPL_BACKEND", "Agg"))

import sys
import professor2 as prof
import numpy as np

class IpolReader():
    def __init__(self, file_name):
        self.file_name = file_name

        self.ihistos, self.meta = prof.read_ipoldata(self.file_name)
        self.parse_meta()

    def parse_meta(self):
        meta = self.meta
        minPVs = map(float, meta['MinParamVals'].split())
        maxPVs = map(float, meta['MaxParamVals'].split())
        centerPVs = [x + 0.5*(y - x) for (x,y) in zip(minPVs, maxPVs)]
        paraNames = meta['ParamNames'].split()

        self.parameters = {}
        for ip, para in enumerate(paraNames):
            self.parameters[para] = {
                "min": minPVs[ip],
                "mid": centerPVs[ip],
                "max": maxPVs[ip]
            }


    def filter_obs(self, weight_file):
        matchers = prof.read_pointmatchers(weight_file)
        for hn in self.ihistos.keys():
            if not any(m.match_path(hn) for m in matchers.keys()):
                del self.ihistos[hn]

        if len(self.ihistos) < 1:
            print "not enought obseravbles"
            sys.exit(1)

    def sensitivity(self):
        # keys: observables
        # values: x, y
        # x: x-axis of the observable
        # y: a dictionary:
        #        key: parameter name
        #        value: sensitivity
        self.sens_values = {}
        for k in sorted(IHISTOS.keys()):
            centgrad={}
            for p in params:
                centgrad[p] = []

            x = array([b.xmid for b in IHISTOS[k].bins])
            for i in xrange(len(x)):
                bin_grad = IHISTOS[k].bins[i].grad(C)
                bin_val = IHISTOS[k].bins[i].val(C)
                for num, P in enumerate(params):
                    if abs(bin_val) > 0:
                        centgrad[P].append(bin_grad[num]/bin_val)
                    else:
                        centgrad[P].append(0)

            self.sens_values[k] = (x, centgrad)

    def sens_of_obs(self, para):
        pass

    def sens_of_para(self, obs):




import optparse
op = optparse.OptionParser(usage=__doc__)
op.add_option("--wfile", dest="WFILE", default=None, help="Path to a weight file, used to restrict plotting to a subset of histograms (default: %default)")
op.add_option("-o", "--output", dest="OUTPUT", default="sensitivities", help="Output directory (default: %default)")
op.add_option("--debug", dest="DEBUG", action="store_true", default=False, help="Turn on some debug messages")
op.add_option("--quiet", dest="QUIET", action="store_true", default=False, help="Turn off messages")
op.add_option("--grad", dest="GRAD", action="store_true", default=False, help="One dimensional sensitivity based on gradient")
op.add_option("--cmap", dest="CMAP", action="store_true", default=False, help="Colour map plots")
op.add_option("-r", dest="RESULT", default=None, help="File with parameter point with respect to which the sensitivities are calculated")
# TODO: Add weight file parsing to decide which histos (and bin subsets) to interpolate
opts, args = op.parse_args()



colours = ["b", "r", "g", "k"]
lstyles = ["-", "--", ":"]
markers = ["o", "x"]
STYLES= []
for l in lstyles:
    for c in colours:
        for m in markers:
            STYLES.append(l+c+m)

def plotColourMap(x, y, Z, hname, pname):
    import matplotlib.pyplot as plt
    fig, ax = plt.subplots()

    # Colour limits
    from matplotlib import cm
    lim = max(abs(Z.max()), abs(Z.min()))
    cax = ax.imshow(Z, interpolation='nearest', cmap=cm.seismic, vmin=-lim, vmax=lim)
    cbar = fig.colorbar(cax, ticks=[-lim, 0, lim])

    # Ticks and labels
    xticki = [int(i) for i in ax.xaxis.get_ticklocs()[1:]]
    xtickl = [""]
    xtickl.extend(["%.3f"%x[i] for i in xticki if i <len(Z[0])])
    ax.set_xticklabels(xtickl, rotation=270)
    yticki = [int(i) for i in ax.yaxis.get_ticklocs()[1:]]
    ytickl = [""]
    ytickl.extend(["%.3f"%y[i] for i in yticki if i <len(Z[:,0])])
    ax.set_yticklabels(ytickl)
    ax.set_xlabel(hname)
    ax.set_ylabel(pname)

    # Nice and tight
    plt.tight_layout()

    # Output
    import os
    if not os.path.exists(opts.OUTPUT):
        os.makedirs(opts.OUTPUT)
    fig.savefig(os.path.join(opts.OUTPUT, "sens_%s_p%s.pdf"%(hname.replace("/","_"), pname)))
    plt.close(fig)
    del fig

def plotGradient(X, Y, hname):
    import matplotlib.pyplot as plt
    fig, ax = plt.subplots()

    # Ticks and labels
    ax.set_xlabel(hname)
    ax.set_ylabel("Sensitivity")


    pnames = sorted(Y.keys())
    for num, p in enumerate(pnames):
        ax.plot(X, Y[p], STYLES[num], label=p)
    ax.legend(loc='best', prop={'size':6})

    # Nice and tight
    plt.tight_layout()

    # Output
    import os
    if not os.path.exists(opts.OUTPUT):
        os.makedirs(opts.OUTPUT)
    fig.savefig(os.path.join(opts.OUTPUT, "sens_%s.pdf"%hname.replace("/","_")))
    plt.close(fig)
    del fig

## Read persisted interpolations to re-create the ipol Histos
import professor2 as prof
if not opts.QUIET:
    print prof.logo

import os, sys
if len(args) <1:
    print "No interpolation file given, exiting"
    sys.exit(1)

IFILE = args[0]
if not os.path.exists(IFILE):
    print "Error: specified interpolation file %s not found, exiting." % IFILE
    sys.exit(1)

## Read in meta data and ipol histos
IHISTOS, META = prof.read_ipoldata(IFILE)

## Weight file parsing
if opts.WFILE:
    matchers = prof.read_pointmatchers(opts.WFILE)
    for hn in IHISTOS.keys():
        if not any(m.match_path(hn) for m in matchers.keys()):
            del IHISTOS[hn]
    if len(IHISTOS.keys())==0:
        print "Nothing left after weight file parsing, exiting"
        sys.exit(0)

## Center of the Parameter space
minPV = map(float, META["MinParamVals"].split())
maxPV = map(float, META["MaxParamVals"].split())
C = [minPV[i] + 0.5*(maxPV[i]-minPV[i]) for i in xrange(len(minPV))]

## Parameter names --- for labels and output file names
params = META["ParamNames"].split()

from numpy import array, linspace, empty, meshgrid

if opts.RESULT is None:
    C = [minPV[m] + 0.5*(maxPV[m]-minPV[m]) for m in xrange(len(minPV))]
else:
    # TODO add check that file exists
    C = prof.read_paramsfile(opts.RESULT)


if opts.GRAD:
    # Iterate over observables
    for k in sorted(IHISTOS.keys()):
        centgrad={}
        for p in params:
            centgrad[p] = []
        x = array([b.xmid for b in IHISTOS[k].bins])
        for i in xrange(len(x)):
            bin_grad = IHISTOS[k].bins[i].grad(C)
            bin_val = IHISTOS[k].bins[i].val(C)
            for num, P in enumerate(params):
                if abs(bin_val) > 0:
                    centgrad[P].append(bin_grad[num]/bin_val)
                else:
                    centgrad[P].append(0)
        plotGradient(x, centgrad, k)

elif opts.CMAP:
    # Iterate over observables
    for k in sorted(IHISTOS.keys()):
        # Iterate over parameters
        x = array([b.xmid for b in IHISTOS[k].bins])
        for P in xrange(len(C)):
            # Symmetric x (bins) and y (parmeter sub space) 
            y = linspace(minPV[P], maxPV[P], len(x))
            X, Y = meshgrid(x, y)
            Z = empty(X.shape)
            # Iterate over bins
            for i in xrange(len(X)):
                # C is where the derivative is evaluated.
                # We scan dimension i but set all parameters !=i to the center value
                import copy
                CC=copy.deepcopy(C)
                # # C = [minPV[m] + 0.5*(maxPV[m]-minPV[m]) for m in xrange(len(minPV))]

                for j in xrange(len(Y)):
                    c = minPV[P] + j/float(len(x))*(maxPV[P]- minPV[P])
                    CC[P] = c
                    sensitivity = IHISTOS[k].bins[i].der(CC) # The derivative
                    Z[j,i] = sensitivity

            # Colourmap Plotting
            plotColourMap(x, y, Z, k, params[P])

else:
    print "Need to specify a plot option: --cmap or --grad"
