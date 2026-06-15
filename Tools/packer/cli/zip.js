'use strict'

const fs = require('fs')
const path = require('path')

const yazl = require('yazl')

const { gatDependencies } = require('./dependency-tree')

function run (filePath) {
    const dependencies = gatDependencies(filePath)

    const id = [
        path.parse(filePath).name,
        '-',
        new Date().toJSON().replace(/(\d+)-(\d+)-(\d+)T(\d+):(\d+):(\d+).(\d+)Z/, '$1-$2-$3-$4-$5-$6-$7')
    ].join('')

    const zip = new yazl.ZipFile
    dependencies.level.forEach((levelFile) => {
        zip.addFile(
            path.join(__dirname, '../../..', 'Content/data/lovika/levels', levelFile),
            path.join('Content/data/lovika/levels', levelFile),
        )
    })

    dependencies.objectGroup.forEach((objectGroupFile) => {
        zip.addFile(
            path.join(__dirname, '../../..', 'Content/data/lovika/objectgroups', objectGroupFile),
            path.join('Content/data/lovika/objectgroups', objectGroupFile),
        )
    })

    zip.outputStream.pipe(fs.createWriteStream(path.join(__dirname, '..', 'bundles/out', `${id}.zip`)))
        .on('close', () => {
            console.log(`wrote to ${id}.zip`)
        })

    zip.end()
}

if (process.argv.length != 3) {
    console.error(`use: node zip.js <level>`)
    process.exit(1)
}

run(process.argv[2])