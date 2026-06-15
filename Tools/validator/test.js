'use strict'

const fs = require('fs')
const path = require('path')
const assert = require('assert')

const { validate } = require('./cli')
const { stringifyIssues } = require('./common/stringify-loc')

const basePath = '../../content/data/lovika/levels'

describe('spliced-levels', () => {
	// fs.readdirSync(basePath).forEach((candidate) => {
	// 	console.log(".cand %s", candidate)
	// 	console.log(test(candidate))
	// })
	const levelFiles = fs.readdirSync(basePath).filter((levelfile) => {
		return !levelfile.startsWith("sub_") 
		&& path.extname(levelfile) == ".json"
		&& !path.basename(levelfile).includes("test")
		&& fs.lstatSync(path.join(basePath, levelfile)).isFile()

	})

	const issues = []
	levelFiles.forEach((levelFile) => {
		const batch = validate(levelFile)
		console.log(".checked %s", levelFile)

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

	const lines = []
	lines.push(stringifyIssues(issues))

	it ('validate', () => {
		if (issues.length <= 0) {
			assert.ok(true)
		} else {
			lines.unshift('')
			assert.ok(false, lines.join('\n'))
		}
	})
})

describe('levels', () => {
	const dirs = fs.readdirSync(basePath).filter((dirname) => fs.lstatSync(path.join(basePath, dirname)).isDirectory())
	const levelFiles = []

	dirs.forEach((dir) => {
		const mainLevelPath = path.join(basePath, dir, `${dir}.json`)
		if (fs.existsSync(mainLevelPath) && !dir.startsWith("sub_")) {
			levelFiles.push(path.join(dir, `${dir}.json`))

			// disable daily levels as they're unmantained and have errors
			// const dailyLevelPath = path.join(basePath, dir, `${dir}daily.json`)
			// if (fs.existsSync(dailyLevelPath)) {
			// 	levelFiles.push(path.join(dir, `${dir}daily.json`))
			// }
		}
	})

	const issues = []
	console.log("#issues %d", issues.length)
	levelFiles.forEach((levelFile) => {
		const batch = validate(levelFile)
		console.log(".checked %s", levelFile)

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

	const lines = []

	lines.push(stringifyIssues(issues))

	it ('validate', () => {
		if (issues.length <= 0) {
			assert.ok(true)
		} else {
			lines.unshift('')
			assert.ok(false, lines.join('\n'))
		}
	})
})