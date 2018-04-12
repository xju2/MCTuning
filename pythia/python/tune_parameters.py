#!/usr/bin/env python
# python 2.7.13

import json
import sys

import math

def find_precision(number):
    # https://stackoverflow.com/questions/3018758/determine-precision-and-scale-of-particular-number-in-python?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
    max_digits = 14
    int_part = int(number)
    magnitude = 1 if int_part == 0 else int(math.log10(int_part)) + 1
    if magnitude >= max_digits:
        return (magnitude, 0)

    frac_part = abs(number) - int_part
    multiplier = 10 ** (max_digits - magnitude)
    frac_digits = multiplier + int(multiplier * frac_part + 0.5)
    while frac_digits % 10 == 0:
        frac_digits /= 10

    scale = int(math.log10(frac_digits))
    return (magnitude+scale, scale)

class Sampling:
    def __init__(self, min_, max_):
        self.min_ = min_
        self.max_ = max_
        #self.scale = max(find_precision(self.max_)[1],
        #                 find_precision(self.min_)[1])
        self.current = 0
        self.point_list = []

    def generate(self, npoints):
        pass

    def mid(self):
        return 0.5*(self.min_ + self.max_)

    def get(self):
        if self.current >= len(self.point_list):
            raise IndexError("Maximum points: {}, but asking {}.".format(
                len(self.point_list), self.current)
            )

        value = self.point_list[self.current]
        self.current += 1
        return value

class LinearSampling(Sampling):

    def generate(self, npoints):
        scale = max(find_precision(self.max_)[1],
                    find_precision(self.min_)[1],
                    find_precision(npoints)[0]
                   )

        self.current = 0
        step = (self.max_ - self.min_)/(npoints - 1)
        self.point_list = [round(self.min_ + x*step, scale) for x in range(npoints-1)]
        self.point_list.append(self.max_)

def get_sampling(para, sampling_type):
    if sampling_type == "Linear":
        return LinearSampling(para.min_, para.max_)
    else:
        print sampling_type,"is not implemented!"
        print "Linear will be used"
        return LinearSampling(para.min_, para.max_)

class Parameter:
    """
    Must contain a unique name,
    id, nickname and description are optional.
    """
    def __init__(self, name,  min_, max_,
                 sampling="Linear",
                 description="None",
                 id_=-1,
                 nickname=None,
                 scale=-1
                ):
        self.id_ = id_
        self.name = name

        if nickname is None:
            self.nickname = self.name
        else:
            self.nickname = nickname

        self.min_= min_
        self.max_= max_
        self.value = self.min_

        self.sampling = get_sampling(self, sampling)
        self.description = description
        self.scale = scale

    def jsonDefault(self):
        return self.__dict__

    def to_str(self):
        return "{} {}({},{})".format(self.id_, self.name, self.min_, self.max_)

    def __repr__(self):
        return "Parameter {}({},{})".format(self.name, self.min_, self.max_)

    def __str__(self):
        return self.to_str()

    def __eq__(self, other):
        return self.name == other.name

    def __ne__(self, other):
        return not self.__eq__(other)

    def __hash__(self):
            return hash((self.name))

    def for_tune(self):
        return "{} {}".format(self.nickname, self.value)

    def for_config(self):
        return "{:<40} = {:<10} \t! {}".format(self.name, self.value, self.description)

class TuneMngr:
    """
    Manage tuned prameters and sampling of these parameters
    """
    def __init__(self):
        self.para_list = set([])

    def readInputJason(self, json_file):
        data = json.load(open(json_file))
        for value in data["variables"]:
            self.para_list.add(Parameter(**value))

    def summary(self):
        print "Total parameters: {}".format(len(self.para_list))
        for para in sorted(self.para_list, key=lambda para:para.id_):
            print para

    def generate(self, npoints):
        self.current = 0
        for para in self.para_list:
            para.sampling.generate(npoints)

    def fetch(self):
        self.current += 1
        for para in self.para_list:
            para.value = para.sampling.get()

    def get_config(self):
        out = "Random:setSeed = on     ! user-set seed\n"
        out += "Random:seed    = {} \n".format(str(int(self.current*3)))
        out += "\n".join([para.for_config() for para in self.para_list])
        return out

    def get_tune(self):
        return "\n".join([para.for_tune() for para in self.para_list])

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0]," json"
        sys.exist(1)

    tune = TuneMngr()
    tune.readInputJason(sys.argv[1])
    # tune.summary()
    nruns = 3
    tune.generate(nruns)
    irun = 0
    while True:
        irun += 1
        print "-------",irun,"-------------"
        try:
            tune.fetch()
        except IndexError:
            print "Index Error, stop!"
            break
        print "--------begin config------------"
        print tune.get_config()
        print "--------end config------------"
        print "--------begin tune------------"
        print tune.get_tune()
        print "--------end tune------------"
