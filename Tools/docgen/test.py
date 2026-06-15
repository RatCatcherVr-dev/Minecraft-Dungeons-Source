#!/bin/python3/python3
import os
import re
from filefinder import *

sources=[]
rematch=r'^.+ConsoleCommands.cpp'
sources=find_files("/Source/Dungeons/game/actor/character/", rematch)
sources.append(find_files("/Source/Dungeons/game/item/", rematch))
sources.append(find_files("/Source/Dungeons/online/", rematch))
