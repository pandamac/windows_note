#!/usr/bin/env python
# -*- coding: utf-8 -*-

__author__ = 'panda'
'''
language  = u'中文呢'
name = 'panda'
print 'hello %s, your language is %s' % (name,language)
classmates = ['1','2',['3','4']]
print 'classmates[0] type = %s is %s' % (type(classmates[0]),classmates[0])
print 'classmates[0:1] type = %s is %s' % (type(classmates[0:1]),classmates[0:1])
print 'classmates[0:2] type = %s is %s' % (type(classmates[0:2]),classmates[0:2])
print 'classmates[2] type = %s is %s' % (type(classmates[2]),classmates[2])
classmates = ('1','2',['3','4'])
classmates[2][0] = '0'
classmates[2][1] = '0'
print classmates
age = 3
if age > 5:
    print 'age<5'
else:
    print 'age>=5'
names = ['panda1','panda2','panda3']
for name in names:
    print name

birth = raw_input('input your birth: ')
if int(birth) <2000:
    print u"00前"
else:
    print u"00后"

x=2
while x>1:
    print 'aaaaaaaaa'

d = {'panda1':95,'panda2':100,'panda3':11,'panda4':123}
print d['panda1']
print 'panda5' in d
print d.get('panda5')
print d.get('panda5',123123)
print d.get('panda4')
print d
print type(d)
d['panda5'] = 123123
print d
d.pop('panda1')
print d
d['panda6']=1111111111
print d
for x in d:
    print '%s is %s\n' %(x,d[x])
print d.items()

s = set([3,4,5])
print s
a = set([1,1,1,2,2,2,3,3,3])
print a
a.add(4)
print a
a.remove(4)
print a
print s&a
print a|s
print type(s&a)
c = [c for c in s|a]
print 'c is %s tpye is %s\n' % (c,type(c))
a = [5,2,1,3,4]
print a
'''
'''
a = 'abc'
a.replace('a','A')
print a
print a.replace('a','A')
'''
'''a = (1,2,3)
print a
'''
'''
import math

def my_abs(x):
    if not isinstance(x,(int,float)):
        raise TypeError('bad operand type')
    if x<0:
        return -x
    else:
        return x
print my_abs(-1)
print my_abs(1)
print my_abs('1')

def move(x,y,step,angle=0):
    nx = x + step * math.cos(angle)
    ny = y - step * math.sin(angle)
    return nx,ny
x = move(100,100,60,math.pi/6)
#print 'x is %s \ntype is %s \n' % (x,type(x))

def modify(x,y):
    nx = 1
    ny = 2
    return nx,ny
a = 123
b = 321
a,b = modify(a,b)
#print a,b
'''
'''
import math

def power(x,n=2):
    n2=0
    while n2 <= n:
        n2 = n2+1
        x = x*x
    return x

print 'hehe'
print power(2,3)

def addEnd(L=None):
    if L == None:
        L = []
    L.append('End')
    return L
print addEnd()
print addEnd(['1','2','3'])

def addArrary(*num):
    sum = 0
    for x in num:
        sum = sum + x*x
    return sum
print addArrary(1,2)
abc = [1,2,3]
print addArrary(*abc)
'''
'''
def func(a,b,c=0,*args,**kw):
    print 'args type is',type(args)#args type is <type 'tuple'>
    print 'kw type is',type(kw)    #kw type is <type 'dict'>
    print 'a=',a,' b=',b,' c=',c,' args=',args,' kw=',kw
#func(1,2,3,4,5,6)
#func(1,2,3,'a','b','c','d')
#func(1,2,3,'a','b','c','d',x=123)

a = (1,2,3,4,5,6)
b = {'panda':123,'panda2':456,'panda3':789}
#print type(*a)
#print type(**b)
#func(*a,**b)#a= 1  b= 2  c= 3  args= (4, 5, 6)  kw= {'panda2': 456, 'panda': 123, 'panda3': 789}
func(1,2,3)
func(*(1,2,3))
func(**{'a':1,'b':2})
'''
'''
def func(x):
    if x==1:
        return 1
    return x*func(x-1)
#print func(1000) zhan yichu
#尾递归是指，在函数返回的时候，调用自身本身，并且，return语句不能包含表达式。这样，编译器或者解释器就可以把尾递归做优化，使递归本身无论调用多少次，都只占用一个栈帧，不会出现栈溢出的情况。
#遗憾的是，大多数编程语言没有针对尾递归做优化，Python解释器也没有做优化，所以，即使把上面的fact(n)函数改成尾递归方式，也会导致栈溢出。
def fact(x):
    return fact2(1,1,x)
def fact2(sum,i,x):
    if i>x:
        return sum
    return fact2(sum*i,i+1,x)
print fact(1000)
>>> l[::]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55
, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99]
>>> l[::5]
[0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95]

>>> a[:]
(1, 2, 3, 4, 5)
>>> a[::2]
(1, 3, 5)
>>> a[:]
(1, 2, 3, 4, 5)

a = {'panda1':123,'panda2':456,'panda3':789}
for i in a:
    print '%s is %s' % (i,a[i])
for i in a.itervalues():
    print i
for x,y in a.iteritems():
    print x,'   ',y
'''
'''
from collections import Iterable
print '\'abc\' is Iterable? %s',isinstance('abc',Iterable)
print '\'[1,2,3]\' is Iterable? %s',isinstance([1,2,3],Iterable)
print '123 is Iterable? %s',isinstance(123,Iterable)
#enumerate
>>> for i ,value in enumerate(['panda','bob','alice']):
...     print i,value
0 panda
1 bob
2 alice

>>> for x,y in [(1,2),(3,4),(5,6)]:     print x,y
1 2
3 4
5 6

>>> for i in range(1,10):
...     l.append(i*i)
[1, 4, 9, 16, 25, 36, 49, 64, 81]
>>> [x*x for x in range(1,10)]
[1, 4, 9, 16, 25, 36, 49, 64, 81]
>>> [x*x for x in range(1,10) if x % 2 ==0]
[4, 16, 36, 64]

>>> [m+n for m in 'abcdefg' for n in 'abcdefg']
['aa', 'ab', 'ac', 'ad', 'ae', 'af', 'ag', 'ba', 'bb', 'bc', 'bd', 'be', 'bf', 'bg', 'ca', 'cb', 'cc', 'cd', 'ce', 'cf', 'cg', 'da', 'db', 'dc', 'dd', 'de', 'df', 'dg', 'ea', 'eb', 'ec', 'ed', 'ee', 'ef', 'eg', 'f
a', 'fb', 'fc', 'fd', 'fe', 'ff', 'fg', 'ga', 'gb', 'gc', 'gd', 'ge', 'gf', 'gg']

>>> import os
>>> [d for d in os.listdir('.')]
['.idea', 'panda_python_study.py', 'tail_call_optimized.py', 'tail_call_optimized.pyc']

>>> d={'panda':123,'bob':456,'alice':789}
>>> [a +'='+ str(b) for a,b in d.iteritems()]
['bob=456', 'panda=123', 'alice=789']

>>> L= ['PANDA','BOB','ALICE']
>>> [x.lower() for x in L]
['panda', 'bob', 'alice']

>>> a=123
>>> isinstance(a,str)
False

>>> a
['PANDA', 'BOB', 123, 'ALICE']
>>> b
[]
>>> for x in a:
...     if isinstance(x,str):
...             b.append(x.lower())
...     else:
...             b.append(x)
>>> b
['panda', 'bob', 123, 'alice']


import math
N=1000
flag=1
L=[]
for x in range(2,N):
    flag=1
    for y in range(2,int(math.sqrt(x))+2):
        if x==y:
            continue
        if x % y ==0:
            flag = 0
            break
    if flag ==1:
        #print '%s is prime number' % x
        L.append(x)
print L
'''
'''
#如果列表元素可以按照某种算法推算出来，那我们是否可以在循环的过程中不断推算出后续的元素呢？这样就不必创建完整的list，从而节省大量的空间。在Python中，这种一边循环一边计算的机制，称为生成器（Generator）
>>> l = [x*x for x in [1,2,3]]
>>> l
[1, 4, 9]
>>> g = (x*x for x in [1,2,3])
>>> g
<generator object <genexpr> at 0x026C38C8>

a = (x*x for x in range(10))
print a.next();
print a.next();
print a.next();
#我们讲过，generator保存的是算法，每次调用next()，就计算出下一个元素的值，直到计算到最后一个元素，没有更多的元素时，抛出StopIteration的错误
>>> from collections import Iterable
>>> isinstance(g,Iterable)
>>> g = (x*x for x in range(10))
>>> for x in g:
...     print x

def fib(max):
    n, a, b = 0, 0, 1
    while n < max:
        print b
        a, b = b, a + b #隔离进行运算的 1 1 2 3 5
        n = n + 1
fib(10)#普通的函数 和generator 很像，变成generator 只要改写 print 为 yield

def fib(max):
    n, a, b = 0, 0, 1
    while n < max:
        yield b
        a, b = b, a + b #隔离进行运算的
        n = n + 1
print fib(10) #<generator object fib at 0x027539E0>
#这里，最难理解的就是generator和函数的执行流程不一样。函数是顺序执行，遇到return语句或者最后一行函数语句就返回。而变成generator的函数，在每次调用next()的时候执行，遇到yield语句返回，再次执行时从上次返回的yield语句处继续执行。
a = fib(10)
print a.next()
print a.next()
print a.next()
print a.next()
print a.next()
print 'test over ``````````````````````````'
for x in a:
    print x
# 1 1 2 3 5
#generator是非常强大的工具，在Python中，可以简单地把列表生成式改成generator，也可以通过函数实现复杂逻辑的generator。
#要理解generator的工作原理，它是在for循环的过程中不断计算出下一个元素，并在适当的条件结束for循环。对于函数改成的generator来说，遇到return语句或者执行到函数体最后一行语句，就是结束generator的指令，for循环随之结束。
'''
'''
#我们先看map。map()函数接收两个参数，一个是函数，一个是序列，map将传入的函数依次作用到序列的每个元素，并把结果作为新的list返回。
>>> def func(x):
...     return x*x
>>> map(func,[1,2,3,4,5])
[1, 4, 9, 16, 25]

再看reduce的用法。reduce把一个函数作用在一个序列[x1, x2, x3...]上，这个函数必须接收两个参数，reduce把结果继续和序列的下一个元素做累积计算，其效果就是：
>>> def func2(x,y):
...     return x+y
>>> reduce(func2,[1,2,3,4,5])  #求序列和
15

>>> def func(x,y):
...     return x*10+y
>>> def func2(x):
...     return {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9}[x]
>>> reduce(func,map(func2,'13579'))
13579

def str2int(s):
    def fn(x, y):
        return x * 10 + y
    def char2num(s):
        return {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9}[s]
    return reduce(fn, map(char2num, s))

#关键字lambda表示匿名函数，冒号前面的x表示函数参数。
匿名函数有个限制，就是只能有一个表达式，不用写return，返回值就是该表达式的结果。
>>> map(lambda x: x * x, [1, 2, 3, 4, 5, 6, 7, 8, 9])
[1, 4, 9, 16, 25, 36, 49, 64, 81]

#模拟 int()
>>> def func(x):
...     return {'0': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9}[x]
>>> reduce(lambda x,y: x*10+y, map(func,'13578'))
13578
'''
'''
>>> sorted([123,412,2,0,-1,4124])
[-1, 0, 2, 123, 412, 4124]

>>> def cmpF(x,y):
...     if x>y:
...             return -1
...     elif x==y:
...             return 0
...     else:
...             return 1
>>> sorted([1,-1,3123,2,124,124,123142,124,21,3],cmpF)
[123142, 3123, 124, 124, 124, 21, 3, 2, 1, -1]

>>> def cmp_ignore_case(x,y): #忽略大小写的排序
...     x2 = x.upper()
...     y2 = y.upper()
...     if x2 < y2:
...             return -1
...     if x2 > y2:
...             return 1
...     return 0
>>> sorted(['Ada','asdas','BBBSF','zzzz','AAZC'],cmp_ignore_case)
['AAZC', 'Ada', 'asdas', 'BBBSF', 'zzzz']

>>> def add_(*args):
...     sum=0
...     for x in args:
...             sum = sum+x
...     return sum
>>> add_(1,2,3,4,5)
15

>>> def func(*args):
...     def sum():
...             ax=0
...             for n in args:
...                     ax = ax + n
...             return ax
...     return sum
>>> func(1,2,3,4,5)()
15
>>> f1 = func(1, 3, 5, 7, 9)
>>> f2 = func(1, 3, 5, 7, 9)
>>> f1==f2
False

def my_map(func,ulist): #模拟map()
    return [func(x) for x in ulist]
def func2(x):
    return x*x
print my_map(func2,[1,2,3])

def prod(ulist):#构造输入一个list 的函数得到乘积
    return reduce(lambda x,y: x*y,ulist)
print prod([1,2,3,4,5])
'''
'''
>>> def func():
...     print u'中国人'
>>> a = func
>>> a.__name__
'func'

#由于log()是一个decorator，返回一个函数，所以，原来的now()函数仍然存在，只是现在同名的now变量指向了新的函数，于是调用now()将执行新函数，即在log()函数中返回的wrapper()函数。
#简单版本
def log(func):
    def warpper(*args,**kw):
        print 'call %s:' % func.__name__
        return func(*args,**kw)
    return warpper
@log#方式 1
def now2():
    print '2014'
now2()

def now():#方式 2
    print '2014'
now = log(now)
now()
#之后是自定义文本输出
def log(text):
    def decorator(func):
        def wrapper(*args, **kw):
            print '%s %s():' % (text, func.__name__)
            return func(*args, **kw)
        return wrapper
    return decorator
@log('execute')
def now():
    print '2013-12-25'

>>> now()
execute now():
2013-12-25

>>> now = log('execute')(now)
#这样 函数的  __name__ 属性被改变了 用 @functools.wraps(func) 让函数名不变
#完整版本
import functools

def log(func):
    @functools.wraps(func)
    def wrapper(*args,**kw):
        print 'call %s' % func.__name__
        return func(*args,**kw)
    return wrapper
def time_():
    print '2014'
a = log(time_)
print a.__name__

import functools#实现 输入无 或者 输入 有 的情况下 打印
def log(text=None):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kw):
            if isinstance(text, str):
                print text+': %s' % func.__name__
                return func(*args, **kw)
            else:
                print 'default : %s' % func.__name__
                return func(*args, **kw)
        return wrapper
    return decorator
@log('11111')#有字符串 方法1
def call_():
    print 'panda\n'
call_()

log('22222')(call_)()#有字符串 输出两次 方法2

@log()
def call_2():
    print 'panda2\n'
call_2()#无字符串 方法1

log()(call_2)()  #无字符串 输出两次方法2

#关键点在于装饰器的实际意义
#@log 相当于now = log(now)
#@log(text) 相当于 now = log(text)(now)

import functools #前后输出

def log(func):
    @functools.wraps(func)
    def wrapper(*args, **kw):
        print 'begin call'
        func(*args, **kw)
        print 'end call'
    return wrapper
@log
def now():
    print '2014-08-12'
now()
def now2():
    print '2014-10-20'
log(now2)()
'''
'''
#偏函数
def int2(x,base=2):
    return int(x,base)
print int2('1111100000')

import functools#简单总结functools.partial的作用就是，把一个函数的某些参数（不管有没有默认值）给固定住（也就是设置默认值），返回一个新的函数，调用这个新函数会更简单
int2 = functools.partial(int,base=2)
print int2('10000')
'''
'''
#一个.py文件就称之为一个模块（Module）
#一个abc.py的文件就是一个名字叫abc的模块，一个xyz.py的文件就是一个名字叫xyz的模块。
#引入了包以后，只要顶层的包名不与别人冲突，那所有模块都不会与别人冲突。现在，abc.py模块的名字就变成了mycompany.abc，类似的，xyz.py的模块名变成了mycompany.xyz。
#请注意，每一个包目录下面都会有一个__init__.py的文件，这个文件是必须存在的，否则，Python就把这个目录当成普通目录，而不是一个包
#__init__.py本身就是一个模块，而它的模块名就是mycompany。

#!/usr/bin/env python
# -*- coding: utf-8 -*-
#当我们在命令行运行hello模块文件时，Python解释器把一个特殊变量__name__置为__main__，而如果在其他地方导入该hello模块时，if判断将失败，
# 因此，这种if测试可以让一个模块通过命令行运行时执行一些额外的代码，最常见的就是运行测试。
__author__ = 'panda'
import sys
def test():
    args = sys.argv
#    print args
#    print len(args)
    if len(args) == 1:
        print 'hello world'
    elif len(args) == 2:
        print 'hello, %s' % args[1]
    else:
        print 'too many arguments'
if __name__=='__main__':
    test()
#python panda_python_study.py panda 命令行运行
#hello, panda

#>>> import panda_python_study IDE中运行
#>>> panda_python_study.test()
#hello world

try:
    import cStringIO as StringIO
except ImportError:
    import StringIO

try:
    import json # python >=2.6
except ImportError:
    import simplejson as json #python <=2.5
#hello模块定义的文档注释也可以用特殊变量__doc__访问
#类似__xxx__这样的变量是特殊变量，可以被直接引用，但是有特殊用途，比如上面的__author__，__name__就是特殊变量
#类似_xxx和__xxx这样的函数或变量就是非公开的（private），不应该被直接引用，比如_abc，__abc等

def _private_1(name):
    print 'hello,%s' % name
def _private_2(name):
    print 'Hi,%s' % name
def greeting(name):
    if len(name) > 3:
        return _private_1(name)
    else:
        return _private_2(name)
greeting('panda')
'''
'''
#https://pypi.python.org/pypi
easy_install PIL #图形库

import Image
im = Image.open('C:\Users\panda\Desktop\证明.png')
print im.format,im.size,im.mode
im.thumbnail((200,100))
im.save('thumb.jpg','JPEG')
'''
'''
#Python提供了__future__模块，把下一个新版本的特性导入到当前版本，于是我们就可以在当前版本中测试一些新版本的特性
# still running on Python 2.7

class Student(object):
    def __init__(self,name,score):#，把一些我们认为必须绑定的属性强制填写进去。通过定义一个特殊的__init__方法，在创建实例的时候，就把name，score等属性绑上去
        self.name = name
        self.score = score

    def print_score(self):
        print '%s : %s' % (self.name,self.score)
bart = Student('Bart',100)
lisa = Student('Lisa',123)
bart.print_score()
lisa.print_score()
#如果要让内部属性不被外部访问，可以把属性的名称前加上两个下划线__，在Python中，实例的变量名如果以__开头，就变成了一个私有变量（private），只有内部可以访问，外部不能访问

class Student(object):
    def __init__(self,name,score):
        self.__name = name
        self.__score = score

    def print_score(self):
        print '%s : %s' % (self.__name,self.__score)
bart = Student('Bart',100)
bart.print_score()
print bart.__score#不能访问 又要允许就设置个函数 def set_score(self,score);
#需要注意的是，在Python中，变量名类似__xxx__的，也就是以双下划线开头，并且以双下划线结尾的，是特殊变量，特殊变量是可以直接访问的，不是private变量，所以，不能用__name__、__score__这样的变量名。
#有些时候，你会看到以一个下划线开头的实例变量名，比如_name，这样的实例变量外部是可以访问的，但是，按照约定俗成的规定，当你看到这样的变量时，意思就是，“虽然我可以被访问，但是，请把我视为私有变量，不要随意访问”
#双下划线开头的实例变量是不是一定不能从外部访问呢？其实也不是。不能直接访问__name是因为Python解释器对外把__name变量改成了_Student__name，所以，仍然可以通过_Student__name来访问__name变量： 但是强烈建议你不要这么干，因为不同版本的Python解释器可能会把__name改成不同的变量名。

class Animal(object):
    def run(self):
        print 'Animal is running'

class Dog(Animal):
    def run(self):
        print 'Dog is running...'
    def eat(self):
        print 'eating meat...'
#dog = Dog()
#dog.run()#当子类和父类都存在相同的run()方法时，我们说，子类的run()覆盖了父类的run()，在代码运行的时候，总是会调用子类的run()。这样，我们就获得了继承的另一个好处：多态
#c = Dog() # c是Dog类型
#>>> isinstance(c, Animal)
#True
#>>> b = Animal()
#>>> isinstance(b, Dog)
#False
def run_(Animal):
    Animal.run()
run_(Dog())
run_(Animal())
#最后注意到有一种类型就叫TypeType，所有类型本身的类型就是TypeType，比如：
#>>> type(int)==type(str)==types.TypeType
#True

#如果要获得一个对象的所有属性和方法，可以使用dir()函数，它返回一个包含字符串的list
>>> import os
>>> dir(os)

如果也想用len(myObj)的话，就自己写一个__len__()方法： def __len__(self):
仅仅把属性和方法列出来是不够的，配合getattr()、setattr()以及hasattr()，我们可以直接操作一个对象的状态：
hasattr(obj,'y',19)
>>> getattr(obj, 'z', 404) # 获取属性'z'，如果不存在，返回默认值404
404
>>> hasattr(obj, 'power') # 有属性'power'吗？
True
>>> getattr(obj, 'power') # 获取属性'power'

>>> def set_age(self, age): # 定义一个函数作为实例方法
...     self.age = age
>>> from types import MethodType
>>> s.set_age = MethodType(set_age, s, Student) # 给实例绑定一个方法
>>> s.set_age(25) # 调用实例方法
>>> s.age # 测试结果
25

为了给所有实例都绑定方法，可以给class绑定方法：
>>> def set_score(self, score):
...     self.score = score
>>> Student.set_score = MethodType(set_score, None, Student)

使用__slots__
但是，如果我们想要限制class的属性怎么办？比如，只允许对Student实例添加name和age属性。

class Student(object):
    __slots__ = ('name','age')#tuple 定义允许绑定的属性名称
    def __init__(self,name,age):
        self.name = name
        self.age = age
    def run(self):
        print 'name =%s, age = %s' % (self.name,self.age)
panda = Student('panda',100)
panda.run()
#panda.score = 100
#Traceback (most recent call last):
#  File "<stdin>", line 1, in <module>
#AttributeError: 'Student' object has no attribute 'score'
#使用__slots__要注意，__slots__定义的属性仅对当前类起作用，对继承的子类是不起作用的：
#除非在子类中也定义__slots__，这样，子类允许定义的属性就是自身的__slots__加上父类的__slots__。
class Bob(Student):
    __slots__ = ('mathscore')
    def eat(self):
        print 'eat'

b = Bob('Bob',100)
b.run()
b.score = 100

#为了限制score的范围，可以通过一个set_score()方法来设置成绩，再通过一个get_score()来获取成绩，这样，在set_score()方法里，就可以检查参数：
#还记得装饰器（decorator）可以给函数动态加上功能吗？对于类的方法，装饰器一样起作用。Python内置的@property装饰器就是负责把一个方法变成属性调用的
#@property的实现比较复杂，我们先考察如何使用。把一个getter方法变成属性，只需要加上@property就可以了、

class Student(object):

    @property#1)把一个getter方法变成属性，只需要加上@property就可以了
    def score(self):
        return self._score

    @score.setter
    def score(self,value):#2)负责把一个setter方法变成属性赋值
        if not isinstance(value,int):
            raise ValueError('score must be an integer')
        elif value< 0 or value >100:
            raise ValueError('socre is not property')
        else:
            self._score = value
            print 'score is %s' % self._score
panda = Student()
panda.score = 10
print panda.score#1)方法当作属性来使用
panda.score = 99#2)负责把一个setter方法变成属性赋值
#@property广泛应用在类的定义中，可以让调用者写出简短的代码，同时保证对参数进行必要的检查，这样，程序运行时就减少了出错的可能性。
'''
'''
#多继承
class Runnable(object):
    def run(self):
        print('Running...')
class Flyable(object):
    def fly(self):
        print('Flying...')
class Animal(object):
    pass
# 大类:
class Mammal(Animal):
    pass
class Bird(Animal):
    pass
# 各种动物:
class Dog(Mammal,Runnable):
    pass
class Bat(Mammal,Flyable):
    pass
class Parrot(Bird,Flyable):
    pass
class Ostrich(Bird,Runnable):
    pass
bob = Dog()
bob.run()

#让Ostrich鸵鸟 除了继承自Bird外，再同时继承Runnable。这种设计通常称之为Mixin。
#只允许单一继承的语言（如Java）不能使用Mixin的设计
class Student(object):
    def __init__(self,name):
        self.name = name

    def __str__(self):
        return 'Student object name : %s' % self.name
print Student('panda')
#细心的朋友会发现直接敲变量不用print，打印出来的实例还是不好看,解决办法是再定义一个__repr__()。但是通常__str__()和__repr__()代码都是一样的，所以，有个偷懒的写法
class Student(object):
    def __init__(self,name):
        self.name = name

    def __str__(self):
        return 'Student object name : %s' % self.name
    __repr__ = __str__
Student('panda')

#如果一个类想被用于for ... in循环，类似list或tuple那样，就必须实现一个__iter__()方法，该方法返回一个迭代对象，然后，Python的for循环就会不断调用该迭代对象的next()方法拿到循环的下一个值，直到遇到StopIteration错误时退出循环。
class Fib(object):
    def __init__(self):
        self.a, self.b = 0, 1 # 初始化两个计数器a，b

    def __iter__(self):
        return self # 实例本身就是迭代对象，故返回自己

    def next(self):
        self.a, self.b = self.b, self.a + self.b # 计算下一个值
        if self.a > 100000: # 退出循环的条件
            raise StopIteration();
        return self.a # 返回下一个值
for x in Fib():
    print x

#要实现  index 方式存取 要实现 __getitem__方法
class fib(object):
    def __getitem__(self, n):
        a,b = 0,1
        for x in range(n):
            a,b = b,a+b
            if a > 1000000:
                raise StopIteration()
        return a
for x in range(100):
    print fib()[x]

#如果 像列表切片就不行了 那么要求设置判断传入参数是否是slice 切片
class fib(object):
    def __getitem__(self, n):
        if isinstance(n,int):
            a,b =1,1
            for x in range(n):
                a,b = b,a+b
            return a
        if isinstance(n,slice):
            start = n.start
            stop = n.stop
            a,b = 1,1
            L = []
            for x in range(stop):
                if x >= start:
                    L.append(a)
                a,b = b,a+b
            return L
print fib()[0:10]
print [1,2,3,4,5][-1]
#也没有对负数作处理，所以，要正确实现一个__getitem__()还是有很多工作要做的。
#与之对应的是__setitem__()方法，把对象视作list或dict来对集合赋值。最后，还有一个__delitem__()方法，用于删除某个元素。
#常情况下，当我们调用类的方法或属性时，如果不存在，就会报错
#当调用不存在的属性时，比如score，Python解释器会试图调用__getattr__(self, 'score')来尝试获得属性
#注意到任意调用如s.abc都会返回None，这是因为我们定义的__getattr__默认返回就是None
class Student(object):
    def __getattr__(self, attr):
        if attr=='age':
            return lambda: 25
>>> s.age()
25

__call__
#一个对象实例可以有自己的属性和方法，当我们调用实例方法时，我们用instance.method()来调用。能不能直接在实例本身上调用呢？类似instance()
>>> class Student(object):
...     def __call__(self):
...             print 'My name is %s' % self.name
...     def __init__(self,name):
...             self.name = name
>>> g = Student('panda')
>>> g()
My name is panda
#__call__()还可以定义参数。对实例进行直接调用就好比对一个函数进行调用一样，所以你完全可以把对象看成函数，把函数看成对象，因为这两者之间本来就没啥根本的区别。
#通过callable()函数，我们就可以判断一个对象是否是“可调用”对象。
>>> callable(Student)
True

#当Python解释器载入panda_python_study模块时，就会依次执行该模块的所有语句，执行结果就是动态创建出一个Student的class对象

class Student(object):
    def Panda(self,name='panda'):
        print 'name is %s' % name
#g = Student()
#g.Panda()

#外部文档中调用这个类 from xxx.py import 类名
from panda_python_study import Student
h = Student()
h.Panda()
print type(Student)
print type(h)
#<type 'type'>
#<class 'panda_python_study.Student'>
#Student是一个class，它的类型就是type

import sys #导入sys.py
from sys import argv,path  #  导入特定的成员
如果你要使用所有sys模块使用的名字，你可以这样：
from sys import *
#!!! type()函数既可以返回一个对象的类型，又可以创建出新的类型，比如，我们可以通过type()函数创建出Hello类，而无需通过class Hello(object)...的定义：
class type(object)
 |  type(object) -> the object's type
 |  type(name, bases, dict) -> a new type

 >>> def func(self,name='world'):
...     print 'Hello %s' % name
>>> Student = type('panda',(object,),dict(hello=func))
>>> h = Student()
>>> h.hello()
Hello world

要创建一个class对象，type()函数依次传入3个参数：
class的名称；
继承的父类集合，注意Python支持多重继承，如果只有一个父类，别忘了tuple的单元素写法；
class的方法名称与函数绑定，这里我们把函数fn绑定到方法名hello上。
！通过type()函数创建的类和直接写class是完全一样的，因为Python解释器遇到class定义时，仅仅是扫描一下class定义的语法，然后调用type()函数创建出class。

除了使用type()动态创建类以外，要控制类的创建行为，还可以使用metaclass。
metaclass允许你创建类或者修改类。换句话说，你可以把类看成是metaclass创建出来的“实例”
metaclass是Python面向对象里最难理解，也是最难使用的魔术代码。正常情况下，你不会碰到需要使用metaclass的情况，所以，以下内容看不懂也没关系，因为基本上你不会用到。
http://www.liaoxuefeng.com/wiki/001374738125095c955c1e6d8bb493182103fac9270762a000/001386820064557c69858840b4c48d2b8411bc2ea9099ba000
'''
'''
try:
    r = 10/int('a')
    print '111111'
except ValueError,e:
    print 'ValueError:',e
except ZeroDivisionError,e:
    print 'except:',e
finally:
    print 'finally`````'
print 'END'

try:
    foo()
except StandardError, e:
    print 'StandardError: ',e
except ValueError, e:
    print 'ValueError',e
#第二个except永远也捕获不到ValueError，因为ValueError是StandardError的子类，如果有，也被第一个except给捕获了。
#Python所有的错误都是从BaseException类派生的，常见的错误类型和继承关系看这里：
#https://docs.python.org/2/library/exceptions.html#exception-hierarchy
'''
'''
try:
    f = open('/path/to/file', 'r')
    print f.read()
finally:
    if f:
        f.close()
with open('d:/1.log','r') as f:#Python引入了with语句来自动帮我们调用close()方法,这和前面的try ... finally是一样的
    print f.read()#调用read()会一次性读取文件的全部内容，如果文件有10G，内存就爆了,so read(size)

with open('d:/1.log','r') as f:
    print f.readline()#ret one line
    print f.readlines()#ret list

with open('d:/1.log','r') as f:
    for x in f.readlines():
        print x.strip()
#       print x#这样会看到每行都有换行 所以一把用x.strip()

#像open()函数返回的这种有个read()方法的对象，在Python中统称为file-like Object。除了file外，还可以是内存的字节流，网络流，自定义流等等。file-like Object不要求从特定类继承，只要写个read()方法就行。

with open('f:/big_.jpg','rb') as f:#前面讲的默认都是读取文本文件，并且是ASCII编码的文本文件。要读取二进制文件，比如图片、视频等等，用'rb'模式打开文件即可
    print f.read()

with open('d:/1.log','rb') as f:
    print f.read().decode('gbk')
#Python还提供了一个codecs模块帮我们在读文件时自动转换编码，直接读出unicode：

import codecs
with codecs.open('d:/1.log','r','gbk') as f:
    print f.read()
#pyc文件是十分有用的——它会快得多，因为一部分输入模块所需的处理已经完成了

import os
with open('os_info.txt','wb') as f:
    f.write(str(os.environ))
with open('os_info.txt','rb') as f2:
    print f2.read()
os.system('del os_info.txt')
>>> os.path
<module 'ntpath' from 'D:\Program Files (x86)\Python27\lib\ntpath.pyc'>
>>> os.path.abspath('.') 绝对目录
'G:\\beiyong\\biancheng\\python\\python_test'
>>> os.mkdir('panda')
>>> os.rmdir('panda')

>>> os.path.join(os.path.abspath('.'),'panda') #组合在一起
'G:\\beiyong\\biancheng\\python\\python_test\\panda'

>>> os.path.split(os.path.join(os.path.abspath('.'),'panda')) #拆开
('G:\\beiyong\\biancheng\\python\\python_test', 'panda')
#os.path.splitext()可以直接让你得到文件扩展名
>>> os.path.splitext(os.path.join(os.path.abspath('.'),'panda.txt'))
('G:\\beiyong\\biancheng\\python\\python_test\\panda', '.txt')
>>> os.rename('1.txt','1.exe')
>>> os.remove('1.exe')

import shutil
shutil.copyfile
shutil.copy('1.txt','2.txt')

>>> [x for x in os.listdir('.')]
['.idea', '1.txt', '2.txt', 'panda', 'panda_python_study.py', 'panda_python_study.pyc', 'tail_call_optimized.py', 'tail_call_optimized.pyc', 'test1.py']
>>> [x for x in os.listdir('.') if os.path.isdir(x)]
['.idea', 'panda']
>>> [x for x in os.listdir('.') if os.path.isfile(x) and os.path.splitext(x)[1] == '.py']
['panda_python_study.py', 'tail_call_optimized.py', 'test1.py']

import os
import sys
#编写一个search(s)的函数，能在当前目录以及当前目录的所有子目录下查找文件名包含指定字符串的文件，并打印出完整路径：
def search(str_):
    def searchPath(path,str_):
        for x in os.listdir(path):
            if x != '.':
                x = os.path.join(path,x)
            if os.path.isfile(x):
                if x.find(str_) == -1:
                    continue
                else:
                    print x
            else:
                searchPath(x,str_)
    searchPath('.',str_)

if __name__ == "__main__":
    search(sys.argv[1])

import os
def search(s):
    path = os.path.abspath('.')
    for root,dirs,files in os.walk(path):
        for item in files:
            if s in item:
                print os.path.join(root,item)
search('test')
'''
'''
#我们把变量从内存中变成可存储或传输的过程称之为序列化，在Python中叫pickling
反过来，把变量内容从序列化的对象重新读到内存里称之为反序列化，即unpickling。
Python提供两个模块来实现序列化：cPickle和pickle。这两个模块功能是一样的，区别在于cPickle是C语言写的，速度快，pickle是纯Python写的，速度慢
try:
    import cPickle as pickle
except ImportError:
    import pickle
#>>> cPickle.dumps(a)
#pickle.dumps()#方法把任意对象序列化成一个str

with open('1.txt','wb') as f:
    d = dict(name='panda',age=21,score=80)
    pickle.dump(d,f)

with open('1.txt','rb') as f2:
    print pickle.load(f2)
#如果我们要在不同的编程语言之间传递对象，就必须把对象序列化为标准格式，比如XML，但更好的方法是序列化为JSON
#JSON不仅是标准格式，并且比XML更快，而且可以直接在Web页面中读取，非常方便,JSON表示的对象就是标准的JavaScript语言的对象
JSON类型	Python类型
{}	         dict
[]	         list
"string"	'str'或u'unicode'
1234.56	    int或float
true/false	True/False
null	     None

>>> import json
>>> d=dict(name='panda',age=21)
>>> json.dumps(d)
'{"age": 21, "name": "panda"}'
#dumps()方法返回一个str，内容就是标准的JSON
#要把JSON反序列化为Python对象，用loads()或者对应的load()方法，前者把JSON的字符串反序列化，后者从file-like Object中读取字符串并反序列化：
>>> a = '{"name":"panda","age":21}'
>>> import json
>>> json.loads(a)
{'age': 21, 'name': 'panda'}
#有一点需要注意，就是反序列化得到的所有字符串对象默认都是unicode而不是str。由于JSON标准规定JSON编码是UTF-8，所以我们总是能正确地在Python的str或unicode与JSON的字符串之间转换。
#Python的dict对象可以直接序列化为JSON的{}，
#不过，很多时候，我们更喜欢用class表示对象，比如定义Student类，然后序列化：但是必须先设置个函数去转换class 对象为 dict类型才行

import json

class Student(object):
    def __init__(self,name,age,score):
        self.name = name
        self.age = age
        self.score = score
        print 'name: %s, age: %s, score: %s' % (self.name,self.age,self.score)
s = Student('panda',21,100)
print json.dumps(s)
#错误的原因是Student对象不是一个可序列化为JSON的对象。
#前面的代码之所以无法把Student类实例序列化为JSON，是因为默认情况下，dumps()方法不知道如何将Student实例变为一个JSON的{}对象
#可选参数default就是把任意一个对象变成一个可序列为JSON的对象,我们只需要为Student专门写一个转换函数，再把函数传进去即可：

import json

class Student(object):
    def __init__(self,name,age,score):
        self.name = name
        self.age = age
        self.score = score
        print 'name: %s, age: %s, score: %s' % (self.name,self.age,self.score)
def Student2dict(self):
        return {
            'name':self.name,
            'age':self.age,
            'score':self.score
        }
s = Student('panda',21,100)
print json.dumps(s,default=Student2dict)
#不过，下次如果遇到一个Teacher类的实例，照样无法序列化为JSON。我们可以偷个懒，把任意class的实例变为dict：
#因为通常class的实例都有一个__dict__属性，它就是一个dict，用来存储实例变量
>>> import json
>>> s = '{"name":"panda","age":21}'
>>> print(json.dumps(s,default=lambda obj: obj.__dict__))
"{\"name\":\"panda\",\"age\":21}"

#同样的道理，如果我们要把JSON反序列化为一个Student对象实例，loads()方法首先转换出一个dict对象，然后，我们传入的object_hook函数负责把dict转换为Student实例：
import json
class Student(object):
    def __init__(self,name,age,score):
        self.name = name
        self.age = age
        self.score = score
        print 'name: %s, age: %s, score: %s' % (self.name,self.age,self.score)
def dict2student(d):
    return Student(d['name'],d['age'],d['score'])
json_str = '{"age":21,"name":"panda","score":88}'
print json.loads(json_str,object_hook=dict2student)#打印出的是反序列化的Student实例对象
#Python语言特定的序列化模块是pickle，但如果要把序列化搞得更通用、更符合Web标准，就可以使用json模块。
#json模块的dumps()和loads()函数是定义得非常好的接口的典范
'''
'''
#fork()调用一次，返回两次，因为操作系统自动把当前进程（称为父进程）复制了一份（称为子进程），然后，分别在父进程和子进程内返回。
子进程永远返回0，而父进程返回子进程的ID。这样做的理由是，一个父进程可以fork出很多子进程，所以，父进程要记下每个子进程的ID，而子进程只需要调用getppid()就可以拿到父进程的ID
#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os#linux 下才行
print 'Process (%s) start...' % os.getpid()
pid = os.fork()
if pid == 0:
        print 'I am child process (%s) and my parent is %s ' % (os.getpid(),os.getppid())
else:
        print 'I (%s) just created a child process (%s) ' % (os.getpid(),pid)
print 'done'
#root@kali:~/Desktop# python test.py
#Process (28274) start...
#I (28274) just created a child process (28275)
#done
#root@kali:~/Desktop# I am child process (28275) and my parent is 1
#done
#由于Python是跨平台的，自然也应该提供一个跨平台的多进程支持。multiprocessing模块就是跨平台版本的多进程模块。
#multiprocessing模块提供了一个Process类来代表一个进程对象

from multiprocessing import Process
import os
# 子进程要执行的代码
def run_proc(name):
    print 'Run child process %s (%s)...' % (name, os.getpid())

if __name__=='__main__':
    print 'Parent process %s.' % os.getpid()
    p = Process(target=run_proc, args=('test',))
    print 'Process will start.'
    p.start()#创建子进程时，只需要传入一个执行函数和函数的参数，创建一个Process实例，用start()方法启动，这样创建进程比fork()还要简单。
    p.join()#join()方法可以等待子进程结束后再继续往下运行，通常用于进程间的同步。
    print 'Process end.'
# process 31624.
#Process will start.
#Run child process test (31625)...
#Process end.

#!/usr/bin/env python
# -*- coding: utf-8 -*-
#如果要启动大量的子进程，可以用进程池的方式批量创建子进程：
from multiprocessing import Pool
import os, time, random
#对Pool对象调用join()方法会等待所有子进程执行完毕，调用join()之前必须先调用close()，调用close()之后就不能继续添加新的Process了
def long_time_task(name):
    print 'Run task %s (%s)...' % (name, os.getpid())
    start = time.time()
    time.sleep(random.random() * 3)
    end = time.time()
    print 'Task %s runs %0.2f seconds.' % (name, (end - start))

if __name__=='__main__':
    print 'Parent process %s.' % os.getpid()
    p = Pool()
    for i in range(5):
        p.apply_async(long_time_task, args=(i,))
    print 'Waiting for all subprocesses done...'
    p.close()
    p.join()
    print 'All subprocesses done.'
Parent process 6408.
Run task 0 (7076)...
Waiting for all subprocesses done...
Run task 1 (5696)...
Run task 2 (6068)...
Run task 3 (4524)...
Task 1 runs 1.55 seconds.
Run task 4 (5696)...
Task 2 runs 1.65 seconds.
Task 3 runs 2.18 seconds.
Task 0 runs 2.50 seconds.
Task 4 runs 2.37 seconds.
#而task 4要等待前面某个task完成后才执行，这是因为Pool的默认大小在我的电脑上是4，因此，最多同时执行4个进程。这是Pool有意设计的限制
#p = Pool(5) 就可以执行5个进程了
All subprocesses done.
'''
'''
namedtuple
我们知道tuple可以表示不变集合
定义一个class又小题大做了，这时，namedtuple就派上了用场
namedtuple是一个函数，它用来创建一个自定义的tuple对象，并且规定了tuple元素的个数，并可以用属性而不是索引来引用tuple的某个元素。
>>> from collections import namedtuple
>>> point = namedtuple('Point',['x','y'])
>>> p = point(1,2)
>>> p.x,p.y
(1, 2)
>>> isinstance(p,point)
True
>>> isinstance(p,tuple)
True
这样一来，我们用namedtuple可以很方便地定义一种数据类型，它具备tuple的不变性，又可以根据属性来引用，使用十分方便。

deque
使用list存储数据时，按索引访问元素很快，但是插入和删除元素就很慢了，因为list是线性存储，数据量大的时候，插入和删除效率很低。
deque是为了高效实现插入和删除操作的双向列表，适合用于队列和栈：
>>> from collections import deque
>>> a = deque(['a','b','c','d'])
>>> a.append('x')
>>> a
deque(['a', 'b', 'c', 'd', 'x'])
>>> a.appendleft('y')  #另外还有 pop()  popleft()
>>> a
deque(['y', 'a', 'b', 'c', 'd', 'x'])
>>>

#使用dict时，如果引用的Key不存在，就会抛出KeyError。如果希望key不存在时，返回一个默认值，就可以用defaultdict：
>>> from collections import defaultdict
>>> d = defaultdict(lambda: 'N/A')   #设置如果抛出KetError 时 出错信息
>>> d['key2']
'N/A'

使用dict时，Key是无序的。在对dict做迭代时，我们无法确定Key的顺序。
如果要保持Key的顺序，可以用OrderedDict：
>>> od = OrderedDict([('a', 1), ('b', 2), ('c', 3)])
>>> od # OrderedDict的Key是有序的
OrderedDict([('a', 1), ('b', 2), ('c', 3)])

Counter是一个简单的计数器，例如，统计字符出现的个数：
>>> for ch in 'panda is a good man':
...     c[ch] = c[ch] + 1
>>> c
Counter({'a': 4, ' ': 4, 'd': 2, 'o': 2, 'n': 2, 'g': 1, 'i': 1, 'm': 1, 'p': 1, 's': 1})
'''
'''
如果要让记事本这样的文本处理软件能处理二进制数据，就需要一个二进制到字符串的转换方法。Base64是一种最常见的二进制编码方法
由于标准的Base64编码后可能出现字符+和/，在URL中就不能直接作为参数，所以又有一种"url safe"的base64编码，其实就是把字符+和/分别变成-和_：
>>> base64.b64encode('panda')
'cGFuZGE='
>>> base64.b64decode('cGFuZGE==')
'panda'
>>> base64.urlsafe_b64encode('i\xb7\x1d\xfb\xef\xff')
'abcd--__'
>>> base64.urlsafe_b64decode('abcd--__')
'i\xb7\x1d\xfb\xef\xff'
因为Base64是把3个字节变为4个字节，所以，Base64编码的长度永远是4的倍数

>>> import struct
>>> struct.pack('>I',65536)#四字节无符号整数
'\x00\x01\x00\x00'

>>> struct.unpack('>IH','\x00\x40\x10\x00\x00\x00') #>I 4字节无符号整数  H 2字节无符号整数
(4198400, 0)
查询 Format Character
      =: native order, std. size & alignment
      <: little-endian, std. size & alignment
      >: big-endian, std. size & alignment
      !: same as >
地址： 0x401000  \x00\x40\x10\x00  转化用大尾

分析 BMP格式采用小端方式存储数据
>>> with open('1.bmp','rb') as f:
...     a = f.readline()
>>> b =a[:30]
>>> import struct
>>> struct.unpack('<ccIIIIIIHH',b)
('B', 'M', 90678, 0, 54, 40, 256, 118, 1, 24)

import struct
with open('1.bmp','rb') as f:
    a = f.read()
b = a[:30]#失误！！！！！！！！！！！！！！ a = f.read(30)
B_ ,M_,size_,default_1,default_2,default_3,weitu1,weitu2,default_4,color_ = struct.unpack('<ccIIIIIIHH',b)#这里也可以用数组指针访问元素
if B_ == 'B':
    if M_ == 'M':
        print 'size  = %s \ncolor = %s' % (size_,color_)

>>> import hashlib
>>> md5 = hashlib.md5()
>>> md5.update('how to use md5 in system by panda')
>>> print md5.hexdigest()
08712b57371b8b261790adbf0adb6fa3
import hashlib

sha1 = hashlib.sha1()
sha1.update('how to use sha1 in ')
sha1.update('python hashlib?')
print sha1.hexdigest()
由于常用口令的MD5值很容易被计算出来，所以，要确保存储的用户口令不是那些已经被计算出来的常用口令的MD5，这一方法通过对原始口令加一个复杂字符串来实现，俗称“加盐”：
def calc_md5(password):
    return get_md5(password + 'the-Salt')
只要Salt不被黑客知道，即使用户输入简单口令，也很难通过MD5反推明文口令
如果假定用户无法修改登录名，就可以通过把登录名作为Salt的一部分来计算MD5，从而实现相同口令的用户也存储不同的MD5。
'''
from xml.parsers.expat import  ParserCreate

class DefaultSaxHandler(object):
    def start_element(self, name, attrs):
        print('sax:start_element: %s, attrs: %s' % (name, str(attrs)))

    def end_element(self, name):
        print('sax:end_element: %s' % name)

    def char_data(self, text):
        print('sax:char_data: %s' % text)

xml = r'''<rss xmlns:yweather="http://xml.weather.yahoo.com/ns/rss/1.0" xmlns:geo="http://www.w3.org/2003/01/geo/wgs84_pos#" version="2.0">
<channel>
<title>Yahoo! Weather - Zhuhai, CN</title>
<link>
http://us.rd.yahoo.com/dailynews/rss/weather/Zhuhai__CN/*http://weather.yahoo.com/forecast/CHXX0512_c.html
</link>
<description>Yahoo! Weather for Zhuhai, CN</description>
<language>en-us</language>
<lastBuildDate>Mon, 27 Oct 2014 10:30 am CST</lastBuildDate>
<ttl>60</ttl>
<yweather:location city="Zhuhai" region="" country="China"/>
<yweather:units temperature="C" distance="km" pressure="mb" speed="km/h"/>
<yweather:wind chill="30" direction="50" speed="14.48"/>
<yweather:atmosphere humidity="58" visibility="8" pressure="1015.92" rising="0"/>
<yweather:astronomy sunrise="6:25 am" sunset="5:49 pm"/>
<image>
<title>Yahoo! Weather</title>
<width>142</width>
<height>18</height>
<link>http://weather.yahoo.com</link>
<url>
http://l.yimg.com/a/i/brand/purplelogo//uh/us/news-wea.gif
</url>
</image>
<item>
<title>Conditions for Zhuhai, CN at 10:30 am CST</title>
<geo:lat>22.28</geo:lat>
<geo:long>113.56</geo:long>
<link>
http://us.rd.yahoo.com/dailynews/rss/weather/Zhuhai__CN/*http://weather.yahoo.com/forecast/CHXX0512_c.html
</link>
<pubDate>Mon, 27 Oct 2014 10:30 am CST</pubDate>
<yweather:condition text="Partly Cloudy" code="30" temp="30" date="Mon, 27 Oct 2014 10:30 am CST"/>
<description>
<![CDATA[
<img src="http://l.yimg.com/a/i/us/we/52/30.gif"/><br /> <b>Current Conditions:</b><br /> Partly Cloudy, 30 C<BR /> <BR /><b>Forecast:</b><BR /> Mon - Partly Cloudy. High: 28 Low: 24<br /> Tue - Partly Cloudy/Wind. High: 27 Low: 24<br /> Wed - Partly Cloudy. High: 27 Low: 24<br /> Thu - Partly Cloudy. High: 27 Low: 24<br /> Fri - Partly Cloudy. High: 27 Low: 24<br /> <br /> <a href="http://us.rd.yahoo.com/dailynews/rss/weather/Zhuhai__CN/*http://weather.yahoo.com/forecast/CHXX0512_c.html">Full Forecast at Yahoo! Weather</a><BR/><BR/> (provided by <a href="http://www.weather.com" >The Weather Channel</a>)<br/>
]]>
</description>
<yweather:forecast day="Mon" date="27 Oct 2014" low="24" high="28" text="Partly Cloudy" code="30"/>
<yweather:forecast day="Tue" date="28 Oct 2014" low="24" high="27" text="Partly Cloudy/Wind" code="24"/>
<yweather:forecast day="Wed" date="29 Oct 2014" low="24" high="27" text="Partly Cloudy" code="30"/>
<yweather:forecast day="Thu" date="30 Oct 2014" low="24" high="27" text="Partly Cloudy" code="30"/>
<yweather:forecast day="Fri" date="31 Oct 2014" low="24" high="27" text="Partly Cloudy" code="30"/>
<guid isPermaLink="false">CHXX0512_2014_10_31_7_00_CST</guid>
</item>
</channel>
</rss>

handler = DefaultSaxHandler()
parser = ParserCreate()
parser.returns_unicode = True
parser.StartElementHandler = handler.start_element
parser.EndElementHandler = handler.end_element
parser.CharacterDataHandler = handler.char_data
a = parser.Parse(xml)
#http://weather.yahooapis.com/forecastrss?u=c&w=2161856  查询天气
#https://weather.yahoo.com/china/guangdong/zhuhai-2161856/

如果我们要编写一个搜索引擎，第一步是用爬虫把目标网站的页面抓下来，第二步就是解析该HTML页面，看看里面的内容到底是新闻、图片还是视频。
HTML本质上是XML的子集，但是HTML的语法没有XML那么严格，所以不能用标准的DOM或SAX来解析HTML。

from HTMLParser import HTMLParser
from htmlentitydefs import name2codepoint

class MyHTMLParser(HTMLParser):
    def handle_starttag(self, tag, attrs):
        print('<%s>' % tag)
    def handle_endtag(self, tag):
        print('</%s>' % tag)
    def handle_startendtag(self, tag, attrs):
        print('<%s/>' % tag)
    def handle_data(self, data):
        print('data')
    def handle_comment(self, data):
        print('<!-- -->')
    def handle_entityref(self, name):
        print('&%s;' % name)
    def handle_charref(self, name):
        print('&#%s;' % name)
parser = MyHTMLParser()
parser.feed(r'''''')
'''
'''
基本上，所有的第三方模块都会在PyPI - the Python Package Index上注册，只要找到对应的模块名字，即可用easy_install或者pip安装。

import socket# Client

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect(('www.sina.com.cn',80))
s.send('GET / HTTP/1.1\r\nHost: www.sina.com.cn\r\nConnection: close\r\n\r\n')
buffer = []
while True:
    d = s.recv(1024)
    if d:
        buffer.append(d)
    else:
        break
data = ''.join(buffer)
print data.decode('gbk')
header,html = data.split('\r\n\r\n',1)
#print header
with open('sina.html','wb') as f:
    f.write(html)
s.close()

import socket #Server
import threading
import time
def tcplink(sock,addr):
    print 'Accespt new connection from %s:%s' % addr
    sock.send('Welcome!')
    while True:
        data = sock.recv(1024)
        time.sleep(1)
        if data == 'exit' or not data:
            break
        sock.send('Hello,%s' % data)
    sock.close()
    print 'Connection from %s:%s closed' % addr

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind(('127.0.0.1',9999))
s.listen(5)
print 'Waiting for connection'
while True:
    sock, addr = s.accept()
    t = threading.Thread(target=tcplink,args=(sock,addr))
    t.start()
'''
'''
SMTP是发送邮件的协议，Python内置对SMTP的支持，可以发送纯文本邮件、HTML邮件以及带附件的邮件
SMTP（Simple Mail Transfer Protocol）即简单邮件传输协议,它是一组用于由源地址到目的地址传送邮件的规则，由它来控制信件的中转方式
Python对SMTP支持有smtplib和email两个模块，email负责构造邮件，smtplib负责发送邮件。

from email.header import Header
from email import encoders
from email.mime.text import MIMEText
from email.utils import parseaddr,formataddr
import smtplib

def _format_addr(s):
    name, addr = parseaddr(s)
    return formataddr((  Header(name, 'utf-8').encode(), addr.encode('utf-8') if isinstance(addr, unicode) else addr))


from_addr = '18508280792@163.com'#email addr passwd
password = '82282762'

smtp_server = 'smtp.163.com'#smtp server addr 你得填163提供的SMTP服务器地址：smtp.163.com
to_addr = '694918610@qq.com'

msg = MIMEText('hello,send by Python','plain','utf-8')
#注意到构造MIMEText对象时，第一个参数就是邮件正文，第二个参数是MIME的subtype，传入'plain'，最终的MIME就是'text/plain'，最后一定要用utf-8编码保证多语言兼容性
#如果我们要发送HTML邮件,把HTML字符串传进去,再把第二个参数由plain变为html就可以了
msg['From'] = _format_addr(u'Python 爱好者 <%s>' % from_addr)
msg['To']  = _format_addr(u'管理员 <%s>' % to_addr)#msg['To']接收的是字符串而不是list，如果有多个邮件地址，用,分隔即可
msg['Subject'] = Header(u'来自SMTP 的问候~~~~','utf-8').encode()


server = smtplib.SMTP(smtp_server,25)#smtp port is 25
server.set_debuglevel(1)#我们用set_debuglevel(1)就可以打印出和SMTP服务器交互的所有信息
server.login(from_addr,password)
server.sendmail(from_addr,[to_addr],msg.as_string())
server.quit()

#发送邮件用 SMTP     收邮件用 POP3

zh = u"中国"

with open('d:/1.txt','w') as f:
    f.write(zh.encode('utf-8'))
with open("d:/1.txt",'r') as f2:
    b = f2.read()
    print 'no decode :%s' % b
    print 'decode :%s' % b.decode('utf-8')
'''
import re







