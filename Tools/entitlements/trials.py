# modify Trials
# ---------------
import argparse
# this script is based on the wiki-instructions on the Dungeons wiki

# from source.EntitlementsEditor import *
from source.TrialsEditor import *
from source.helpers import *
from os import remove

def get_username():
	return input("Type the (staging) admin username [leave blank for '{}']: ".format(EntitlementsEditor.staging_username))

def get_password():
	return input("Type the (staging) password [leave blank for '{}']: ".format(EntitlementsEditor.staging_password))



def main():
	
	parser = argparse.ArgumentParser(description="list and modify MojangServices user's entitlements. The default behaviour add user entitlements through a prompt."
													"Note: requires admin account credentials (prompted at runtime)")
	
	parser.add_argument("-p", "--path-build", default="", help="Override path to a build for user-id fetch")
	parser.add_argument("-cp", "--cache-path", action="store_true", help="cache the path the build for user-id fetch")
	parser.add_argument("-cl", "--clear-cached-path", action="store_true", help="clear path, revert to path-build (if defined) or project dir")
	parser.add_argument("-g", "--generate", action="store_true", help="generate new trials (overwrite)")
	parser.add_argument("-s", "--show", action="store_true", help="show current trials JSON")
	parser.add_argument("-rm", "--remove", default="", help="remove desired entitlement (TODO: support removing a list of them).")

	args = parser.parse_args()

	path_build = args.path_build

	editor = TrialsEditor(path_build)

	if args.show:
		editor.ShowTrials()
	elif args.generate:
		editor.GenerateTrials()


	print("OK go")


if __name__=="__main__":
	main()
	print("finished 'trials.py'.")
