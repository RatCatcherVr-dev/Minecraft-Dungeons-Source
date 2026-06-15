'use strict'

const splicer = require('./common/splicer/splice-level-generic')
const { methods: strMethods } = require('./common/splicer/methods/str')
const { makeContext } = require('./common/validators/core')
const { fixPath, isHyperMission } = require('./common/utils')

const validators = {
    ...require('./common/validators/level'),
    objectGroup: require('./extras/validate-tile-refs-str').validate,
    resourcePacks: require('./extras/validate-resourcepack-refs-str').validate,
    ...require('./common/validators/hyper-mission'),
}

const readAndParse = {
    levelPart: require('./common/readers/level-part').str,
    objectGroup: require('./common/readers/object-group').str,
}

const { stringifyIssues } = require('./common/stringify-loc')

function validateLevel (levelFile) {
    let str

    try {
        str = splicer.run(strMethods, readAndParse, levelFile)
    } catch (ex) {
        if (ex.description != null) {
            return [{
                type: 'error',
                message: ex.description,
                extras: {
                    loc: {
                        start: { line: ex.lineNumber - 1 },
                        filePath: levelFile,
                    },
                },
            }]
        } else {
            return [{
                type: 'error',
                message: ex.message,
                extras: ex.extras,
            }]
        }
    }

    const context = makeContext()

    validators.level(str, context)

    if (context.isEmpty()) {
        validators.levelMore(str, context)
    }

    if (context.isEmpty()) {
        validators.objectGroup(str, context)
    }

    if (context.isEmpty()) {
        validators.resourcePacks(str, context)
    }

    return context.getIssues()
}

function validateHyperMission (hyperMissionFile) {
    let str

    try {
        str = readAndParse.levelPart(hyperMissionFile)
    } catch (ex) {
        return [{
            type: 'error',
            message: ex.message,
            extras: ex.extras,
        }]
    }

    const context = makeContext()

    validators.hyperMission(str, context)

    if (context.isEmpty()) {
        validators.hyperMissionMore(str, context)
    }

    return context.getIssues()
}

function validate (file) {
    return (isHyperMission(file) ? validateHyperMission : validateLevel)(file)
}

function run (file) {
    const issues = validate(file)
    const issuesString = stringifyIssues(issues)

    process.stdout.write(issuesString)
    process.exit(issues.length > 0 ? 1 : 0)
}

if (require.main === module) {
    if (process.argv.length !== 3) {
        console.error(`use: cli.js <levelname>|<hypermission>`)
        process.exit(1)
    }

    run(fixPath(process.argv[2]))
} else {
    Object.assign(module.exports, {
        validate,
        isHyperMission,
    })
}