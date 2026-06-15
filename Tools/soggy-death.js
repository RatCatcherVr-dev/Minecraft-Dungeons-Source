'use strict'

const fs = require('fs')
const path = require('path')

if (process.argv.length != 3) {
    console.error(`use: node soggy-death.js <levelname>`)
    process.exit(1)
}

const levelName = process.argv[2]
const levelPath = path.join(__dirname, '..', 'Content/data/lovika/objectgroups', levelName, 'objectgroup.json')

function findLowest (elements) {
    return Math.min(...elements.map(({ pos }) => pos[1]))
}

fs.readFile(levelPath, (err, data) => {
    if (err) {
        console.error(`could not open ${levelName}`)
        process.exit(1)
    }

    const objectgroup = JSON.parse(data)

    objectgroup.objects.forEach((object) => {
        const newRegions = object.regions.filter((region) => region.tags !== 'boundary-force')

        const doorLevelMin = findLowest(object.doors)
        const regionLevelMin = findLowest(object.regions.filter(
            ({ type }) =>
                type === 'spawn' ||
                type === 'loot'
        ))

        const levelMin = Math.min(doorLevelMin, regionLevelMin)

        newRegions.push({
            name: '',
            type: 'trigger',
            tags: 'boundary-force',
            pos: [0, levelMin - 1, 0],
            size: object.size,
        })

        object.regions = newRegions
    })

    fs.writeFileSync(levelPath, JSON.stringify(objectgroup, null, 2))
})