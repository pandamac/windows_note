#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
正则表达式
所以我们判断一个字符串是否是合法的Email的方法是：
创建一个匹配Email的正则表达式；
用该正则表达式去匹配用户的输入来判断是否合法。
\d 数字 \w 字母或数字  . 匹配任何字符 * 匹配任意个字符包括0个 \s 匹配空格 也包括tab等空符 + 表示至少一个字符 ? 表示0个或者1个 {n}表示n个字符 {n,m}表示n-m个字符
00\d = 0可以匹配 007 无法匹配 00a
py. 匹配 pyc pyo py@ 等
\d{3}\s+\d{3,8}
\d{3}表示3个数字 比如010
\s+ 至少一个空格
\d{3,8}  表示3-8个数字
综合起来，上面的正则表达式可以匹配以任意个空格隔开的带区号的电话号码
028 82283928
\d{3}\-\d{3,8} 表示 028-82283928
[0-9a-zA-Z\_] 可以匹配一个数字，字母或者下划线
[0-9a-zA-Z\_]+ 至少由一个，字母或者下划线组成的字符串 比如 a100 0_Z Py3000等
[a-zA-Z\_][0-9a-zA-Z\_]{0,19} 精确限制了变量的长度为 1-20个字符
\p
?<Month>  分组
A|B  可以匹配 A或者B  所以 [P|p]ython 可以匹配 Python 或者 python
^ 表示行的开头  ^\d 表示必须以数字开头
$ 表示行的结尾  \d$ 表示必须以数字结束
py 可以匹配 python  但是 ^py$ 只能匹配 py
#有了准备知识，我们就可以在Python中使用正则表达式了。Python提供re模块，包含所有正则表达式的功能。由于Python的字符串本身也用\转义，所以要特别注意：
s = 'Abc\\-001' #Python 字符串 对应正则表达式字符串为 'Abc\-001'
>>> print 'Abc\\-001'
Abc\-001
强烈使用 r 前缀  所以不用考虑转义问题了
>>> print r'Abc\\-001'
Abc\\-001

eg:
>>> re.match(r'^\d{3}\-\d{3,8}','010-12345')#Try to apply the pattern at the start of the string, returning a match object, or None if no match was found
<_sre.SRE_Match object at 0x027C5D08>
>>> re.match(r'^\d{3}\-\d{3,8}','010 12345')
>>>

>>> 'a b   c'.split(' ')
['a', 'b', '', '', 'c']

>>> re.split(r'[\s]+','a b c  a aaaa')
['a', 'b', 'c', 'a', 'aaaa']

>>> re.split(r'[\s\,]+','a,b,c  d   asd   ,asd   asdasd')
['a', 'b', 'c', 'd', 'asd', 'asd', 'asdasd']

>>> re.split(r'[\s\;\,]+','a,b ;asd   ;a;sdfasd  as;f;sdf;sdf,sdfsd;fsd,f;sd,f;sdf,ds;f;dsf')
['a', 'b', 'asd', 'a', 'sdfasd', 'as', 'f', 'sdf', 'sdf', 'sdfsd', 'fsd', 'f', 'sd', 'f', 'sdf', 'ds', 'f', 'dsf']
#除了简单地判断是否匹配之外，正则表达式还有提取子串的强大功能。用()表示的就是要提取的分组（Group)
>>> re.match('^(\d{3})-(\d{3,8})$','010-82283928').group(0)
'010-82283928'
>>> re.match('^(\d{3})-(\d{3,8})$','010-82283928').group(1)
'010'
>>> re.match('^(\d{3})-(\d{3,8})$','010-82283928').group(2)
'82283928'

时间匹配
>>> import time
>>> print time.strftime('%Y-%m-%d')
2014-10-22

import re
print re.match(r'^([0-9]|[1-9][0-9]|[1-9][0-9][0-9]|[1-2][0-9][0-9][0-9])-([0-9]|1[0-9])-([0-9]|[1-3][0-9])$','2014-10-22').groups()
#对于'2-30'，'4-31'这样的非法日期，用正则还是识别不了，或者说写出来非常困难，这时就需要程序配合识别了

#正则匹配默认是贪婪匹配，也就是匹配尽可能多的字符
>>> re.match(r'^(\d+)(0*)$', '102300').groups()
('102300', '')   #(\d+) 包含了 102300 全部 结果0*只能匹配空字符串了
必须让\d+采用非贪婪匹配（也就是尽可能少匹配），才能把后面的0匹配出来，加个?就可以让\d+采用非贪婪匹配：
>>> re.match(r'^(\d+?)(0*)$','123000').groups()
('123', '000')

当我们在Python中使用正则表达式时，re模块内部会干两件事情：
编译正则表达式，如果正则表达式的字符串本身不合法，会报错；
用编译后的正则表达式去匹配字符串。
如果一个正则表达式要重复使用几千次，出于效率的考虑，我们可以预编译该正则表达式，接下来重复使用时就不需要编译这个步骤了，直接匹配：
>>> import re
>>> re_telephone = re.compile(r'^(\d{3})-(\d{3,8})$')
>>> re_telephone.match('010-12345').groups()
('010', '12345')
>>> re_telephone.match('010-12345123').groups()
('010', '12345123')
编译后生成Regular Expression对象，由于该对象自己包含了正则表达式，所以调用对应的方法时不用给出正则字符串。
import re
data = 'Tom has 3 email, they are bill.gates@microsoft.com 123 someone@gmail.com and <Tom Paris> tom@voyager.org'
#for e in re.findall(r'[\w.]+@[\w.]+',data):
#    print e
for e2 in re.findall(r'<[^=]+@[\w.]+',data):  # 其中 [^=]表示除开= 的外的
    print e2

#>>> re.search(r'<(.+)>.+@.+\..+','<Tom Paris> tom@voyager.org').group()
#'<Tom Paris> tom@voyager.org'

import re
p = re.compile('\d+')
iterator = p.finditer('12  safsdf      sdf   sdfdsf   sdf   sdfsdf sdf 123 24 42134')
for match in iterator:#迭代器
    print match.group()
#为了匹配字母 "|"，可以用 \|，或将其包含在字符类中，如[|]。

import re
a = re.compile(r'\A\w+-\Z\w+')
print a.match(r'101-123').group()

#!python       \b  单词边界。这是个零宽界定符（zero-width assertions）只用以匹配单词的词首和词尾
>>> p = re.compile(r'\bclass\b')#如果你没有使用 raw 字符串时，那么 Python 将会把 "\b" 转换成一个回退符
>>> print p.search('no class at all')
<re.MatchObject instance at 80c8f28>

>>> a.match('abcd').group(0,0,0)
('abcd', 'abcd', 'abcd')

#!python  下面的 RE 在一个字符串中找到成双的词
#>>> p = re.compile(r'(\b\w+)\s+\1')
#>>> p.search('Paris in the the spring').group()
#'the the'
'''
import re

with open(r'C:\Users\panda\Desktop\1.txt','rb') as f:
    a = f.read().decode('gbk')
    print a[:300]
#   print re.search(r'\w{3,20}\.UI',a).groups()
    for e2 in re.findall(r'\w{3,20}\.UI',a):
        print e2





