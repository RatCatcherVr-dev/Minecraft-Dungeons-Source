'use strict'

const fs = require('fs')
const path = require('path')

function parse (source) {
    return Function(`return (${source})`)()
}

function isObject (candidate) {
    return candidate != null &&
        typeof candidate === 'object' &&
        !(candidate instanceof Array)
}

function isArray (candidate) {
    return candidate instanceof Array
}

const includePropName = '#include'

function splice (filePath, readFile) {
    return (function recurse (filePath) {
        const source = readFile(filePath)

        const data = parse(source)

        if (!isObject(data)) {
            return data
        }

        const includes = data[includePropName]

        if (!isArray(includes)) {
            return data
        }

        delete data[includePropName]

        includes.forEach((include) => {
            const subData = recurse(include)

            if (!isObject(subData)) {
                return
            }

            Object.assign(data, subData)
        })

        return data
    })(filePath)
}

function run (levelFile) {
    const basePath = path.join(__dirname, '..', 'Content/data/lovika/levels')

    function readFile (levelFile) {
        return fs.readFileSync(
            path.join(basePath, levelFile),
            { encoding: 'utf8' }
        )
    }

    const spliced = splice(levelFile, readFile)
    const stringed = JSON.stringify(spliced)

    const parts = path.parse(levelFile)
    fs.writeFileSync(path.join(basePath, parts.dir, `${parts.name}-compact.json`), stringed)
}

if (process.argv.length !== 3) {
    console.error(`use: node splice-level.js <levelname>`)
    process.exit(1)
}

run(process.argv[2])