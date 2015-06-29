"""
python 2.7
author:hacklvwar
date:2012/10/24
use:createshellcode
"""

incode=raw_input("Enter the input filename:")
outcode=raw_input("Enter the output filename:")
infile=open(incode,'r')
outfile=open(outcode,'a+')
flag=True
outlines=''
while flag:
    linecode=infile.readline()
    if linecode:
        outline=linecode[13:35].strip()
        lines=outline.split(' ')
        outline=''.join(lines)
        lines=outline.split(':')
        outline=''.join(lines)
        newline=''
        for c in range(0,len(outline)-1,2):
            newline+='\\x'+outline[c:c+2]
        outlines+=newline
    else:
        flag=False
newlines=''
for c in range(0,len(outlines)-1,64):
    newlines+='\"'+outlines[c:c+64]+'\"\n'
outfile.writelines(newlines)
infile.close()
outfile.close()