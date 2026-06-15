import requests
import re
import json
import os
import source.helpers as helpers
from pathlib import Path
from datetime import datetime

# game_dungeons_beta,
# game_dungeons,
# game_dungeons_dlc_1,
# game_dungeons_dlc_2,
# game_dungeons_dlc_3,
# game_dungeons_dlc_4,
# game_dungeons_dlc_5,
# game_dungeons_dlc_6,
# game_dungeons_hero_cape_1,
# game_dungeons_hero_skin_1,
# game_dungeons_hero_skin_2,
# game_dungeons_hero_pet_1,
# product_dungeons_beta,
# product_dungeons,
# product_dungeons_hero_edition,
# product_dungeons_dlc_1,
# product_dungeons_dlc_2,
# product_dungeons_dlc_3,
# product_dungeons_dlc_4,
# product_dungeons_dlc_5,
# product_dungeons_dlc_6,
# game_minecraft,
# product_minecraft,
# product_dungeons_season_pass;

all_entitlements = {
"dungeons_beta": "Owner has access to Dungeons beta (updates?)",
"dungeons": "The owner has purchased Dungeons (base game)",
"dungeons_hero_edition": "The owner has purchased Hero Edition",
"dungeons_dlc_1": "The owner has purchased DLC 1 (separately from Hero Edition)",
"dungeons_dlc_2": "The owner has purchased DLC 2 (separately from Hero Edition)",
"dungeons_dlc_3": "The owner has purchased DLC 3 (separately from Season Pass)",
"dungeons_dlc_4": "The owner has purchased DLC 4 (separately from Season Pass)",
"dungeons_dlc_5": "The owner has purchased DLC 5 (separately from Season Pass)",
"minecraft": "Owner has access to Minecraft",
"dungeons_season_pass": "The owner has purchased Season Pass"

}

game_entitlements = {
"game_dungeons_beta": "The owner is allowed to participate in the Dungeons beta",
"game_dungeons": "The owner is allowed to play Dungeons",
"game_minecraft" : "The owner is allowed to play (Vanilla) Minecraft",
"game_dungeons_dlc_1": "The owner is allowed to play DLC 1",
"game_dungeons_dlc_2": "The owner is allowed to play DLC 2",
"game_dungeons_dlc_3": "The owner is allowed to play DLC 3",
"game_dungeons_dlc_4": "The owner is allowed to play DLC 4",
"game_dungeons_dlc_5": "The owner is allowed to play DLC 5",
"game_dungeons_hero_cape_1": "The owner is allowed to equip the Hero Edition exclusive cape",
"game_dungeons_hero_skin_1": "The owner is allowed to use the first Hero Edition exclusive skin",
"game_dungeons_hero_skin_2": "The owner is allowed to use the second Hero Edition exclusive skin",
"game_dungeons_hero_pet_1": "The owner is allowed to equip the Hero Edition exclusive pet"

}

future_entitlements = {
"game_dungeons_dlc_6": "The owner is allowed to play DLC 6",
"game_dungeons_season_cape_1": "The owner is allowed to equip the Season Pass exclusive cape",
"game_dungeons_season_skin_1": "The owner is allowed to use the first Season Pass exclusive skin",
"game_dungeons_season_skin_2": "The owner is allowed to use the second Season Pass exclusive skin",
"game_dungeons_season_pet_1": "The owner is allowed to equip the Season Pass exclusive pet",

"dungeons_dlc_6": "The owner has purchased DLC 6 (separately from Season Pass)"
}


class EntitlementsEditor:
	staging_username = "pigeon"
	staging_password = "redtiger"

	_token_cached='tmp/token_cached.json'
	url=r'https://backoffice-staging.minecraftservices.com/backoffice'
	_timeout = 5
	_default_time_format="%a %b %d %H:%M:%S %Y"

	def __init__(self, base_path, user_id = ""):
		print("Creating Entitlements Editor...")
		self._header = {}
		self._UserID = user_id
		self.base_path = base_path
		self._products = None
	
	def sign_in(self, username, password):
		# fall back on the default sign-in for staging
		if not username:
			username = EntitlementsEditor.staging_username
		if not password:
			password = EntitlementsEditor.staging_password

		headers = {
			'Content-Type': 'application/json',
		}
		data = r'{"username": "%s", "password": "%s"}' % (username, password)
		try:
			response = requests.post('{}/login'.format(EntitlementsEditor.url), timeout = EntitlementsEditor._timeout, headers=headers, data=data)
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
			return
		
		helpers.ensure_path(EntitlementsEditor._token_cached)	
		with open(EntitlementsEditor._token_cached, 'w+') as T:
			json_token = json.loads(response.text)
			json_token["timestamp"] = datetime.now().ctime()
			json.dump(json_token, T)

	def has_valid_token(self):
		# // also check if valid!!
		if Path(EntitlementsEditor._token_cached).is_file():
			with open(EntitlementsEditor._token_cached, 'r') as T:
				# check if too old!
				try: 
					data = json.load(T)
				except ValueError:
					return False
				fileTime = datetime.strptime(data["timestamp"], EntitlementsEditor._default_time_format)
				return (datetime.now() - fileTime).seconds < data["expires_in"]
		return False
		# print("implement check for valid token stash")

	def __str__(self):
		out = " [AVAILABLE for add/remove] \n"
		for p in self.backend_products():
			# print("{}: {}".format(k,v))
			out += "{}\n".format(p)
		out += "\n [GAME entitlement toggles] \n"
		for k, v in game_entitlements.items():
			# print("{}: {}".format(k,v))
			out += "{}: {}\n".format(k,v)
		out += "\n [future entitlement keys]\n"
		for k, v in future_entitlements.items():
			out += "{}: {}\n".format(k,v)
			# print("{}: {}".format(k,v))
		out += "\n"
		return out

	def backend_all_entitlements(self):
		try:
			response = requests.get("{}/products".
			format(EntitlementsEditor.url),
			headers=self.headers(), timeout = EntitlementsEditor._timeout)
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
			return
		print("All entitlements")

	def backend_products(self):
		# this is only cached once in a single run..
		if self._products: return self._products

		try:
			response = requests.get("{}/products".
			format(EntitlementsEditor.url),
			headers=self.headers(), timeout = EntitlementsEditor._timeout)
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
			return self._products
		print("Available products")
		data = json.loads(response.text)
		self._products = [item['name'] for item in data]
		return self._products

	def list_entitlements(self):
		try:
			response = requests.get('{}/entitlements/{}'
			.format(EntitlementsEditor.url, self.userID()), 
			headers=self.headers(), timeout = EntitlementsEditor._timeout)
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
			return
		print("Current [game] entitlements")
		print(response.text)

	def add_entitlements(self, entitlement_ids):
		entitlement_ids = self.verify(entitlement_ids)
		if not entitlement_ids: return
		try:
			response = requests.post('{}/entitlements/{}'
			.format(EntitlementsEditor.url, self.userID())
			, headers=self.headers()
			, data= ",".join(entitlement_ids)
			, timeout = EntitlementsEditor._timeout) #.options("language=json")
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
			return
		print("Added entitlement(s)")
		
		print(response.text)

	def remove_entitlements(self, entitlement_ids):
		entitlement_ids = self.verify(entitlement_ids)
		try:
			response = requests.delete('{}/entitlements/{}'
			.format(EntitlementsEditor.url, self.userID())
			, headers=self.headers()
			, data= ",".join(entitlement_ids)
			, timeout = EntitlementsEditor._timeout) #.options("language=json")
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
			return
		print("REMOVED entitlement(s)")
		print(response.text)
		
	def verify(self, entitlement_ids):
		verified = []
		for id in entitlement_ids:
			if id.startswith("game_dungeons_"):
				print("Error, backoffice no longer accepts individudal entitlements in API. Run 'py main.py -a' to list available args!")
				continue
			if id in all_entitlements.keys():
				verified.append(id)
			else:
				print("Error, no such entitlement exists '{}'".format(id))

		return verified

		return True

	def not_responding(self):
		print("The server is not responding. Are you connected through a Mojang service environment? (i.e. in the Mojang office or on VPN)?")


	def userID(self):
		if not self._UserID:
			self._UserID = helpers.get_userid(self.base_path)
		return self._UserID

	def headers(self):
		if not self.has_valid_token():
			raise "no valid token, can't produce headers for curl!"
		if not self._header:
			with open(EntitlementsEditor._token_cached) as f:
				token_response=json.load(f)
				self._header = { 'Authorization': 'Bearer {}'.format(token_response['access_token']) }

		return self._header
