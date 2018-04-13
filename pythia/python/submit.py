#!/usr/bin/env python
# python 2.7.13

from tune_parameters import TuneMngr
import json
import subprocess
import sys
import os

from optparse import OptionParser

class Jobs:
    def __init__(self):
        self.exe = '/global/homes/x/xju/mctuning/software/MCTuning/pythia/python/generate_pythia_events.sh'
        self.no_submit = True;
        pass

    def readInputJason(self, json_file):
        data = json.load(open(json_file))
        self.tune = TuneMngr()
        self.tune.readInputJason(data['pythia_parameters'])
        self.nRuns = data['nRuns']
        self.nEventsPerRun = data['nEventsPerRun']

    def submit_all(self):
        self.tune.generate(self.nRuns)
        for iRun in range(self.nRuns):
            if not self.prepare(iRun):
                break
            self.submit(iRun)

    def workdir(self, irun):
        folder = 'submit/'+str(irun)
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

        self.submit_folder = folder
        return True

    def submit(self, irun):
        # may different from machine to machine
        cmd = ['sbatch', '-p', 'shared-chos',
               '-t', '10:00:00',
               '-D', self.workdir(irun),
               self.exe
              ]
        if self.no_submit:
            print cmd
        else:
            subprocess.call(cmd)

if __name__ == "__main__":
    usage = "%prog [options] json"
    version="%prog 1.0"
    parser = OptionParser(usage=usage, description="submit jobs to generate MC events", version=version)
    parser.add_option("-s", "--submit", default=False, action="store_true", help="submit the job")

    (options,args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        exit(1)

    jobs = Jobs()
    jobs.readInputJason(args[0])

    if options.submit:
        jobs.no_submit = False
    else:
        print "This is a dry try, jobs are NOT submitted"

    jobs.submit_all()
