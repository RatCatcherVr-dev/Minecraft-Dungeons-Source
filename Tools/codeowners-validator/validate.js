'use strict'

const fs = require('fs')
const path = require('path')

const glob = require('glob')

function raise (message, index, line) {
    console.log(`${message}\n${index}: ${line}`)
    process.exit(1)
}

const basePath = path.join(__dirname, '../..')
const source = fs.readFileSync(path.join(basePath, '.github/CODEOWNERS'), 'utf-8')

const lines = source.split('\n')
    .map((line, index) => ({ line, index }))
    .filter(({ line }) => !/(?:^\s*#)|(?:^\s*$)/.test(line))

lines.forEach(({ line, index }) => {
    const [pattern, ...owners] = line.trim().split(/\s+/)

    if (owners.length === 0) {
        raise(`missing owners on line`, index, line)
    }

    if (pattern.startsWith('/')) {
        const matches = glob.sync(`.${pattern}`, { cwd: basePath })

        if (matches.length === 0) {
            raise(`could not find any files for given pattern`, index, line)
        }
    }

    // skip *, *.extension and other non-root patterns
})