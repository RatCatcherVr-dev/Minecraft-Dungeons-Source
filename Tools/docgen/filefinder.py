#!/bin/python3/python3
import os
import re
from pathlib import Path

### todo: allow different relative path==can move file
def dungeons_root():
    dungeonsproj = "../Dungeons.uproject"
    pfile = Path(dungeonsproj)

    if pfile.is_file(): return str(pfile.parent)
    dungeonsproj = "../"+dungeonsproj
    pfile = Path(dungeonsproj)

    if pfile.is_file(): return str(pfile.parent)
    else: raise("Couldn't find Dungeons HOME")

def print_subtree(root):
    rootDir = root
    isConsoleCommands = re.compile(r'^.+ConsoleCommands.cpp')

    for dirName, subdirList, fileList in os.walk(rootDir):
        # print('Found directory: %s' % dirName)
        subdir=dirName.split(rootDir)[1]
        print('dir: %s' % subdir)
        for fname in fileList:
            if isConsoleCommands.match(fname):
                print('\t%s\%s' % (subdir, fname))

def find_files(rootDir, regexString):
    rootDir=dungeons_root()+rootDir
    relpath=dungeons_root()
    retv=[]
    if (type(regexString)==type(re)):
        regex = regexString
    else:
        regex = re.compile(regexString)
    for dirName, subdirList, fileList in os.walk(rootDir):
        # print('Found directory: %s' % dirName)
        subdir=dirName.split(relpath)[1]
        # print('searching... %s' % subdir)
        for fname in fileList:
            if regex.match(fname):
                path_match='{}/{}'.format(subdir.rstrip("/\\"),fname)
                retv.append(path_match)
                print('\t%s'%path_match)
    return retv
    
# print("check file tree")
# print_subtree(os.getcwd())
