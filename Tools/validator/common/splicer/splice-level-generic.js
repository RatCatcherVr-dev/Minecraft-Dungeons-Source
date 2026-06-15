'use strict'

const { fixPath } = require('../utils')

function makeSplicer (_, readAndParse) {
    function makeAdder (map, name) {
        return (entry) => {
            if (!_.has(entry, 'id')) {
                throw {
                    message: `${name} must have an 'id' property`,
                    extras: entry,
                }
            }

            const id = _.value(_.get(entry, 'id'))

            if (map.has(id)) {
                throw {
                    message: `found two or more ${name} with id '${id}'`,
                    extras: {
                        loc: entry.loc,
                        locDuplicate: map.get(id).loc,
                    },
                }
            } else {
                map.set(id, entry)
            }
        }
    }

    function addEach (source, adder) {
        _.forEach(source, (element) => { adder(element) })
    }

    function preFixPath (fun) {
        return (filePathRaw) => fun(fixPath(_.value(filePathRaw)))
    }

    function splice (filePathRaw) {
        const filePath = fixPath(filePathRaw)

        const splicedFiles = new Set

        const mobGroupsAll = new Map
        const addMobGroup = makeAdder(mobGroupsAll, 'mob groups')

        const objectGroupsAll = new Map
        const addObjectGroup = (objectGroup) => {
            const id = _.value(objectGroup).toLowerCase()

            if (!objectGroupsAll.has(id)) {
                objectGroupsAll.set(id, objectGroup)
            }
        }

        const propsAll = new Map
        const addProp = makeAdder(propsAll, 'props')

        const tilesAll = new Map
        const addTile = makeAdder(tilesAll, 'tiles')

        const propGroupsAll = new Map
        const addPropGroup = makeAdder(propGroupsAll, 'prop groups')

        const tileGroupsAll = new Map
        const addTileGroup = makeAdder(tileGroupsAll, 'tile groups')

        const challengesAll = new Map
        const addChallenge = makeAdder(challengesAll, 'challenge')

        const dungeonsAll = new Map
        const addDungeon = makeAdder(dungeonsAll, 'dungeons')

        function addSplicedFile (filePath) {
            if (splicedFiles.has(filePath)) {
                return true
            }

            splicedFiles.add(filePath)
            return false
        }

        function dispatch (filePath, parentPath) {
            const data = readAndParse.levelPart(filePath, parentPath)

            if (_.isArray(data)) {
                _.forEach(data, (dungeon) => { traverse(dungeon, filePath) })
            } else {
                traverse(data, filePath)
            }
        }

        function traverse (level, filePath, first = false) {
            if (!first) {
                addDungeon(level)
            }

            // --- mob groups
            addEach(_.get(level, 'mob-groups'), addMobGroup)

            _.forEach(_.get(level, 'mob-files'), preFixPath((mobFile) => {
                if (addSplicedFile(mobFile)) {
                    return
                }

                const mobGroups = readAndParse.levelPart(mobFile, filePath)

                addEach(mobGroups, addMobGroup)
            }))

            // --- tiles
            _.forEach(_.get(level, 'object-groups'), (objectGroup) => {
                addObjectGroup(objectGroup)
            })

            // --- dungeons
            function addDungeons (dataSource) {
                addEach(_.get(dataSource, 'dungeons'), addDungeon)

                _.forEach(_.get(dataSource, 'dungeon-files'), preFixPath((dungeonFile) => {
                    if (addSplicedFile(dungeonFile)) {
                        return
                    }

                    dispatch(dungeonFile, filePath)
                }))
            }

            _.forEach(_.get(level, 'tile-files'), preFixPath((tileFile) => {
                if (addSplicedFile(tileFile)) {
                    return
                }

                const tileData = readAndParse.levelPart(tileFile, filePath)

                _.forEach(_.get(tileData, 'object-groups'), (objectGroup) => {
                    addObjectGroup(objectGroup)
                })

                addEach(_.get(tileData, 'props'), addProp)

                addEach(_.get(tileData, 'tiles'), addTile)

                addEach(_.get(tileData, 'prop-groups'), addPropGroup)

                addEach(_.get(tileData, 'tile-groups'), addTileGroup)

                addEach(_.get(tileData, 'challenges'), addChallenge)

                addDungeons(tileData)
            }))

            addDungeons(level)

            // --- overwrite props
            _.forEach(_.get(level, 'props'), (prop) => {
                propsAll.set(_.value(_.get(prop, 'id')), prop)
            })

            // --- overwrite tiles
            _.forEach(_.get(level, 'tiles'), (tile) => {
                tilesAll.set(_.value(_.get(tile, 'id')), tile)
            })

            _.forEach(_.get(level, 'prop-groups'), (propGroup) => {
                propGroupsAll.set(_.value(_.get(propGroup, 'id')), propGroup)
            })

            _.forEach(_.get(level, 'tile-groups'), (tileGroup) => {
                tileGroupsAll.set(_.value(_.get(tileGroup, 'id')), tileGroup)
            })

            _.forEach(_.get(level, 'challenges'), (challenge) => {
                challengesAll.set(_.value(_.get(challenge, 'id')), challenge)
            })
        }

        const level = readAndParse.levelPart(filePath)

        if (!_.isObject(level)) {
            throw {
                message: 'level must be an object',
                extras: level,
            }
        }

        addSplicedFile(filePath)
        traverse(level, filePath, true)

        ;[...objectGroupsAll.values()].forEach((objectGroupFile) => {
            const objectGroupFileComplete = `${_.value(objectGroupFile)}.json`

            const objectGroup = readAndParse.objectGroup(objectGroupFileComplete)

            _.forEach(_.get(objectGroup, 'objects'), (object) => {
                const tags = _.value(_.get(object, 'tags'))

                if (tags === '') {
                    return
                }

                tags.split(',').forEach((tag) => {
                    const entry = {
                        id: _.value(_.get(object, 'id')),
                        weight: 1,
                    }

                    if (propGroupsAll.has(tag)) {
                        const propGroup = propGroupsAll.get(tag)
                        const props = _.get(propGroup, 'props')

                        if (!props.some((entry) => _.value(_.get(entry, 'id')) === tag)) {
                            props.push(_.lift(entry))
                        }
                    } else {
                        propGroupsAll.set(tag, _.lift({
                            id: tag,
                            props: [entry],
                        }))
                    }
                })
            })
        })

        _.set(level, 'mob-groups', _.wrap([...mobGroupsAll.values()]))

        _.set(level, 'object-groups', _.wrap([...objectGroupsAll.values()]))
        _.set(level, 'props', _.wrap([...propsAll.values()]))
        _.set(level, 'tiles', _.wrap([...tilesAll.values()]))
        _.set(level, 'prop-groups', _.wrap([...propGroupsAll.values()]))
        _.set(level, 'tile-groups', _.wrap([...tileGroupsAll.values()]))
        _.set(level, 'challenges', _.wrap([...challengesAll.values()]))

        _.set(level, 'dungeons', _.wrap([...dungeonsAll.values()]))

        _.remove(level, 'mob-files')
        _.remove(level, 'tile-files')
        _.remove(level, 'dungeon-files')

        return level
    }

    return splice
}

function run (_, readAndParse, levelFile) {
    const splicer = makeSplicer(_, readAndParse)
    return splicer(levelFile)
}

Object.assign(module.exports, {
    run,
})