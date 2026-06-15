from source.EntitlementsEditor import EntitlementsEditor
import json
import requests


class TrialsEditor(EntitlementsEditor):
	def __init__(self, base_path):
		print("Creating Trials Editor")

		# sign in to backoffice is the same as for the entitlements
		# need file to use to generate etc.

	def __str__(self):
		return "Trials Editor:: TEMP::"

	def GetTrial(self):
		example_id="1485e50c-9bb6-4dc5-9552-2cd9b97417e7"
		try:
			response = requests.get("{}/trials/game/dungeons/trialId/{}", EntitlementsEditor.url, example_id)
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
		return response

	def GetTrials(self):
		try:
			response = requests.get("{}/trials/game/dungeons?=".format(TrialsEditor.url))
		except(requests.ConnectTimeout, requests.ConnectionError):
			self.not_responding()
		return response

	def ShowTrials(self):
		print("using backoffice: {}".format(TrialsEditor.url))
		currentTrials = json.loads(self.GetTrials().text)
		print("CURRENT TRIALS")
		for item in currentTrials:
			print(item)

	def GenerateTrials(self):
		NotImplemented