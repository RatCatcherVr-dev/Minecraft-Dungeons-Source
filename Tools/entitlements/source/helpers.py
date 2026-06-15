import re
import os
from pathlib import Path

def GetProjectDir():
	# search for Dungeons.uproject to find root
	s = "."
	for i in range(0,3):
		dungeonsfile = Path(s+"/Dungeons.uproject")
		if dungeonsfile.is_file():
			return s
		else:
			s = "../"+s

	# fall back to getting from env
	return os.environ.get("DUNGEONS_PATH")

def by_date(f):
	return f.stat().st_mtime

def get_dungeons_logfiles(base_path):
	candidates = []
	logs_path=""
	if (Path(base_path) / "WindowsNoEditor").is_dir(): base_path = Path(base_path) / "WindowsNoEditor"
	if (Path(base_path) / "Dungeons.exe").is_file(): #packed build:
		logs_path = Path(base_path) / "Dungeons" / "Saved" / "Logs" 
	else:# dev root
		logs_path = Path(base_path) / "Saved" / "Logs" 

	if (logs_path.is_dir()):
		candidates = [log for log in logs_path.rglob("*.log") if "Dungeons" in log.name]
		candidates.sort(key=by_date, reverse=False)

	if len(candidates) == 0:
		raise Exception("Found no log files in '%s' \nDid you point to a Dungeons build/install that creates logs?" % logs)
	return candidates

def parse_userid(logfiles):
	# todo: add check for cahe/file/input arg
	
	for logfile in logfiles:
		# try:		
		print("Reading user id from %s" % logfile)
		with open(logfile, 'r', encoding='utf-8') as F:
			data = F.read()
			hits = re.search(r':\s*Authenticated against Minecraft API as user ([a-f0-9\-]+).', data)
			if (hits):
				print("found player-id '{}'".format(hits.groups()[0]))
				return hits.groups()[0]

			authfail = re.search(r'Failed to authenticate towards Minecraft services API', data)
			if (authfail):
				print("Found AUTH warning in log: You can try to re-trigger using 'Dungeons.MinecraftAPI.Authenticate'")
			backendfail = re.search(r'Unable to get client for authenticating towards Minecraft services API', data)
			if (backendfail):
				print("DungeonsAPIClient module is unavailable, cannot authenticate with Minecraft API")

	print("No match for user-id string in log.\nIf you are trying to use staging env over VPN, you will have to RE-START the game ON VPN to write this log file.")
	raise Exception("unable to extract user id, quitting!")

def get_userid(base_path):
	if not base_path: base_path=GetProjectDir()
	return parse_userid(get_dungeons_logfiles(base_path))

def ensure_path(path):
	location = Path(path)
	if location.suffix: location = location.parent
	if not location.is_dir(): os.makedirs(location)
