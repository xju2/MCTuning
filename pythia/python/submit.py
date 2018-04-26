#!/usr/bin/env python
# python 2.7.13

from tune_parameters import TuneMngr
from tune_parameters import find_precision

import json
import subprocess
import sys
import os

from optparse import OptionParser

def str_to_int(events):
    evt_type = type(events)
    if evt_type is int:
        return events
    elif evt_type is not unicode and\
            evt_type is not str:
        return -1
    else:
        events = events.lower()

    if "m" in events:
        res = events.replace('m', "000000")
    elif 'k' in events:
        res = events.replace('k', '000')
    else:
        res = events

    try:
        res = int(res)
    except ValueError:
        print events,"is not a good input"
        res = -1

    return res

class Jobs:
    def __init__(self):
        self.exe = '/global/homes/x/xju/mctuning/software/MCTuning/pythia/python/generate_pythia_events.sh'
        self.no_submit = True;
        pass

    def readInputJason(self, json_file):
        data = json.load(open(json_file))
        self.tune = TuneMngr()
        self.tune.readInputJason(data['pythia_parameters'])

        self.nRuns = str_to_int(data['nRuns'])
        # self.scale = find_precision(self.nRuns)[0]

        self.nEventsPerRun = str_to_int(data['nEventsPerRun'])
        self.seed = data['seed']

        self.nEventsPerJob = str_to_int(data['nEventsPerJob'])

        self.anaID = data['rivet_analysis']
        self.process = data['pythia_process']
        self.js = json_file

    def submit_all(self):
        if self.nEventsPerJob < 0 or\
           self.nEventsPerRun < 0:
            return

        self.tune.generate(self.nRuns)

        total_jobs = 0
        for iRun in range(self.nRuns):
            if not self.prepare(iRun):
                break
            total_jobs += self.submit(iRun)

        print "Total jobs:", total_jobs
        if self.no_submit:
            print "To submit:\nsubmit.py -s", self.js

    def workdir(self, irun):
        folder = 'submit/{:0=6}'.format(irun)
        return os.path.abspath(folder)

    def prepare(self, irun):
        folder = self.workdir(irun)
        subprocess.call(['mkdir', '-p', folder])

        try:
            self.tune.fetch()
        except IndexError:
            return False

        tune_output = folder+"/used_params"
        with open(tune_output, 'w') as f:
            f.write(self.tune.get_tune())

        pythia_config_output = folder+"/tune_parameters.cmnd"
        with open(pythia_config_output, 'w') as f:
            f.write(self.tune.get_config())
            f.write("\n")
            # add other global configurations
            out =  "Random:setSeed           = on     ! user-set seed\n"
            # out += "Random:seed             = {} \n".format(str(int(irun+self.seed*10**self.scale))) #
            out += "Main:numberOfEvents     = {}\n".format(str(self.nEventsPerJob))
            out += "Main:timesAllowErrors   = {}\n".format(str(max(int(self.nEventsPerJob * 0.002),  1)))
            # out += "Next:numberCount        = {}\n".format(str(max(int(self.nEventsPerJob * 0.01), 100)))
            out += "Next:numberCount        = 10000  ! \n"
            f.write(out)

        self.submit_folder = folder
        return True

    def submit(self, irun):
        # may different from machine to machine
        nJobsPerRun = int(self.nEventsPerRun / self.nEventsPerJob)
        scale = find_precision(nJobsPerRun)[0]
        for ijob in range(nJobsPerRun):
            seed = ijob + self.seed*10**scale
            cmd = ['sbatch', '-p', 'shared-chos',
                   '-t', '24:00:00',
                   '-D', self.workdir(irun),
                   self.exe,
                   str(seed),
                   str(self.process),
                   str(self.anaID)
                  ]
            if self.no_submit:
                print cmd
            else:
                subprocess.call(cmd)

        return nJobsPerRun

def list_analysis():
    return ["ATLAS_2014_I1268975", "ATLAS_2017_I1519428"]

if __name__ == "__main__":
    usage = "%prog [options] json"
    version="%prog 1.0"
    parser = OptionParser(usage=usage, description="submit jobs to generate MC events", version=version)
    parser.add_option("-s", "--submit", default=False, action="store_true", help="submit the job")

    (options,args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        print "supported analysis:\t", list_analysis()
        exit(1)

    jobs = Jobs()
    jobs.readInputJason(args[0])

    if options.submit:
        jobs.no_submit = False
    else:
        print "This is a dry try, jobs are NOT submitted"

    jobs.submit_all()
