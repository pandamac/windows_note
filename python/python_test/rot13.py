#!/usr/bin/env python
#  -*- coding: utf-8 -*-
#coding:rot13
__author__ = 'panda'
'''
用rot13对python的源代码加密一下。
代码1 是用 python 代码实现的 rot13 加密
代码2 是把代码1加密以后的样子，可以照样执行。
代码3 是把 代码1 加密成 代码2 的shell脚本
rot13的说明，请看百科  http://zh.wikipedia.org/wiki/ROT13
'''
def rot(s,OffsetX):
    def _rot(ch):
        try:
            asc = ord(ch)
        except:
            return ch
        if (asc > 96) and (asc < 123):#a-z
            return chr((asc-97+OffsetX).__mod__(26) + 97)
        elif (asc > 64) and (asc < 91):#A-Z
            return chr((asc-65+OffsetX).__mod__(26) + 65)
        else:
            return ch

    return ''.join([_rot( c ) for c in s])
print rot( 'cevag', 13 )
print rot( 'NOP', 13 )
print rot( '你好', 13 )
print rot( '57E9F980EABF49973F757CDB9F80D36C', 13 )