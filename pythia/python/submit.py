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

import time

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

def nersc_hours(queue_name, hours, nnodes, njobs):
    """
    only valide for Haswell
    """
    charge_factor = 0
    if 'regular' in queue_name or 'debug' in queue_name:
        charge_factor = 80
    elif 'shared' in queue_name:
        charge_factor = 2.5
    else:
        print("I don't know which queue you are in")

    return charge_factor*hours*nnodes*njobs

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
        print "parameters input:", data['parameters']
        print "total runs:", self.nRuns
        print "minimum for Professor:", self.tune.minimum_runs_for_Prof()
        if self.nRuns < self.tune.minimum_runs_for_Prof():
            print "!!! Increase the number of parameters' values !!!"

        self.nEventsPerRun = str_to_int(data['nEventsPerRun'])
        self.seed = data['seed']

        self.nEventsPerJob = str_to_int(data['nEventsPerJob'])
        self.nJobsPerRun = int(self.nEventsPerRun / self.nEventsPerJob)

        self.anaID = data['rivet_analysis']
        self.process = data['pythia_process']
        self.pythia_opt = data.get('pythia_options', None)
        self.js = json_file
        self.queue_name = data.get('queue', 'shared')
        self.time = data.get('time', '1:00:00')
        self.repo = data.get('repo', 'm1029')

        default_cfg = "/global/homes/x/xju/code/MCTuning/pythia/data/atlas_detector_cfg.yoda"
        detector_cfg = data.get("detector_cfg", default_cfg)
        print "reading detector configuration: ", detector_cfg
        self.detector_hist2D = yoda.read(detector_cfg)

        #self.tune.update_nickname(self.detector_hist2D)

        return True

    def submit_all(self):
        if self.nEventsPerJob < 0 or\
           self.nEventsPerRun < 0:
            return

        total_jobs = 0
        finished_jobs = 0
        for iRun in range(self.nRuns):
            if not self.prepare(iRun):
                finished_jobs += self.nJobsPerRun
                continue
            self.submit(iRun)
            total_jobs += self.nJobsPerRun

        print "--------------------"
        print "Working dir:", os.path.abspath(self.subdir)
        print "Total runs:", self.nRuns
        print "Total events in each run: {:,}".format(self.nEventsPerRun)
        print "Events per Job: {:,}".format(self.nEventsPerJob)
        print "Total Finished Jobs: {:,}".format(finished_jobs)
        print "Total jobs to submit: {:,}".format(total_jobs)
        print "Queue to submit: {}".format(self.queue_name)
        print "Time reserved: {}".format(self.time)
        print "Repo to burn: {}".format(self.repo)
        if os.getenv("NERSC_HOST") == 'cori':
            time_str = time.strptime(self.time, '%H:%M:%S')
            hours = time_str.tm_hour + time_str.tm_min/60.
            print "Total NERSC Hours: {:,f}".format(
                nersc_hours(self.queue_name, hours, 1, total_jobs)
            )
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
                        #n_yodas = len(glob.glob(folder+"/out_*.yoda"))
                        n_yodas = len([x for x in glob.glob(folder+"/*.yoda") if "detector" not in x])
                        if n_yodas > 0:
                            # print new_irun," is already in", folder, n_yodas
                            return False
                        else:
                            break
                    #else:
                    #    print "directory is there, but input is changed"
            except IOError:
                break

            new_irun += 1
            folder = self.workdir(new_irun)
            tune_output = os.path.join(folder, "used_params")

        subprocess.call(['mkdir', '-p', folder])
        with open(tune_output, 'w') as f:
            f.write(prof_out)

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
        pythia_config_output = folder+"/tune_parameters.cmnd"
        with open(pythia_config_output, 'w') as f:
            f.write(out)

        with open(os.path.join(folder, 'runPythia.cmd'), 'w') as f:
            with open(self.process) as pp:
                f.write(pp.read())
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
        scale = find_precision(self.nJobsPerRun)[0]
        for ijob in range(self.nJobsPerRun):
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

                cmd += ['-A', self.repo]
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


if __name__ == "__main__":
    usage = "%prog [options] json"
    version="%prog 1.0"
    parser = OptionParser(usage=usage, description="submit jobs to generate MC events", version=version)
    parser.add_option("-s", "--submit", default=False, action="store_true", help="submit the job")
    parser.add_option("--subdir", default=".")

    (options,args) = parser.parse_args()

    if len(args) < 1:
        parser.print_help()
        exit(1)

    jobs = Jobs()
    jobs.subdir = options.subdir
    if not jobs.readInputJason(os.path.join(jobs.subdir, args[0])):
        exit(1)

    if options.submit:
        jobs.no_submit = False

    jobs.submit_all()
