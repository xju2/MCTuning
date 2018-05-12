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
        self.cmd_txt = ""
        pass

    def readInputJason(self, json_file):
        data = json.load(open(json_file))
        self.tune = TuneMngr(data['pythia_parameters'])

        # self.nRuns = str_to_int(data['nRuns'])
        self.nRuns = self.tune.generate()[0]
        print "total runs:", self.nRuns
        print "minimum for Professor:", self.tune.minimum_runs_for_Prof()
        if self.nRuns < self.tune.minimum_runs_for_Prof():
            print "!!! Increase the number of parameters' values !!!"

        self.nEventsPerRun = str_to_int(data['nEventsPerRun'])
        self.seed = data['seed']

        self.nEventsPerJob = str_to_int(data['nEventsPerJob'])

        self.anaID = data['rivet_analysis']
        self.process = data['pythia_process']
        self.pythia_opt = data.get('pythia_options', None)
        self.js = json_file

        return True

    def submit_all(self):
        if self.nEventsPerJob < 0 or\
           self.nEventsPerRun < 0:
            return

        total_jobs = 0
        for iRun in range(self.nRuns):
            if not self.prepare(iRun):
                break
            total_jobs += self.submit(iRun)

        print "--------------------"
        print "Total runs:", self.nRuns
        print "Total events in each run: {:,}".format(self.nEventsPerRun)
        print "Events per Job: {:,}".format(self.nEventsPerJob)
        print "Total jobs: {:,}".format(total_jobs)
        print "--------------------"
        if self.no_submit:
            print "This is a dry try, jobs are NOT submitted"
            print "To submit:\tsubmit.py -s", self.js
            out_cmd_name = "cmd.txt"
            print "commands are written to", out_cmd_name
            with open(out_cmd_name, 'w') as f:
                f.write(self.cmd_txt)


    def workdir(self, irun):
        folder = 'submit/{:0=6}'.format(irun)
        return os.path.abspath(folder)

    def get_folder(self, irun):
        while os.path.exists(self.workdir(irun)):

        return irun

    def prepare(self, irun):

        try:
            prof_out = self.tune.get_tune(irun)
            pythia_out = self.tune.get_config(irun)
        except IndexError:
            return False

        folder = self.workdir(irun)
        new_irun = irun
        tune_output = folder+"/used_params"
        while os.path.exists(folder):
            with open(tune_output) as f:
                if prof_out == "".join(f):
                    print irun," is already in", folder
                    return False

            new_irun += 1
            folder = self.workdir(new_irun)
            tune_output = folder+"/used_params"

        subprocess.call(['mkdir', '-p', folder])
        with open(tune_output, 'w') as f:
            f.write(prof_out)

        pythia_config_output = folder+"/tune_parameters.cmnd"
        with open(pythia_config_output, 'w') as f:
            # write global configurations
            out =  "Random:setSeed           = on     ! user-set seed\n"
            out += "Main:numberOfEvents     = {}\n".format(str(self.nEventsPerJob))
            out += "Main:timesAllowErrors   = {}\n".format(str(max(int(self.nEventsPerJob * 0.002),  1)))
            out += "Next:numberCount        = 10000  ! \n"
            out += '\n'

            # additional options in jason input
            if self.pythia_opt:
                out += '\n'.join(self.pythia_opt)
                out += '\n'

            out += pythia_out + '\n'
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
                self.cmd_txt += " ".join(cmd)
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
    if not jobs.readInputJason(args[0]):
        exit(1)

    if options.submit:
        jobs.no_submit = False

    jobs.submit_all()
