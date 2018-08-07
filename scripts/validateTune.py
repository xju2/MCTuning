#!/usr/bin/env python
# -*- coding: utf-8 -*-

import yoda
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../ipynb')))

from yoda_helper import compare_yoda_files

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print os.path.basename(sys.argv[0])," ref_data tuned_data [label]"
        exit(1)

    if len(sys.argv) > 3:
        label = sys.argv[3]
    else:
        label = "tuned"

    input_dict = {
        "ref_file": sys.argv[1],
        "tuned_dict_files": {
            label:  sys.argv[2]
        },
        "options": {
            "output_dir": "validation",
            "logY": True
        }
    }
    compare_yoda_files(**input_dict)

    input_dict["options"]["logY"] = False
    compare_yoda_files(**input_dict)
