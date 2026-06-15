
'use strict'

const { and, or } = require('./core')
const { ast } = require('./core-str')

function minMax (minPropertyName, maxPropertyName) {
    return ast.extractObject((object, context) => {
        const min = object[minPropertyName]
        const max = object[maxPropertyName]

        if (
            min != null &&
            max != null &&
            min.value > max.value
        ) {
            context.raise(minPropertyName+' must be <= '+maxPropertyName, min)
        }
    })
}

const mobGroupEntry = or([
    ast.string(),
    and(
        ast.shapeOnly({
            'type': ast.string('type must be a string'),
            'min-difficulty': ast.optional(ast.number()),
            'max-difficulty': ast.optional(ast.number()),
            'min': ast.optional(ast.number('min must be a string')),
            'max': ast.optional(ast.number('max must be a string')),
            'max%': ast.optional(ast.number()),
            'weight': ast.optional(ast.number('weight must be a string')),
        }),
        minMax('min', 'max'),
        minMax('min-difficulty', 'max-difficulty'),
    )
], 'mob group entry must be an id or a { type, min, max, weight, ... } entry')

const mobGroupEntries = and(
    ast.array('mob group entries must have type arrays'),
    ast.size('mob group entries must have at least one element', { min: 1 }),
    ast.every(mobGroupEntry),
);

const minMaxPair = and(
    ast.every(ast.number()),
    ast.size('must have 2 elements', { min: 2, max: 2 }),
    ast.extractArray((elements, context) => {
        if (elements[0].value > elements[1].value) {
            context.raise('min must be <= max', elements[0])
        }
    }),
)

const mobGroup = and(
    ast.shapeOnly({
        'id': ast.string('mob group must have string ids'),
        'min-difficulty' : ast.optional(ast.number()),
        'max-difficulty' : ast.optional(ast.number()),
        'types': mobGroupEntries,
        'pick-types': ast.optional(or([
            ast.number(),
            minMaxPair,
        ], 'pick-types must be a number or a [min, max] pair')),
    }),
    minMax('min-difficulty', 'max-difficulty')
)

const mobGroups = and(
    ast.array('mob groups must be an array'),
    ast.every(mobGroup),
    ast.uniqueIds('mob groups must have unique ids', ast.util.getId),
)

Object.assign(module.exports, {
    mobGroups,
    mobGroupEntries,
    minMaxPair,
})
