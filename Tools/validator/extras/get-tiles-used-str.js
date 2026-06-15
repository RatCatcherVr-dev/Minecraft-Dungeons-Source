'use strict'

function getId (weightedId) {
    return weightedId.type === 'object'
        ? weightedId.get('id').value()
        : weightedId.value()
}

function getTilesUsed (level) {
    const tileGroups = new Map
    if (level.has('tile-groups')) {
        level.get('tile-groups').forEach((tileGroup) => {
            const tiles = tileGroup.get('tiles').map(getId)
            tileGroups.set(tileGroup.get('id').value(), tiles)
        })
    }

    const tilesUsed = new Set

    function traverseStretch (stretch) {
        if (stretch.has('tiles')) {
            stretch.get('tiles').forEach((weightedId) => {
                const tile = getId(weightedId)
                tilesUsed.add(tile)
            })
        }

        if (stretch.has('tile-groups')) {
            stretch.get('tile-groups').forEach((weightedId) => {
                const tileGroup = getId(weightedId)
                tileGroups.get(tileGroup).forEach((tile) => {
                    tilesUsed.add(tile)
                })
            })
        }
    }

    level.get('stretches').forEach(traverseStretch)

    if (level.has('dungeons')) {
        level.get('dungeons').forEach((dungeon) => {
            dungeon.get('stretches').forEach(traverseStretch)
        })
    }

    return [...tilesUsed]
}

Object.assign(module.exports, {
    getTilesUsed,
})