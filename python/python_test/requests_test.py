#!/usr/bin/env python
#  -*- coding: utf-8 -*-
__author__ = 'panda'

'''
>>>import requests
>>> r = requests.get('http://www.zhidaow.com')  # 发送请求
>>> r.status_code  # 返回码
200
>>> r.headers['content-type']  # 返回头部信息
'text/html; charset=utf8'
>>> r.encoding  # 编码信息
'utf-8'
>>> r.text  #内容部分（PS，由于编码问题，建议这里使用r.content）
u'<!DOCTYPE html>\n<html xmlns="http://www.w3.org/1999/xhtml"...'

import requests

payload={'wd':'向昌权','rn':'100'}
#r = requests.get('http://www.baidu.com',params=payload)
#print r.url
#print r.text
#print r.content#以字节的方式去显示
#print r.encoding
r = requests.get('http://ip.taobao.com/service/getIpInfo.php?ip=122.88.60.28')
print r.json()['data']['country']
print r.history

>>>r = requests.get('http://www.baidu.com/link?url=QeTRFOS7TuUQRppa0wlTJJr6FfIYI1DJprJukx4Qy0XnsDO_s9baoO8u1wvjxgqN', allow_redirects = False)
>>>r.status_code
302#只要加上一个参数allow_redirects，禁止了跳转，就直接出现跳转的状态码了

requests.get('http://github.com', timeout=0.001)设置超时时间

import requests代理访问

proxies = {
  "http": "http://10.10.1.10:3128",
  "https": "http://10.10.1.10:1080",
}
requests.get("http://www.zhidaow.com", proxies=proxies)
如果代理需要账户和密码，则需这样：
proxies = {
    "http": "http://user:pass@10.10.1.10:3128/",
}

请求头内容可以用r.request.headers来获取。
>>> r.request.headers
{'Accept-Encoding': 'identity, deflate, compress, gzip',
'Accept': '*/*', 'User-Agent': 'python-requests/1.2.3 CPython/2.7.3 Windows/XP'}

r = requests.get('http://www.zhidaow.com')#自定义请求头部
print r.request.headers['User-Agent']
#python-requests/1.2.3 CPython/2.7.3 Windows/XP

headers = {'User-Agent': 'alexkh'}
r = requests.get('http://www.zhidaow.com', headers = headers)
print r.request.headers['User-Agent']
#alexkh

import requests

def get_status(url):
    r = requests.get(url,allow_redirects=False)
    return r.status_code

print get_status('http://www.baidu.com')
'''