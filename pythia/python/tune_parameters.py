#!/usr/bin/env python
# python 2.7.13

import json
import sys

import math
import pandas as pd

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

def least_runs(n):
    # n is the number of parameters
    # return number of coefficient to be fitted
    return 1 + n + n*(n+1)/2

class Parameter:
    """
    Must contain a unique name,
    id, nickname and description are optional.
    """
    def __init__(self, name,  min_, max_,
                 nominal,
                 values, nickname,
                 description="None",
                 id_=-1
                ):
        self.name = name
        self.min_= min_
        self.max_= max_
        self.nominal = nominal

        # get a list of values of this parameter
        if type(values) is list:
            self.values = values
        elif type(values) is int:
            self.values = self.get_even_values(values)
        else:
            print "I don't know values:",values
            exit(2)

        self.nickname = nickname
        self.description = description
        self.id_ = id_
        self.run_values = []

    def get_even_values(self, values):
        scale = max(find_precision(self.max_)[1],
                    find_precision(self.min_)[1],
                    find_precision(values)[0]
                   ) + 1
        step = (self.max_ - self.min_)/(values - 1)
        return [round(self.min_ + x*step, scale) for x in range(values-1)] + [self.max_]

    def jsonDefault(self):
        return self.__dict__

    def to_str(self):
        return "{} {} ({}, {}, {}), {}".format(self.id_, self.name, self.min_, self.nominal, self.max_, len(self.values))

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

    def prof_config(self, value):
        return "{} {}".format(self.nickname, value)

    def pythia_config(self, value):
        return "{:<40} = {:<10} \t! {}".format(self.name, value, self.description)

class TuneMngr:
    """
    Manage tuned prameters and sampling of these parameters
    """
    def __init__(self, json_file):
        # avoid any duplicated variables from the input
        # two variables are the same, if they share the
        # same name
        self.para_list = set([])
        data = json.load(open(json_file))
        for value in data["variables"]:
            self.para_list.add(Parameter(**value))

        self.DOE = data.get("DOE", "Factorial")
        if self.DOE.lower() != "factorial" and \
           self.DOE.lower() != "one-to-one":
            print "I don't know how to do: ", self.DOE
            print "but it's OK, I will just use Factorial"
        self.summary()

    def minimum_runs_for_Prof(self):
        return least_runs(len(self.para_list))

    def runs_from_DOE(self):
        return 1

    def summary(self):
        print "\nBegin of parameter summary"
        print "  Design of Exp.:", self.DOE
        print "  Total parameters: {}".format(len(self.para_list))
        for para in sorted(self.para_list, key=lambda para:para.id_):
            print "\t",para

        print "End of parameter summary"

    def append_factors(self, para):
        new_list = [x for x in self.para_list if x != para]
        if len(new_list) > 0:
            para.run_values = para.values*reduce(lambda x, y: x*y, [len(z.values) for z in new_list])
        else:
            para.run_values = para.values

    def append_one2one(self, para):
        para.run_values = para.values + [para.nominal]*(self.max_len - len(para.values))


    def generate(self):
        if self.DOE.lower() == "one-to-one":
            self.max_len = max([len(para.values) for para in self.para_list])
            map(self.append_one2one, self.para_list)

        elif self.DOE.lower() == "factorial":
            map(self.append_factors, self.para_list)
        else:
            print "I do nothing."

        data = dict([(para.nickname, para.run_values) for para in self.para_list])
        self.df = pd.DataFrame(data=data)
        print "\n****Generated List of Parameters***"
        print self.df
        print "\n"
        return self.df.shape

    def get_config(self, irun):
        return "\n".join([para.pythia_config(self.df.iloc[irun][ip]) for ip, para in enumerate(self.para_list)])

    def get_tune(self, irun):
        return "\n".join([para.prof_config(self.df.iloc[irun][ip]) for ip, para in enumerate(self.para_list)])

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print sys.argv[0]," json"
        exit(1)

    print least_runs(1)
    print least_runs(5)
    print least_runs(6)
    print least_runs(10)
    print least_runs(100)
    print least_runs(500)
    print least_runs(1000)

    #sys.exit(0)
    tune = TuneMngr(sys.argv[1])

    tune.generate()
    irun = 0
    while irun < 1:
        try:
            print "--------begin config------------"
            print tune.get_config(irun)
            print "--------end config------------\n"
            print "--------begin tune------------"
            print tune.get_tune(irun)
            print "--------end tune------------\n"
        except IndexError:
            break
        irun += 1
