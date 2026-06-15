'use strict'

const readAndParse = {
    levelPart: require('./validator/common/readers/level-part').str,
}

const { getTilesUsed } = require('./validator/extras/get-tiles-used-str')


if (process.argv.length != 3) {
    console.error(`use: node ls-tiles.js <levelname>`)
    process.exit(1)
}

const level = readAndParse.levelPart(`${process.argv[2]}.json`)

const tileUsed = getTilesUsed(level)

console.log(tileUsed.join('\n'))