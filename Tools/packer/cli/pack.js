'use strict'

const fs = require('fs')
const path = require('path')

const fetch = require('node-fetch')

const { gatDependencies } = require('./dependency-tree')

const config = require('../server/config')

function readLevelFile (levelFile) {
    return fs.readFileSync(
        path.join(__dirname, '../../..', 'Content/data/lovika/levels', levelFile),
        { encoding: 'utf8' }
    )
}

function readObjectGroupFile (objectGroupFile) {
    return fs.readFileSync(
        path.join(__dirname, '../../..', 'Content/data/lovika/objectgroups', objectGroupFile),
        { encoding: 'utf8' }
    )
}

function run (filePath) {
    const dependencies = gatDependencies(filePath)

    const files = {
        level: {},
        objectGroup: {},
    }

    dependencies.level.forEach((levelFile) => {
        files.level[levelFile] = readLevelFile(levelFile)
    })

    dependencies.objectGroup.forEach((objectGroupFile) => {
        files.objectGroup[objectGroupFile] = readObjectGroupFile(objectGroupFile)
    })

    const id = [
        path.parse(filePath).name,
        '-',
        new Date().toJSON().replace(/(\d+)-(\d+)-(\d+)T(\d+):(\d+):(\d+).(\d+)Z/, '$1-$2-$3-$4-$5-$6-$7')
    ].join('')

    const data = JSON.stringify(files)

    const bundlePath = path.join(__dirname, '..', 'bundles/out', `${id}.json`)
    fs.writeFile(bundlePath, data, (err) => {
        console.log(`wrote to ${bundlePath}`)
    })

    const hosts = [{
        name: 'adrian',
        ip: '10.0.0.119',
    }/*, {
        name: 'aron',
        ip: '10.0.0.119',
    }*/]

    hosts.forEach(({ name, ip }) => {
        const url = `http://${ip}:${config['api-port']}/${id}`

        fetch(url, {
            method: 'post',
            headers: {
                'Content-Type': 'application/json',
            },
            body: data,
        }).then(
            () => { console.log(`sent to ${name} (${ip})`) },
            () => { console.log(`could not send to ${name} (${ip})`) },
        )
    })
}

run(process.argv[2])