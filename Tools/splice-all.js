'use strict'

const fs = require('fs')
const path = require('path')

const validator = require('./validator/cli')
const { stringifyIssues } = require('./validator/common/stringify-loc')
const splicer = require('./validator/common/splicer/cli')
const { isHyperMission } = require('./validator/common/utils')


const basePath = path.join(__dirname, '..', 'Content/data/lovika/levels')

const files = fs.readdirSync(basePath)
const dirs = files.filter((file) => fs.lstatSync(path.join(basePath, file)).isDirectory())

const success = []
const failure = []

function run (dir, file, suffix = '') {
    const shortPath = path.join(dir, `${file}${suffix}.json`)

    const issues = validator.validate(shortPath)
    if (issues.length > 0) {
        failure.push(shortPath)
        console.error(`failed to splice ${shortPath}`)
        console.error(stringifyIssues(issues))
        console.error('')
    } else {
        splicer.runAndWrite(shortPath)
        success.push(shortPath)
    }
}

dirs.forEach((dir) => {
    try {
        const mainLevelPath = path.join(basePath, dir, `${dir}.json`)
        if (fs.existsSync(mainLevelPath)) {
            run(dir, dir)

            const dailyLevelPath = path.join(basePath, dir, `${dir}daily.json`)
            if (fs.existsSync(dailyLevelPath)) {
                run(dir, dir, 'daily')
            }
        }

        {
            // hypermission
            const files = fs.readdirSync(path.join(basePath, dir))
            const hyperFiles = files.filter(isHyperMission)
            hyperFiles.forEach((hyperFile) => {
                run(dir, path.parse(hyperFile).name)
            })
        }
    } catch (ex) {
        console.error(`exception thrown while splicing ${dir}; ASK ADRIAN/ARON`)
    }
})

console.log('success:')
success.forEach((shortPath) => { console.log(`    ${shortPath}`) })
console.log('')

console.error('failure:')
failure.forEach((shortPath) => { console.error(`    ${shortPath}`) })