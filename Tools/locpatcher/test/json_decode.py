import json

json_test='''{
	"id": "FieryForge",
	"object-groups": [
		"fieryforge/objectgroup"
	],
	"resource-packs": [
		"RedstoneMines"
	],

	"visual-theme": "cave",
	"sound-theme": "cave",
	"prop-density": 0.5
}'''

data = json.loads(json_test)
for l in data:
    print(l)