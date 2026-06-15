'use strict'

const { fixPath } = require('../validator/common/utils')
const { native: readLevel } = require('../validator/common/readers/level-part')

function getRefs (levelFile) {
    const level = readLevel(levelFile)

    return {
        objectGroups: level['object-groups'],
        resourcePacks: level['resource-packs'],
    }
}

if (require.main === module) {
    if (process.argv.length !== 3) {
        console.error(`use: cli.js <levelfile>`)
        process.exit(1)
    }

    const refs = getRefs(fixPath(process.argv[2]))
    console.log(refs)
} else {
    Object.assign(module.exports, {
        getRefs,
    })
}