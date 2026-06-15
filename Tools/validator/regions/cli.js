'use strict'

const zlib = require('zlib')

const readAndParse = require('../common/readers/object-group').native
const { BlockRegion } = require('./block-region')
const blocks = require('./blocks')


function parseBlocks (blockDataEncoded, size) {
    const blockDataCompressed = Buffer.from(blockDataEncoded, 'base64')

    const blockDataRaw = zlib.inflateSync(blockDataCompressed)

    const blockCount = size.x * size.y * size.z

    const dataSize = Math.floor((blockCount + 1) / 2)
    const bytesPerBlock = (blockDataRaw.length - dataSize) / blockCount

    if (bytesPerBlock === 1) {
        return new BlockRegion(blockDataRaw, size)
    } else if (bytesPerBlock === 2) {
        const blockData = []

        for (let i = 0, p = 0; i < blockCount; i++, p += 2) {
            blockData.push((blockDataRaw[p] << 8) | blockDataRaw[p + 1])
        }

        return new BlockRegion(blockData, size)
    } else {
        throw `bytesPerBlock ${bytesPerBlock} not supported`
    }
}

function regionIntersectsTerrain (region, blockRegion) {
    const from = {
        x: region.pos[0],
        y: region.pos[1],
        z: region.pos[2],
    }

    const to = {
        x: from.x + region.size[0],
        y: from.y + region.size[1],
        z: from.z + region.size[2],
    }

    for (let y = from.y; y < to.y; y++) {
        for (let z = from.z; z < to.z; z++) {
            for (let x = from.x; x < to.x; x++) {
                const blockId = blockRegion.get(x, y, z)

                if (blocks.isSolid(blockId)) {
                    return {
                        intersects: true,
                        blockName: blocks.getName(blockId),
                        position: { x, y, z },
                    }
                }
            }
        }
    }

    return { intersects: false }
}

function run (objectgroupName) {
    const objectgroup = readAndParse(objectgroupName)

    const messages = []

    objectgroup.objects.forEach((object) => {
        if (object.regions == null) {
            return
        }

        const size = {
            x: object.size[0],
            y: object.size[1],
            z: object.size[2],
        }

        const blockRegion = parseBlocks(object.blocks, size)

        object.regions.forEach((region) => {
            if (region.type !== 'spawn') {
                return
            }

            const result = regionIntersectsTerrain(region, blockRegion)

            if (result.intersects) {
                messages.push(`region in tile "${object.id}" at [${region.pos[0]} ${region.pos[1]} ${region.pos[2]}] intersects ${result.blockName} at [${result.position.x} ${result.position.y} ${result.position.z}]`)
            }
        })
    })

    messages.sort((a, b) => a.length - b.length)
    messages.forEach((message) => { console.log(message) })
}


if (process.argv.length !== 3) {
    console.error(`use: cli.js <objectgroup-dir>`)
    process.exit(1)
}

run(`${process.argv[2]}/objectgroup.json`)