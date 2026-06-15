import json
from pathlib import Path
from source.multiline_comments import *
import shutil
import re

class jsonreader:
    def __init__(self, filename=""):
        if filename != "":
            self.filename = filename
        # members
        self.new_pairs = {}
        self.label_pairs = {}

    def is_key(self, name):
        import re
        matchstring = r'[\s]'
        return len(re.findall(matchstring, name)) == 0

    def string_to_key(self, key, prefix):
        key = key.replace("'", "").strip(". ") # remove trailing ...'s and spaces
        return "_".join([prefix] + key.split()).lower()

    def replace_in_json(self, pairs, json_data):
        for k, v in pairs.items():
            json_data = json_data.replace(v,k)

        matches = re.finditer(r'[\s\t]*("(name|description)":.+)\n', json_data)
        print("Replace candidates in json-file:")
        for m in matches:
            print(m.group(0))
        return json_data

            
    def update_target_csv(self, sourcefile):
        game_csvfile="../../Content/Decor/Text/" + sourcefile.split("/")[-1]
        shutil.copyfile(sourcefile, game_csvfile)

    def name_from_key(self, key):
        return " ".join(key.title().split("_")[1:])

    def check_and_update_value(self, key):
        # if not already in csv -add a generated value!
        if not key in self.label_pairs:
            val = self.name_from_key(key)
            self.label_pairs[key]= val
            self.new_pairs[key]=val

    def loadfile(self, filename, mode="r+"):
        p = Path(filename)
        if not p.exists:
            print("no such file {}, creating it!".format(filename))
            
            f = open(filename, "w")
            # write header if csv:
            if (filename.endswith(".csv")):
                f.write("Key,SourceString\n")

            return f
        return open(filename, mode)

    def load(self, filename=""):
        if (filename!=""):
            self.filename = filename
        
        # reset!
        self.new_pairs = {}
        self.label_pairs = {}
        p = Path(self.filename)
        if not p.exists:
            print("no such file {}".format(self.filename))
            return
        name = p.name.split(".")[0]
        csvfile = "out/{}Labels.csv".format(name)

        f = self.loadfile(self.filename)
        # extra step to handle multiline comments in Json
        try:
            self.data = json.load(f)
        except json.decoder.JSONDecodeError as error:
            print("ERROR Could not decode '{}' ... using multiline comment stripper!".format(filename))
            f.seek(0)
            f_stripped = remove_comments(f.read())
            
            self.data = json.loads(f_stripped)
        data = self.data

        csv = self.loadfile(csvfile)
        for line in csv:
            tokens=line.split(",")
            if tokens[0] == "Key":
                continue
            self.label_pairs[tokens[0]]=tokens[1]

        for line in data:
            if line == "objectives":
                objectives = data["objectives"]
                for o in objectives:
                    # if name matching... 
                    for l in ["name", "description"]:
                        if self.is_key(o[l]):
                            self.check_and_update_value(o[l])
                            continue
                        k = self.string_to_key(o[l], l)
                        if not k in self.new_pairs:
                            self.new_pairs[k] = o[l]
                            self.label_pairs[k] = o[l]
                            o[l] = k
        
        #write all new key-value-pairs to interim csv:
        for k,v in self.new_pairs.items():
            csv.write("{},{}\n".format(k, v))
        csv.close()
        
        #write back edits to .json:
        f.seek(0)
        datastring = f.read()
        
        f.seek(0)
        datastring = self.replace_in_json(self.new_pairs, datastring)
        f.write(datastring)
        # json.dump(data, f, indent=4)
        # NOTE: This reformats the .json -- instead do a file-as-string-replace
        # and write back!!

        f.truncate()
        f.close()

        #write back new csv entries to actual game file:
        self.update_target_csv(csvfile)

        print("updated {}".format(filename))