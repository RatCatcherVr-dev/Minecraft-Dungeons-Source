'use strict'

const fs = require('fs')
const path = require('path')

function diff (a, b) {
    const result = []

    if (b instanceof Set) {
        a.forEach((element) => {
            if (!b.has(element)) {
                result.push(element)
            }
        })
    } else {
        a.forEach((element) => {
            if (!b.includes(element)) {
                result.push(element)
            }
        })
    }

    return result
}

function getId ({ id }) {
    return id
}

function trim (object, maxElements = 4) {
    const result = {}

    Object.keys(object).forEach((key) => {
        const prop = object[key]

        if (
            typeof prop.length !== 'undefined' &&
            prop.length <= maxElements
        ) {
            result[key] = prop
        }
    })

    return result
}

if (process.argv.length != 4) {
    console.error(`use: node diff-objectgroup.js <jsonA> <jsonB>`)
    process.exit(1)
}

const objectgroupAPath = path.join(__dirname, '..', process.argv[2])
const objectgroupBPath = path.join(__dirname, '..', process.argv[3])

fs.readFile(objectgroupAPath, (err, data) => {
    if (err) {
        console.error('could not open objectgroupA')
        process.exit(1)
    }

    const objectgroupA = JSON.parse(data)
    const idsA = new Set(objectgroupA.objects.map(getId))

    fs.readFile(objectgroupBPath, (err, data) => {
        if (err) {
            console.error('could not open objectgroupB')
            process.exit(1)
        }

        const objectgroupB = JSON.parse(data)
        const idsB = new Set(objectgroupB.objects.map(getId))

        console.log('tiles present only in A')
        console.log(...diff(idsA, idsB))

        console.log('tiles present only in B')
        console.log(...diff(idsB, idsA))
    })
})