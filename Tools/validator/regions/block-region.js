'use strict'

function BlockRegion (blockData, size) {
    this.size = size

    this.volume = []

    let p = 0
    for (let y = 0; y < size.y; y++) {
        const zxPlane = []

        for (let z = 0; z < size.z; z++) {
            const line = []

            for (let x = 0; x < size.x; x++) {
                line.push(blockData[p])
                p++
            }

            zxPlane.push(line)
        }

        this.volume.push(zxPlane)
    }
}

BlockRegion.prototype.get = function (x, y, z) {
    return this.volume[y][z][x]
}

BlockRegion.prototype.toString = function () {
    const lines = []

    for (let z = 0; z < this.size.z; z++) {
        const line = []

        for (let x = 0; x < this.size.x; x++) {
            let isAnySoilid = false

            for (let y = 0; y < this.size.y; y++) {
                if (this.volume[y][z][x] > 10) { // arbitrary low id that makes the result look like something recognisable
                    isAnySoilid = true
                    break
                }
            }

            line.push(isAnySoilid ? 'x' : '.')
        }

        lines.push(line)
    }

    return lines.map((line) => line.join('')).join('\n')
}

Object.assign(module.exports, {
    BlockRegion,
})