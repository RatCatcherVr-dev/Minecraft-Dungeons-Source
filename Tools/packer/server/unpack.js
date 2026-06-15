'use strict'

const fs = require('fs')
const path = require('path')

function unpack (json) {
    const promises = []

    Object.entries(json.level).forEach(([filePath, content]) => {
        promises.push(new Promise((resolve, reject) => {
            const completePath = path.join(__dirname, '../../..', 'Content/data/lovika/levels', filePath)
            fs.mkdirSync(path.dirname(completePath), { recursive: true })
            fs.writeFile(
                completePath,
                content,
                (err) => {
                    if (err) {
                        reject(filePath)
                    } else {
                        resolve()
                    }
                }
            )
        }))
    })

    Object.entries(json.objectGroup).forEach(([filePath, content]) => {
        promises.push(new Promise((resolve, reject) => {
            const completePath = path.join(__dirname, '../../..', 'Content/data/lovika/objectgroups', filePath)
            fs.mkdirSync(path.dirname(completePath), { recursive: true })
            fs.writeFile(
                completePath,
                content,
                (err) => {
                    if (err) {
                        reject(filePath)
                    } else {
                        resolve()
                    }
                }
            )
        }))
    })

    return Promise.all(promises)
}

Object.assign(module.exports, {
    unpack,
})