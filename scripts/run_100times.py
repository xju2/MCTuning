#!/usr/bin/env python
from __future__ import print_function

import subprocess
import threading
import os
import time
import numpy as np
import ROOT

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='run MT')
    add_arg = parser.add_argument
    add_arg('file_name', help='file name')
    add_arg('outdir', help='output directory')
    add_arg('-w', '--workers', type=int, help='number of workers', default=1)
    add_arg('-n', '--n_times', type=int, help='number of smearing', default=100)
    add_arg('--no-mpi', action='store_true', help='no MPI')

    args = parser.parse_args()

    file_name = args.file_name
    out_dir = args.outdir
    n_workers = args.workers
    n_total_tasks = args.n_times
    no_mpi = args.no_mpi

    rank = 0
    size = 1
    has_mpi = False
    if not no_mpi:
        try:
            from mpi4py import MPI
        except ImportError:
            print("mpi4py is missing, MPI not use")
        else:
            comm = MPI.COMM_WORLD
            size = comm.Get_size()
            rank = comm.Get_rank()
            print("World {} ranks, now in rank {} ".format(size, rank))
            has_mpi = True

    no_mpi = no_mpi or not has_mpi
    if rank == 0:
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)
        print("Total  \x1b[33m{}\x1b[0m Jobs".format(n_total_tasks))
        all_jobs = np.arange(0, n_total_tasks)
        job_ids = [x.tolist() for x in np.array_split(all_jobs, size)]
    else:
        job_ids = None

    if no_mpi:
        job_ids = job_ids[0]
    else:
        comm.Barrier()
        job_ids = comm.scatter(job_ids, root=0)

    n_tot_tasks = len(job_ids)
    print("rank {} has {} workers".format(rank, n_workers))
    print("rank {} deals with {} events".format(rank, n_tot_tasks))


    tasks = []
    itask = 0
    n_jobs = 0
    while itask < n_tot_tasks:
        job_idx = job_ids[itask]
        out_name = '{}/{}'.format(out_dir, "Smear{}.root".format(job_idx))
        ## test of out name is already there
        root_file = ROOT.TFile(out_name)
        if root_file and not root_file.IsZombie():
            print("{} is there".format(out_name))
            itask += 1
            continue

        cmd =['/global/homes/x/xju/code/powheginlinegen/apps/smearing_on_peak', '-i', file_name, '-o', out_name, '-s', str(job_idx+7*rank)]
        if len(tasks) < n_workers:
            job_id = threading.Thread(target=subprocess.call, kwargs={"args":cmd})
            job_id.start()
            tasks.append(job_id)
            itask += 1
            time.sleep(1) ## so to have different seeds.
        else:
            while len(tasks) == n_workers:
                for task in tasks:
                    if not task.is_alive():
                        tasks.remove(task)

                time.sleep(120) ## two minutes
    ## finish remaining tasks
    while any([task.is_alive() for task in tasks]):
        time.sleep(120)
