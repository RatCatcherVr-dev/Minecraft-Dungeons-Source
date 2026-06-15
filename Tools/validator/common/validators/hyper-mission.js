'use strict'

const fs = require('fs')
const path = require('path')

const { ast } = require('./core-str')
const { weightedId, getStringOrProperty } = require('./common-str')
const { mobGroupEntries } = require('./mob-groups')

const splicer = require('../splicer/splice-level-generic')
const { methods: nativeMethods } = require('../splicer/methods/native')
const readAndParse = {
    levelPart: require('../readers/level-part').native,
    objectGroup: require('../readers/object-group').native,
}


const archetypeRequirements = ast.shapeOnly({
    'acrobat': ast.optional(ast.number()),
    'archer': ast.optional(ast.number()),
    'fighter': ast.optional(ast.number()),
    'mage': ast.optional(ast.number()),
    'soul': ast.optional(ast.number()),
    'summoner': ast.optional(ast.number()),
    'support': ast.optional(ast.number()),
    'tank': ast.optional(ast.number()),
    'special': ast.optional(ast.number()),
})

const entityType = ast.string()

const hyperDungeon = ast.shapeOnly({
    'id': ast.string(),
    'weight': ast.optional(ast.number()),
    'mob-types': ast.optional(ast.every(entityType)),
    'archetype-requirements': ast.optional(archetypeRequirements),
    'required-dlc': ast.optional(ast.string()),
})

const missionMobs = ast.shapeOnly({
    'ids': ast.every(ast.string()),
    'types': ast.every(ast.string()),
});

const hyperMission = ast.shapeOnly({
    'hyperdungeons': ast.every(hyperDungeon),
    'levels': ast.every(weightedId()),
    'definition-levels': ast.every(weightedId()),
    'hypermission-mobs': ast.optional(mobGroupEntries),
    'mission-mapping': ast.optional(ast.every(missionMobs)),
})

const basePath = path.join(__dirname, '../../../..', 'Content/data/lovika/levels')

function hyperMissionMore (hyperMission, context) {
    function validateLevelRef (level) {
        const levelName = getStringOrProperty(level, 'id')

        if (!fs.existsSync(path.join(basePath, `${levelName}.json`))) {
            context.raise(
                `tried to reference level '${levelName}' which does not exist (or has not been spliced)`,
                level,
            )
        }
    }

    hyperMission.get('levels').forEach(validateLevelRef)
    hyperMission.get('definition-levels').forEach(validateLevelRef)

    if (hyperMission.has('hypermission-mobs')) {
        const mobTypes = new Set(
            hyperMission.get('hypermission-mobs').map((mobGroupEntry) => getStringOrProperty(mobGroupEntry, 'type'))
        )

        hyperMission.get('mission-mapping').forEach((missionMapping) => {
            missionMapping.get('types').forEach((typeNode) => {
                const type = typeNode.value()

                if (!mobTypes.has(type)) {
                    context.raise(
                        `tried to reference undeclared '${type}' mob type`,
                        typeNode,
                    )
                }
            })
        })
    }

    // ---
    const dungeonIds = new Set

    function addDungeonIds (levelName) {
        // there is no way to know the location of the unspliced file; it's not always levelName/levelName.json :/
        // const levelFile = path.join(levelName, `${levelName}.json`)
        // const spliced = splicer.run(nativeMethods, readAndParse, levelFile)

        // using spliced version as a (the only?) alternative
        // there is little (no?) guarantee that the levels are spliced on disk (!) at the time the hypermissons get validated
        const spliced = readAndParse.levelPart(`${levelName}.json`)
        spliced['dungeons'].forEach((dungeon) => {
            dungeonIds.add(dungeon['id'])
        })
    }

    hyperMission.get('levels').forEach((level) => {
        addDungeonIds(level.value())
    })

    hyperMission.get('definition-levels').forEach((definitionLevel) => {
        addDungeonIds(definitionLevel.value())
    })

    hyperMission.get('hyperdungeons').forEach((hyperDungeon) => {
        const id = hyperDungeon.get('id').value()

        if (!dungeonIds.has(id)) {
            context.raise(
                `tried to reference undeclared '${id}' dungeon; verify that involved levels are spliced`,
                hyperDungeon.get('id'),
            )
        }
    })
}

Object.assign(module.exports, {
    hyperMission,
    hyperMissionMore,
})
