#!/usr/bin/env python
# python 2.7.13

from tune_parameters import TuneMngr
from utils import find_precision

import yoda

import json
import subprocess
import sys
import os
import glob

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
        self.subdir = '.'
        self.cmd_txt = ""
        pass

    def readInputJason(self, json_file):
        data = json.load(open(json_file))
        self.tune = TuneMngr(data['parameters'])

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
        self.queue_name = data.get('queue', 'shared')
        self.time = data.get('time', '1:00:00')

        default_cfg = "/global/homes/x/xju/code/MCTuning/pythia/data/atlas_detector_cfg.yoda"
        detector_cfg = data.get("detector_cfg", default_cfg)
        print "reading detector configuration: ", detector_cfg
        self.detector_hist2D = yoda.read(detector_cfg)

        self.tune.update_nickname(self.detector_hist2D)

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
        print "Queue to submit: {}".format(self.queue_name)
        print "Time reserved: {}".format(self.time)
        print "--------------------"
        if self.no_submit:
            print "This is a dry try, jobs are NOT submitted"
            print "To submit:\tsubmit.py -s", self.js
            out_cmd_name = os.path.join(self.subdir, "cmd.txt")
            print "commands are written to", out_cmd_name
            with open(out_cmd_name, 'w') as f:
                f.write(self.cmd_txt)


    def workdir(self, irun):
        folder = os.path.join(self.subdir, 'submit/{:0=6}'.format(irun))
        return os.path.abspath(folder)

    def prepare(self, irun):
        """prepare input files for each job"""

        try:
            detector_out = self.tune.update_detector(irun, self.detector_hist2D)
            prof_out = self.tune.get_tune(irun)
            pythia_out = self.tune.get_config(irun)
        except IndexError:
            return False

        folder = self.workdir(irun)
        new_irun = irun
        tune_output = os.path.join(folder, "used_params")
        while os.path.exists(folder):
            try:
                with open(tune_output) as f:
                    c = ""
                    for line in f:
                        c += line

                    if prof_out == c:
                        if len(glob.glob(folder+"/*.yoda")) > 0:
                            print new_irun," is already in", folder
                            return False
                        else:
                            break
                    else:
                        print "Professor input is changed"
            except IOError:
                break

            new_irun += 1
            folder = self.workdir(new_irun)
            tune_output = os.path.join(folder, "used_params")

        exit(1)
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

        # for value in self.detector_hist2D.values():
        #     for ib, b in enumerate(value.bins):
        #         print ib, b.volume
        detector_output = folder+"/detector_config.yoda"
        yoda.write(detector_out, detector_output)
        del detector_out

        self.submit_folder = folder
        return True

    def submit(self, irun):
        # may different from machine to machine
        nJobsPerRun = int(self.nEventsPerRun / self.nEventsPerJob)
        scale = find_precision(nJobsPerRun)[0]
        for ijob in range(nJobsPerRun):
            seed = ijob + self.seed*10**scale
            cmd = ['sbatch']
            host = os.getenv("NERSC_HOST")
            if host == "cori":
                cmd += [#'-N', "1",
                        '-C', 'haswell',
                        '-q', self.queue_name,
                        '-L', 'project'
                       ]
                if "shared" == self.queue_name:
                    cmd += ['-n', '1']
                else:
                    cmd += ['-N', '1']
            elif host == "pdsf":
                cmd += ['-p', 'shared-chos']
            else:
                pass
            cmd += ['-t', self.time,
                   '-D', self.submit_folder,
                   self.exe,
                   str(seed),
                   str(self.process),
                   str(self.anaID)
                  ]
            if self.no_submit:
                self.cmd_txt += " ".join(cmd)
                self.cmd_txt +=  "\n"
            else:
                subprocess.call(cmd)

        return nJobsPerRun

def list_analysis():
    return ["ATLAS_2014_I1268975", "ATLAS_2017_I1519428", "ATLAS_2017_I1635274"]

if __name__ == "__main__":
    usage = "%prog [options] json"
    version="%prog 1.0"
    parser = OptionParser(usage=usage, description="submit jobs to generate MC events", version=version)
    parser.add_option("-s", "--submit", default=False, action="store_true", help="submit the job")
    parser.add_option("-d", "--subdir", default=".")

    (options,args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        print "supported analysis:\t", list_analysis()
        exit(1)

    jobs = Jobs()
    jobs.subdir = options.subdir
    if not jobs.readInputJason(args[0]):
        exit(1)

    if options.submit:
        jobs.no_submit = False

    jobs.submit_all()
