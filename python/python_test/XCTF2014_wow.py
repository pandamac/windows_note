__author__ = 'panda'
#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import numpy

NUM = 0x16
matrix = [[0 for col in range(NUM)] for row in range(NUM)]
strings = [
    "ThelightTokeepinmindtheholylight",
    "Timeismoneymyfriend",
    "WelcometotheaugerRuiMa",
    "Areyouheretoplayforthehorde",
    "ToarmsyeroustaboutsWevegotcompany",
    "Ahhwelcometomyparlor",
    "Slaytheminthemastersname",
    "YesrunItmakesthebloodpumpfaster",
    "Shhhitwillallbeoversoon",
    "Kneelbeforemeworm",
    "Runwhileyoustillcan",
    "RisemysoldiersRiseandfightoncemore",
    "LifeismeaningleshThatwearetrulytested",
    "BowtothemightoftheHighlord",
    "ThefirstkillgoestomeAnyonecaretowager",
    "Itisasitshouldbe",
    "Thedarkvoidawaitsyou",
    "InordertomoregloryofMichaelessienray",
    "Rememberthesunthewellofshame",
    "Maythewindguideyourroad",
    "StrengthandHonour",
    "Bloodandthunder"
]
verify = [
    0x000373ca,
    0x00031bdf,
    0x000374f7,
    0x00039406,
    0x000399c4,
    0x00034adc,
    0x00038c08,
    0x00038b88,
    0x00038a60,
    0x0002b568,
    0x00032471,
    0x00037dea,
    0x00036f97,
    0x000378e4,
    0x00038706,
    0x00029010,
    0x00034c23,
    0x00038ef8,
    0x00038e29,
    0x0003925e,
    0x0002b5fc,
    0x0002584e
]

def gen_matrix():
    # init
    for x in xrange(NUM):
        for y in xrange(NUM):
            matrix[x][y] = 0
    #assign
    for x in xrange(NUM):
        _len = len(strings[x])
        for y in xrange(_len):
            if y >= NUM:
                break;
            matrix[x][y] = ord(strings[x][y])

result = [104, 99, 116, 102, 123, 76, 74, 95, 121, 54, 99, 100, 99, 95, 113, 119, 101, 101, 114, 116, 33, 125]
if __name__ == '__main__':
    gen_matrix()
    matrix = numpy.array(matrix)
    verify = numpy.array(verify)
    print   numpy.linalg.solve(matrix, verify)#Solve a linear matrix equation, or system of linear scalar equations.
    for x in xrange(len(result)):
        sys.stdout.write(chr(result[x]))#hctf{LJ_y6cdc_qweert!}
'''
Examples

Solve the system of equations 3 * x0 + x1 = 9 and x0 + 2 * x1 = 8:

>>> a = np.array([[3,1], [1,2]])
>>> b = np.array([9,8])
>>> x = np.linalg.solve(a, b)
>>> x
array([ 2.,  3.])
Check that the solution is correct:

>>> np.allclose(np.dot(a, x), b)
True
'''