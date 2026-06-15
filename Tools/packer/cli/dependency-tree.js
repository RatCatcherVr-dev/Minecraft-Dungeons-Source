'use strict'

const fs = require('fs')
const path = require('path')

function traverse (file, readAndParse) {
    const files = new Set
    const objectGroups = new Set

    function addFile (file) {
        if (files.has(file)) {
            return true
        }

        files.add(file)
        return false
    }

    function dispatch (file) {
        if (addFile(file)) {
            return
        }

        const tree = readAndParse(file)

        if (tree instanceof Array) {
            tree.forEach((element) => { traverse(element) })
        } else {
            traverse(tree)
        }
    }

    function traverse (tree) {
        if (tree['mob-files'] != null) {
            tree['mob-files'].forEach(dispatch)
        }

        if (tree['tile-files'] != null) {
            tree['tile-files'].forEach(dispatch)
        }

        if (tree['dungeon-files'] != null) {
            tree['dungeon-files'].forEach(dispatch)
        }

        if (tree['object-groups'] != null) {
            tree['object-groups'].forEach((objectGroup) => { objectGroups.add(objectGroup) })
        }
    }

    dispatch(file)

    return {
        level: [...files],
        objectGroup: [...objectGroups].map((objectGroupFile) => `${objectGroupFile}.json`),
    }
}

function run (levelFile) {
    const basePath = path.join(__dirname, '../../..', 'Content/data/lovika/levels')

    function readFile (levelFile) {
        const source = fs.readFileSync(
            path.join(basePath, levelFile),
            { encoding: 'utf8' }
        )

        return Function(`return (${source})`)()
    }

    return traverse(levelFile, readFile)
}

if (require.main === module) {
    if (process.argv.length !== 3) {
        console.error(`use: node dependency-tree.js <levelname>`)
        process.exit(1)
    }

    const result = run(process.argv[2])

    console.log('level files:')
    console.log(result.level.join('\n'))
    console.log('')

    console.log('objectgroup files:')
    console.log(result.objectGroups.join('\n'))
} else {
    Object.assign(module.exports, {
        gatDependencies: run,
    })
}