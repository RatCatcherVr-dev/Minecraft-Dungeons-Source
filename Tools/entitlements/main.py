# modifyEntitlements.py
import argparse
# this script is based on the wiki-instructions on the Dungeons wiki

from source.EntitlementsEditor import *
from source.helpers import *
from os import remove

def get_username():
	return input("Type the (staging) admin username [leave blank for '{}']: ".format(EntitlementsEditor.staging_username))

def get_password():
	return input("Type the (staging) password [leave blank for '{}']: ".format(EntitlementsEditor.staging_password))

def get_commaseparated_entitlements(action = "add"):
	print(EntitlementsEditor)
	desired = input("Type in comma-separated list of entitlements to {} ('all' for all)".format(action))
	if desired=='all':
		return all_entitlements.keys()
	return desired.split(",")

def main():
	
	parser = argparse.ArgumentParser(description="list and modify MojangServices user's entitlements. The default behaviour add user entitlements through a prompt."
													"Note: requires admin account credentials (prompted at runtime)")
	
	parser.add_argument("-p", "--path-build", default="", help="Override path to a build for user-id fetch")
	parser.add_argument("-cp", "--cache-path", action="store_true", help="cache the path the build for user-id fetch")
	parser.add_argument("-cl", "--clear-cached-path", action="store_true", help="clear path, revert to path-build (if defined) or project dir")
	parser.add_argument("-l", "--list", action="store_true", help="list your user's entitlements")
	parser.add_argument("-a", "--all", action="store_true", help="list available entitlements")
	parser.add_argument("-rm", "--remove", default="", help="remove desired entitlement (TODO: support removing a list of them).")
	parser.add_argument("-pid", "--player-id", default=None, help="specify (custom) player-id, bypassing logfile search")
	# todo: add file with Pid to not have to keep entering it on the console!
# fa098028-b6a9-418f-9d7f-34acf81d52b1
	args = parser.parse_args()

	path_build = args.path_build
	path_cached = "tmp/patch_cached.txt"
	if args.clear_cached_path:
		remove(path_cached)

	if args.cache_path and args.path_build:
		# write path:
		ensure_path(path_cached)
		with open(path_cached, "w+") as F:
			F.write(path_build)
	
	if not args.path_build:
		if Path(path_cached).is_file():
			print("Using cached path for user-id, to clear, use -p=[PATH] flag and/or delete %s" % path_cached)
			with open(path_cached, "r", encoding='utf-8') as F:
				path_build = F.read()
		else: # fall back to project dir
			path_build = GetProjectDir()

	E = EntitlementsEditor(path_build, args.player_id)
	if not E.has_valid_token():
		E.sign_in(get_username(), get_password())

	# list + early out if '-a':
	if args.all:
		print(E.backend_products())
		# print(E)
		return
	
	E.list_entitlements()

	if args.list:
		return
	if args.remove:
		# todo: support more than one to delete at a time!
		products_to_remove = [args.remove] or get_commaseparated_entitlements("REMOVE")
		E.remove_entitlements(products_to_remove)
	else:
		E.add_entitlements(get_commaseparated_entitlements())

	E.list_entitlements()		
	
if __name__=="__main__":
	main()
	print("finished 'modifyEntitlements.py'.")
