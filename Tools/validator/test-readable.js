'use strict'

const fs = require('fs')
const path = require('path')

const { validate } = require('./cli')
const { stringifyIssues } = require('./common/stringify-loc')

const basePath = '../../content/data/lovika/levels'

const dirs = fs.readdirSync(basePath).filter((file) => fs.lstatSync(path.join(basePath, file)).isDirectory())

const levelFiles = []

dirs.forEach((dir) => {
	if (/^sub_/.test(dir)) {
		return
	}

	const mainLevelPath = path.join(basePath, dir, `${dir}.json`)

	if (fs.existsSync(mainLevelPath)) {
		levelFiles.push(path.join(dir, `${dir}.json`))
	}
})

const issues = []

levelFiles.forEach((levelFile) => {
	const batch = validate(levelFile)

	batch.forEach((issue) => {
		if (issue.extras == null) {
			issue.extras = {
				loc: {
					filePath: levelFile
				}
			}
		}
	})

	issues.push(...batch)
})

console.log(stringifyIssues(issues))