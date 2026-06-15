from source.jsonreader import jsonreader as jsr
from pathlib import Path

levelspath = "../../Content/data/lovika/levels/"
def get_level_file(filename):
    return levelspath+filename

def main():
    print("This script updates level .jsons in the hard coded path with labels and dumps those labels in a .csv in the ./out folder")
    print("Running")
    reader = jsr()

    filenames = Path(levelspath).glob("*")

    # disable SoggySwamp for now (bad .json) ##### and x.name != "SoggySwamp.json"
    files = [x for x in filenames if x.is_file() ]

    for file in files:
        reader.load(get_level_file(file.name))

    # #### debug tools ####
    # reader.load(levelspath+"creeperwoods.json")
    # reader.writefile_replace(levelspath+"creeperwoods.json")
    print("Done")


if __name__ == "__main__":
    main()