#!/usr/bin/env python
# python 2.7.13

from tune_parameters import TuneMngr
import json
import subprocess
import sys

class Jobs:
    def __init__(self):
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

    def prepare(self, irun):
        folder = 'submit/'+str(irun)
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
        print "submitting", irun
        pass

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0],"json"
        sys.exist(1)

    jobs = Jobs()
    jobs.readInputJason(sys.argv[1])
    jobs.submit_all()
