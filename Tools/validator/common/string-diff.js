'use strict'

function findUnmatchingIndex (a, b) {
    const lengthMax = Math.max(a.length, b.length)

    for (let i = 0; i < lengthMax; i++) {
        if (a[i] !== b[i]) {
            return i
        }
    }

    return a.length === b.length
        ? -1
        : Math.min(a.length, b.length)
}

function stringifyUnmatchingIndex (a, b, index) {
    const lengthMax = Math.max(a.length, b.length)

    const diffString = index !== -1
        ? `${'-'.repeat(index)}^${'-'.repeat(lengthMax - index - 1)}`
        : '-'.repeat(lengthMax)

    return [a, b, diffString].join('\n')
}

Object.assign(module.exports, {
    findUnmatchingIndex,
    stringifyUnmatchingIndex,
})