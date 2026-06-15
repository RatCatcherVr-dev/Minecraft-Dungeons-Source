'use strict'

const fs = require('fs')
const path = require('path')

const splicer = require('./splice-level-generic')
const { methods: nativeMethods } = require('./methods/native')
const { fixPath, isHyperMission } = require('../utils')

const readAndParse = {
    levelPart: require('../readers/level-part').native,
    objectGroup: require('../readers/object-group').native,
}

function runAndWrite (file) {
    const spliced = isHyperMission(file)
        ? readAndParse.levelPart(file)
        : splicer.run(nativeMethods, readAndParse, file)

    const stringed = JSON.stringify(spliced, null, 4)

    const banner = [
        '// ███╗   ██╗ ██████╗     ████████╗ ██████╗ ██╗   ██╗ ██████╗██╗  ██╗██╗██╗██╗',
        '// ████╗  ██║██╔═══██╗    ╚══██╔══╝██╔═══██╗██║   ██║██╔════╝██║  ██║██║██║██║',
        '// ██╔██╗ ██║██║   ██║       ██║   ██║   ██║██║   ██║██║     ███████║██║██║██║',
        '// ██║╚██╗██║██║   ██║       ██║   ██║   ██║██║   ██║██║     ██╔══██║╚═╝╚═╝╚═╝',
        '// ██║ ╚████║╚██████╔╝       ██║   ╚██████╔╝╚██████╔╝╚██████╗██║  ██║██╗██╗██╗',
        '// ╚═╝  ╚═══╝ ╚═════╝        ╚═╝    ╚═════╝  ╚═════╝  ╚═════╝╚═╝  ╚═╝╚═╝╚═╝╚═╝'
    ].join('\n')

    const basePath = path.join(__dirname, '../../../..', 'Content/data/lovika/levels')
    fs.writeFileSync(path.join(basePath, `${path.parse(file).name}.json`), `${banner}\n${stringed}`)
}

if (require.main === module) {
    if (process.argv.length !== 3) {
        console.error(`use: node cli.js <levelname>|<hypermission>`)
        process.exit(1)
    }

    runAndWrite(fixPath(process.argv[2]))
} else {
    Object.assign(module.exports, {
        runAndWrite,
    })
}