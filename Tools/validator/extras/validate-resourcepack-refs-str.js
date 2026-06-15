'use strict'

const fs = require('fs')
const path = require('path')

const basePath = path.join(__dirname, '../../..', 'Content/data/resourcepacks')

function validate (level, context) {
    function validateResourcepackRef (resourcePackNameNode) {
        const resourcePackName = resourcePackNameNode.value()

        if (!fs.existsSync(path.join(basePath, resourcePackName))) {
            context.raise(
                `tried to reference resource pack '${resourcePackName}' which does not exist`,
                resourcePackNameNode,
            )
        }
    }

    level.get('resource-packs').forEach(validateResourcepackRef)

    level.get('dungeons').forEach((dungeon) => {
        dungeon.get('resource-packs').forEach(validateResourcepackRef)
    })
}

Object.assign(module.exports, {
    validate,
})