'use strict'

const { getTilesUsed } = require('./get-tiles-used-str')
const readAndParse = {
    objectGroup: require('../common/readers/object-group').native,
}

function getTileId (id) {
    const maybeMatch = id.match(/^[^@]+@([^@]+)$/)

    return maybeMatch != null
        ? maybeMatch[1]
        : id
}

function makeLoc (objectGroup, pos) {
    return {
        objectGroup,
        coords: {
            x: pos[0],
            y: pos[1],
            z: pos[2],
        },
    }
}

function makeExtras (loc, locDuplicate) {
    return arguments.length > 1
        ? { loc, locDuplicate }
        : { loc }
}

function validate (level, context) {
    const tileIds = new Map

    level.get('object-groups').forEach((objectGroupNameNode) => {
        const objectGroupName = objectGroupNameNode.value()
        const objectGroup = readAndParse.objectGroup(`${objectGroupName}.json`)

        objectGroup['objects'].forEach((object) => {
            const id = object['id']

            if (id === '') {
                context.raise(
                    `can not have empty ids; '${objectGroupName}' contains a tile with an empty id`,
                    makeExtras(makeLoc(objectGroupName, object['pos'])),
                )
            }

            if (/\s+/.test(id)) {
                context.raise(
                    `can not have spaces in ids; '${objectGroupName}' contains a tile with a space-y id`,
                    makeExtras(makeLoc(objectGroupName, object['pos'])),
                )
            }

            if (tileIds.has(id)) {
                const entry = tileIds.get(id)

                const message = objectGroupName === entry.objectGroup
                    ? `tile '${id}' appears twice in '${objectGroupName}'`
                    : `tile '${id}' appears in both '${objectGroupName}' and '${tileIds.get(id).objectGroup}'`

                context.raise(
                    message,
                    makeExtras(makeLoc(objectGroupName, object['pos']), entry),
                )
            }

            tileIds.set(
                id,
                Object.assign(makeLoc(objectGroupName, object['pos']), { doors: object['doors'] }),
            )

            if (object.hasOwnProperty('regions')) {
                object['regions'].forEach((region) => {
                    if (
                        region['type'] === 'trigger' &&
                        region['name'] === '' &&
                        region['tags'] === ''
                    ) {
                        context.raise(
                            'found trigger region with empty name and tags',
                            makeExtras(makeLoc(objectGroupName, object['pos'])),
                        )
                    }
                })
            }
        })
    })

    const usedTileIds = getTilesUsed(level)

    usedTileIds.forEach((usedTileId) => {
        if (!tileIds.has(getTileId(usedTileId))) {
            const objectGroups = level.get('object-groups').map((objectGroup) => objectGroup.value())
            context.raise(
                `tile '${usedTileId}' does not appear in ${objectGroups.join(', ')}`
            )
        }
    })

    level.get('tiles').forEach((tile) => {
        if (tile.has('entry-door')) {
            const expectedDoor = tile.get('entry-door').value()
            const tileId = tile.get('id').value()
            const prefabId = getTileId(tileId)

            if (tileIds.has(prefabId)) {
                const tile = tileIds.get(prefabId)

                if (
                    tile.doors != null &&
                    !tile.doors.some((door) => door['name'] === expectedDoor)
                ) {
                    context.raise(
                        `tile '${tileId}' does not have a door named '${expectedDoor}' to use as an entry door`,
                        makeExtras(tile),
                    )
                }
            } else {
                const objectGroups = level.get('object-groups').map((objectGroup) => objectGroup.value())

                context.raise(
                    `tile '${tileId}' does not appear in ${objectGroups.join(', ')}`,
                    tile,
                    'warning',
                )
            }
        }

        if (tile.has('exit-door')) {
            const expectedDoors = tile.get('exit-door').type === 'array'
                ? tile.get('exit-door')
                : [tile.get('exit-door')]

            const tileId = tile.get('id').value()
            const prefabId = getTileId(tileId)

            if (tileIds.has(prefabId)) {
                const tile = tileIds.get(prefabId)

                expectedDoors.forEach((expectedDoor) => {
                    if (
                        tile.doors != null &&
                        !tile.doors.some((door) => door['name'] === expectedDoor.value())
                    ) {
                        context.raise(
                            `tile '${tileId}' does not have a door named '${expectedDoor.value()}' to use as an exit door`,
                            makeExtras(tile)
                        )
                    }
                })
            } else {
                const objectGroups = level.get('object-groups').map((objectGroup) => objectGroup.value())

                context.raise(
                    `tile '${tileId}' does not appear in ${objectGroups.join(', ')}`,
                    tile,
                    'warning',
                )
            }
        }

        const tileId = getTileId(tile.get('id').value())
        const tileEntry = tileIds.get(tileId)

        if (tile.has('teleports')) {
            tile.get('teleports').forEach((teleport) => {
                const doorName = teleport.get('door').value()

                if (
                    tileEntry == null ||
                    tileEntry.doors == null ||
                    !tileEntry.doors.some((door) => door['name'] === doorName)
                ) {
                    context.raise(
                        `door '${doorName}' does not appear in tile '${tileId}'`,
                        tile,
                    )
                }
            })
        }
    })
}

Object.assign(module.exports, {
    validate,
})