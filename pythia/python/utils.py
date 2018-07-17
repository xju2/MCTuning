# coding: utf-8
"""
Some utilities 
"""

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
