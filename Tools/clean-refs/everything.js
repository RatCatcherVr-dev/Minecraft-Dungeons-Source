'use strict'

const fs = require('fs')
const path = require('path')

const rimraf = require('rimraf')

const { native: readLevel } = require('../validator/common/readers/level-part')
const { fixPath, isHyperMission } = require('../validator/common/utils')

const { getRefs } = require('./get-refs')

function getKeepers (files) {
    const levelFiles = new Set(
        files
            .filter((file) => !isHyperMission(file))
            .map(fixPath)
            .map((levelFileRaw) => `${levelFileRaw}.json`)
    )

    const hyperFiles = new Set(
        files
            .filter(isHyperMission)
            .map(fixPath)
            .map((hyperFileRaw) => `${hyperFileRaw}.json`)
    )

    hyperFiles.forEach((file) => {
        const hyperMission = readLevel(file)

        hyperMission['levels'].forEach((levelName) => { levelFiles.add(`${levelName}.json`) })
        hyperMission['definition-levels'].forEach((levelName) => { levelFiles.add(`${levelName}.json`) })
    })

    const objectGroups = new Set
    const resourcePacks = new Set

    levelFiles.forEach((levelFile) => {
        const refs = getRefs(levelFile)

        refs.objectGroups.forEach((objectGroup) => {
            objectGroups.add(objectGroup.toUpperCase())
        })

        refs.resourcePacks.forEach((resourcePack) => {
            resourcePacks.add(resourcePack.toUpperCase())
        })
    })

    return {
        levelLikes: [...levelFiles, ...hyperFiles].map((levelFile) => path.parse(levelFile).base.toUpperCase()).sort(),
        objectGroups: [...objectGroups].map((objectGroup) => path.parse(objectGroup).dir).sort(), // assuming blabla/objectgroup.json
        resourcePacks: [...resourcePacks].sort(),
    }
}

function lsExcept (basePath, exceptions) {
    const exceptionsSet = new Set(exceptions)
    return fs.readdirSync(basePath).filter((file) => !exceptionsSet.has(file.toUpperCase()))
}

function run (files, options) {
    const keepers = getKeepers(files)

    const action = options.listOnly ? console.log : rimraf.sync

    function clean (basePath, keepers) {
        lsExcept(basePath, keepers).forEach((entry) => {
            action(path.join(basePath, entry))
        })
    }

    clean(path.join(__dirname, '../..', 'Content/data/lovika/levels'), keepers.levelLikes)
    clean(path.join(__dirname, '../..', 'Content/data/lovika/objectgroups'), keepers.objectGroups)
    clean(path.join(__dirname, '../..', 'Content/data/resourcepacks'), keepers.resourcePacks)
}

if (process.argv.length < 3) {
    console.error(`use: everything.js <levelfile-1> ... <levelfile-n>`)
    process.exit(1)
}

const levelFiles = process.argv.slice(2).filter((levelFile) => levelFile !== '--list-only')
const options = {
    listOnly: process.argv.slice(2).includes('--list-only'),
}

run(levelFiles, options)