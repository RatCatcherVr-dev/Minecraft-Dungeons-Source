'use strict'

const { and, or } = require('./core')
const { ast } = require('./core-str')
const { locator, localizationKey, assetPath } = require('./common-str')

const reward = ast.shapeOnly({
    'region': ast.optional(locator()),
    'object': ast.optional(assetPath()),
})

const wave = or([
    ast.shapeOnly({
        'count': ast.number(),
        'groups': ast.every(ast.string()),
        'spawn-at-region': ast.optional(ast.string()),
        'show-spawn-indicator': ast.optional(ast.boolean()),
    }),
    ast.tuple([
        ast.number(),
        ast.string(),
    ]),
], 'wave must be a [number, string] pair or a { count, groups, ... }')

const mobActivation = ast.string

const gate = or([
    ast.nil(),
    ast.shapeOnly({
        'object': ast.optional(assetPath({ isResizable: true })),
        'tile-exits': ast.optional(ast.boolean()),
        'gates': ast.optional(ast.boolean()),
        'regions': ast.optional(ast.every(locator())),
        'start-unlocked': ast.optional(ast.boolean()),
    })
], 'gate must be null or a { object, tile-exits, gates, ... }')

const killGroups = ast.shapeOnly({
    'mobs': wave,
    'stretch': ast.optional(ast.string()),
    'spawn-regions': ast.optional(ast.every(locator())),
    'marker-region': ast.optional(locator()),
    'reward': or([
        ast.string(),
        ast.tuple([ast.number(), ast.string()]),
    ], 'reward must be a string or a [number, string] pair'),
    'gate': ast.optional(gate),
})

const gauntlet = and(
    ast.shapeOnly({
        'end-region': locator(),
        'end-marker': ast.optional(ast.boolean()),
        'marker-region': ast.optional(locator()),
        'gate': ast.optional(gate),
    }),
    ast.propsXor(['end-marker', 'marker-region'])
)

const arena = ast.shapeOnly({
    'start-time': ast.optional(ast.number()),
    'wave-start': ast.optional(ast.boolean()),
    'has-music': ast.optional(ast.boolean()),
    'interval': ast.optional(ast.number()),
    'rest-interval': ast.optional(ast.number()),
    'prespawn-mob': ast.optional(or([ast.values(['']), assetPath()], 'must be either "" or a valid asset path')),
    'prespawn-mob-duration': ast.optional(ast.number()),
    'stretch': ast.optional(ast.string()),
    'battle-max-duration': ast.optional(ast.number()),
    'spawn-effects-pack': ast.optional(ast.string()),
    'waves': ast.every(wave),
    'gate': ast.optional(gate),
    'is-boss': ast.optional(ast.boolean()),
    'mob-activations': ast.optional(ast.every(mobActivation())),
    'end-delay': ast.optional(ast.number()),
})

const click = ast.shapeOnly({
    'object': assetPath(),
    'dummy-object': ast.optional(assetPath()),
    'count': ast.number(),
    'sequential': ast.optional(ast.boolean()),
    'locations': ast.every(locator()),
    'locked-doors': ast.optional(ast.every(locator())),
    'key-locations': ast.optional(ast.every(locator())),
    'key-type': ast.optional(ast.string()),
    'mobs': ast.optional(wave),
    'mob-stretch': ast.optional(ast.string()),
    'spawn-regions': ast.optional(ast.every(locator())),
    'door-path': ast.optional(assetPath({ isResizable: true })),
})

const baseObjective = {
    'gauntlet': ast.optional(gauntlet),
    'arena': ast.optional(arena),
    'click': ast.optional(click),
    'killgroup': ast.optional(killGroups),
    'id': ast.optional(ast.string()),
    'consider-tiles': ast.optional(ast.string()),
    'meta': ast.optional(ast.every(ast.string())),
    'panToObjectiveStart': ast.optional(ast.boolean()),
    'panToObjectiveEnd': ast.optional(ast.boolean()),
    'panToDuration': ast.optional(ast.number()),
    'objective-indicator-force': ast.optional(ast.boolean()),

    // this should not exist
    'sound-narrator': ast.optional(ast.shapeOnly({
        'object': ast.string(),
    })),

    'sound': ast.optional(ast.shapeOnly({
        'track': ast.optional(ast.shapeOnly({
            'object': ast.string(),
        })),
        'completed-track': ast.optional(ast.shapeOnly({
            'object': ast.string(),
        })),
        'narrator': ast.optional(ast.shapeOnly({
            'object': ast.string(),
        })),
    }))
}

const objective = and(
    ast.shapeOnly(Object.assign({}, baseObjective, {
        'name': localizationKey(),
        'description': localizationKey(),
        'displayMode': ast.optional(ast.string()),
        'optional': ast.optional(ast.boolean()),
    })),
    ast.propsXor(['gauntlet', 'arena', 'click', 'killgroup']),
)

const challenge = and(
    ast.shapeOnly(Object.assign({}, baseObjective, {
        'trigger': ast.optional(ast.string()),
        'trigger-object': ast.optional(assetPath()),
        'reward' : ast.optional(reward),
    })),
    ast.propsXor(['gauntlet', 'arena', 'click', 'killgroup']),
    ast.propsXor(['trigger', 'trigger-object']),
)

Object.assign(module.exports, {
    objective,
    challenge,
})
