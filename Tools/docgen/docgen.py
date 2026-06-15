# # list commands help from source# output all help from command files
# todo: search project directory for files matching wildcard. for now: List common files
import os
import re
from pathlib import Path
from filefinder import *

sources=[]
rematch=r'^.+ConsoleCommands.cpp'
sources=find_files("/Source/Dungeons/game/actor/character/", rematch)
sources.extend(find_files("/Source/Dungeons/game/item/", rematch))
sources.extend(find_files("/Source/Dungeons/online/", rematch))
sources.extend(find_files("/Source/Dungeons/game/", rematch))

## global compiled regex for faster use+overview
consolecommandRE=re.compile(r'.*[static|const].*\ FAutoConsoleCommand (.+)\(\s*TEXT\([\"\'](.+)[\"\']\).*')
linebrokenCommandRE=re.compile(r'.*static .*FAutoConsoleCommand (.+)\(')
TEXTmatchRE = re.compile(r'^[\s,]*TEXT\([\"\']([^\"]+)[\"\'][\w\s]*\)(.*)')
filenameRE=re.compile(r'.*\/(.+\.cpp)$')

## helpers
def get_relative_path():
    dungeonsproj = "../Dungeons.uproject"
    pfile = Path(dungeonsproj)

    if pfile.is_file(): return str(pfile.parent)
    dungeonsproj = "../"+dungeonsproj
    pfile = Path(dungeonsproj)

    if pfile.is_file(): return str(pfile.parent)
    else: raise("Couldn't find Dungeons HOME")

def get_descriptions(line):
    M = TEXTmatchRE.match(line)
    descriptions = []
    while M and len(M.groups()) > 0:
        descriptions.append(M.groups()[0])
        M = len(M.groups()) > 1 and TEXTmatchRE.match(M.groups()[1]) or None
    
    if len(descriptions) > 0:
        return descriptions
    return None

CleanForMarkDown = re.compile(r'[<>|]{1,2}')
# CleanNewline = re.compile(r'[\\]n')
def md_compatible_chars(matchobj):
    if matchobj.group(0) == '<': return '['
    if matchobj.group(0) == '>': return ']'
    if matchobj.group(0) == '|': return '/'
    if matchobj.group(0) == '\n': return ' '

def clean_str(s):
    cleaned = CleanForMarkDown.sub(md_compatible_chars, s)
    # cleaned = CleanNewline(md_compatible_chars, cleaned)
    return cleaned

class DocGenerator:
    got_block = False
    current_command = ""
    current_desc = []

    def __init__(self, outfilename):
        self.outfilename = outfilename

    def get_command(self, line):
        if self.got_block: # looking for a command on this line
            M = TEXTmatchRE.match(line)
            if not M :
                print ("Warning! Expecting command 1 line after declaration")
                return None
            
            return M.groups()[0]
            
        M = consolecommandRE.match(line)
        if M:
            if len(M.groups()) < 2: 
                print("Warning: regex matcher found too few params, skipping!")
                return None
            self.got_block = True
            return M.groups()[1]

        if not M and linebrokenCommandRE.match(line):
            self.got_block=True

        return None

    def processing_block(self):
        return self.got_block and len(self.current_command) != 0

    def extract_block(self, row):
        desc = get_descriptions(row)
        if not desc:
            # end of this block!
            block = {"name": self.current_command \
                , "desc": ". ".join(self.current_desc)}
            return True, block

        for d in desc: self.current_desc.append(d)
        return False, {}

    def main(self):
        open(self.outfilename, 'w')

        # extract from and output to file
        for s in sources:
        # for s in ["/Source/Dungeons/game/actor/character/player/PlayerConsoleCommands.cpp"]:
            self.process_file(s)

        print("Done -Wrote {0}".format(self.outfilename))

    def process_file(self, sourcefile):
        # read files
        # for s in ["/Source/Dungeons/online/trials/TrialsConsoleCommands.cpp"]:
        filename = get_relative_path()+sourcefile
        commands = list()

        with open(filename, 'r') as sourcefile:

            for row in sourcefile:
                if self.processing_block():
                    finished, block = self.extract_block(row)
                    if (finished):
                        commands.append(block)
                        self.got_block = False
                        self.current_command=""
                        self.current_desc = []

                else:
                    command = self.get_command(row)
                    if (not command): continue

                    self.current_command = command

        with open(self.outfilename, 'a') as out:
            Fmatch = filenameRE.match(filename)
            section = Fmatch and Fmatch.groups()[0] or "Unknown:"
            if (len(commands)>0):
                out.write("\n{0}\n".format(section))
                out.write("| command | effect |\n| --:|:-- |\n")

            if not Path(filename).is_file():
                out.write("ERROR: missing file on disk: {0}".format(filename))
            for item in commands:
                out.write("| {0} | {1} | \n".format(item["name"], clean_str(item["desc"])))



#### program!
dg = DocGenerator("fromsource_desc.md")
dg.main()

#### done

