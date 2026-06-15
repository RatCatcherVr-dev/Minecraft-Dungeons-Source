'use strict'

function stringifyLoc (loc) {
    if (loc == null) {
        return 'location unknown; this most likely an error (ASK ADRIAN/ARON)'
    }

    {
        const parts = []

        if (loc.filePath != null) {
            parts.push(loc.filePath.toLowerCase())
        }

        if (loc.start != null) {
            parts.push(`line ${loc.start.line}`)
        }

        if (parts.length > 0) {
            return parts.join(', ')
        }
    }

    {
        const parts = []

        if (loc.objectGroup != null) {
            parts.push(loc.objectGroup.toLowerCase())
        }

        if (loc.coords != null) {
            parts.push(`coords [${loc.coords.x}, ${loc.coords.y}, ${loc.coords.z}]`)
        }

        if (parts.length > 0) {
            return parts.join(', ')
        }
    }

    return 'location unknown; this most likely an error (ASK ADRIAN/ARON)'
}

function stringifyPath (node) {
    return node == null || node.key == null
        ? ''
        : `${stringifyPath(node.parent)}[${node.key}]`
}

function stringifyExtras (extras) {
    if (extras == null) {
        return '???'
    }

    return extras.locDuplicate != null
        ? `${stringifyLoc(extras.loc)} and ${stringifyLoc(extras.locDuplicate)}`
        : `${stringifyLoc(extras.loc)} ${stringifyPath(extras)}`
}

function bucketize (issues) {
	const buckets = new Map

	issues.forEach(({ message, extras }) => {
		const location = stringifyExtras(extras)

		if (buckets.has(message)) {
			buckets.get(message).add(location)
		} else {
			buckets.set(message, new Set([location]))
		}
	})

	return buckets
}

function stringifyIssues (issues) {
	const buckets = bucketize(issues)

	const lines = ['']
	buckets.forEach((locations, message) => {
		lines.push(message)

		locations.forEach((location) => {
			lines.push(`    ${location}`)
		})

		lines.push('')
	})

	return lines.join('\n')
}

Object.assign(module.exports, {
    stringifyLoc,
    stringifyExtras,
    stringifyIssues,
})