'use strict'

const levenshtein = require('fast-levenshtein')

const { and, or, variants } = require('./core')
const { ast } = require('./core-str')
const { locator, assetPath, weightedId, getStringOrId } = require('./common-str')
const { objective, challenge } = require('./objective')
const { mobGroups, minMaxPair } = require('./mob-groups')
const stringDiff = require('../string-diff')

const rotation = ast.values([0, 90, 180, 270])

const teleport = ast.shapeOnly({
    'door' : ast.string(),
    'object': ast.optional(ast.string()),
    'exit' : ast.optional(ast.string()),
    'dungeons': ast.optional(ast.every(weightedId())),
})

const tile = ast.shapeOnly({
    'id': ast.string('id must be a string'),
    'rotations': ast.optional(or([
        rotation,
        ast.every(rotation),
    ], 'rotations must be one of 0, 90, 180, 270 or an array of them')),
    'entry-door': ast.optional(ast.string('entry door must be a string')),
    'exit-door': ast.optional(or([
        ast.string('exit door must be a string'),
        ast.every(ast.string('exit door must be a string')),
    ], 'exit-door must be an id or an array of ids')),
    'object': ast.optional(assetPath()),
    'challenges': ast.optional(ast.every(ast.string())),
    'teleports': ast.optional(ast.every(teleport)),
    'unlock-keys': ast.optional(ast.every(ast.string())),
})

const propGroup = ast.shapeOnly({
    'id': ast.string('id must be a string'),
    'props': ast.every(weightedId()),
    'on-regions': ast.optional(ast.every(locator())),
})

const tileGroup = ast.shapeOnly({
    'id': ast.string('id must be a string'),
    'tiles': ast.every(weightedId()),
})

const stretchMobs = ast.shapeOnly({
    'density': ast.optional(ast.number()),
    'only': ast.optional(ast.every(weightedId())),
    'and': ast.optional(ast.every(ast.string())),
    'not': ast.optional(ast.every(ast.string())),
})

const sidePathVariant = and(
    ast.shapeOnly({
        'weight': ast.optional(ast.number()),
        'max-length': ast.optional(minMaxPair),
        'dead-ends': ast.optional(ast.every(weightedId())),
        'tiles': ast.optional(ast.every(weightedId())),
        'tile-groups': ast.optional(ast.every(weightedId())),
    }),
    ast.propsXor(['tiles', 'tile-groups']),
)

const sidePath = ast.shapeOnly({
    'probability': ast.number(),
    'default': ast.optional(sidePathVariant),
    'variants': ast.optional(ast.every(sidePathVariant)),
})

const stretch = and(
    ast.shapeOnly({
        'id': ast.optional(ast.string()),
        'tiles': ast.optional(ast.every(weightedId())),
        'tile-groups': ast.optional(ast.every(weightedId())),
        'prop-groups': ast.optional(ast.every(weightedId())),
        'dead-ends': ast.optional(ast.every(weightedId())),
        'length': ast.optional(ast.number()),
        'visual-theme': ast.optional(ast.string()),
        'sound-theme': ast.optional(ast.string()),
        'ambience': ast.optional(ast.string()),
        'audio-ambience': ast.optional(ast.string()),
        'push-ambience': ast.optional(ast.string()),
        'push-audio-ambience': ast.optional(ast.string()),
        'prop-density': ast.optional(ast.number()),
        'side-paths': ast.optional(sidePath),
        'mobs': ast.optional(stretchMobs),
        'raid-captains': ast.optional(ast.boolean()),
    }),
    ast.propsXor(['tiles', 'tile-groups']),
)

const dungeon = ast.shapeOnly({
    'id': ast.string(),
    'ambience-level-id': ast.optional(ast.string()),
    'ambience': ast.optional(ast.string()),
    'audio-ambience': ast.optional(ast.string()),
    'resource-packs': ast.optional(ast.every(ast.string())),
    'death-outside': ast.optional(ast.boolean()),
    'dimension': ast.optional(ast.string()),
    'music-override': ast.optional(ast.string()),
    'stretches': ast.every(stretch),
    'fill': ast.optional(or([
        ast.any(),
        and(
            ast.shape({ 'type': ast.string() }),
            variants(
                (input) => input.get('type').value(),
                [
                    ['none', ast.any()],
                    ['gradient', ast.shape({ 'from': ast.string(), 'to': ast.string() })],
                ],
            ),
        ),
    ], '')),
})

const level = ast.shapeOnly({
    'id': ast.string('id must be a string'),
    'loctable-id': ast.optional(ast.string('loctable-id should be a string')),
    'music-override': ast.optional(ast.string()),
    'require-matching-doors': ast.optional(ast.boolean()),
    'object-groups': ast.every(ast.string("object group references must be strings")),
    'mob-files': ast.optional(ast.every(ast.string("mob file references must be strings"))),
    'resource-packs': ast.every(ast.string()),
    'visual-theme': ast.optional(ast.string()),
    'sound-theme': ast.optional(ast.string()),
    'prop-density': ast.optional(ast.number()),
    'mob-groups': ast.optional(mobGroups),
    'default-mobs': ast.optional(stretchMobs),
    'character-loadout': ast.optional(ast.string()),
    'dimension': ast.optional(ast.string()),
    'initial-dungeons': ast.optional(ast.every(weightedId())),
    'tiles': ast.optional(and(
        ast.every(tile),
        ast.uniqueIds('tiles must have unique ids', ast.util.getId),
    )),
    'props': ast.optional(and(
        ast.every(tile),
        ast.uniqueIds('props must have unique ids', ast.util.getId),
    )),
    'prop-groups': ast.optional(and(
        ast.every(propGroup),
        ast.uniqueIds('prop groups must have unique ids', ast.util.getId),
    )),
    'tile-groups': ast.optional(and(
        ast.every(tileGroup),
        ast.uniqueIds('tile groups must have unique ids', ast.util.getId),
    )),
    'objectives': ast.every(objective),
    'stretches': ast.every(stretch),
    'side-paths': ast.optional(sidePath),
    'dungeons': ast.optional(ast.every(dungeon)),
    'challenges': ast.optional(ast.every(challenge)),
    'play-intro': ast.optional(ast.boolean()),
    'death-outside': ast.optional(ast.boolean()),
})

// ---

function levelMore (level, context) {
    function findSimilars (candidates, reference) {
        const similars = []

        function populate (distanceMax) {
            candidates.forEach((candidate) => {
                if (levenshtein.get(candidate, reference) <= distanceMax) {
                    similars.push(candidate)
                }
            })
        }

        populate(1)
        if (similars.length <= 0) {
            populate(2)
        }

        return similars
    }

    function makeRefValidator (set, name) {
        return (ref) => {
            const candidateId = getStringOrId(ref)

            if (!set.has(candidateId)) {
                const similars = findSimilars(set, candidateId)

                const message = (() => {
                    if (similars.length === 0) {
                        return `tried to reference undeclared '${candidateId}' ${name}`
                    }

                    if (similars.length === 1) {
                        return [
                            `tried to reference undeclared '${candidateId}' ${name}`,
                            `did you mean ${similars[0]}?`,
                            stringDiff.stringifyUnmatchingIndex(candidateId, similars[0], stringDiff.findUnmatchingIndex(candidateId, similars[0])),
                        ].join('\n')
                    }

                    return [
                        `tried to reference undeclared '${candidateId}' ${name}`,
                        `did you mean any of [${similars.join(', ')}]?`,
                    ]
                })()

                context.raise(message, ref)
            }
        }
    }

    const validateTileGroupRef = makeRefValidator(
        new Set(level.get('tile-groups').map(getStringOrId)),
        'tile group',
    )

    const validatePropGroupRef = makeRefValidator(
        new Set(level.get('prop-groups').map(getStringOrId)),
        'prop group',
    )

    const validateDungeonRef = makeRefValidator(
        new Set(level.get('dungeons').map((dungeon) => dungeon.get('id').value())),
        'dungeon',
    )

    const validateChallengeRef = makeRefValidator(
        new Set(level.get('challenges').map((challenge) => challenge.get('id').value())),
        'challenge',
    )

    function validateSidePath (sidePath) {
        sidePath.get('default').pipe((def) => {
            def.get('tile-groups').forEach(validateTileGroupRef)
        })

        sidePath.get('variants').forEach((variant) => {
            variant.get('tile-groups').forEach(validateTileGroupRef)
        })
    }

    function validateStretch (stretch) {
        stretch.get('prop-groups').forEach(validatePropGroupRef)
        stretch.get('tile-groups').forEach(validateTileGroupRef)
        stretch.get('side-paths').pipe(validateSidePath)
    }

    level.get('stretches').forEach(validateStretch)

    level.get('dungeons').forEach((dungeon) => {
        dungeon.get('stretches').forEach(validateStretch)
    })

    level.get('side-paths').pipe(validateSidePath)

    level.get('tiles').forEach((tile) => {
        tile.get('teleports').forEach((teleport) => {
            teleport.get('dungeons').forEach((dungeon) => {
                if (!getStringOrId(dungeon).startsWith("@")) {
                    validateDungeonRef(dungeon)
                }
            })
        })

        tile.get('challenges').forEach(validateChallengeRef)
    })

    level.get('initial-dungeons').forEach((dungeon) => {
        if (!getStringOrId(dungeon).startsWith("@")) {
            validateDungeonRef(dungeon)
        }
    })
}

Object.assign(module.exports, {
    level,
    levelMore,
})