(() => {
    'use strict'

    function extrude ({ data, width, height }) {
        const points = new Map

        const verts = new Map
        let vertsIndex = 0

        const texs = new Map
        let texsIndex = 0

        const normals = []

        const faces = []

        function addVert (x, y, z) {
            const key = `${x}-${y}-${z}`

            if (!verts.has(key)) {
                verts.set(key, {
                    index: vertsIndex + 1,
                    x,
                    y,
                    z,
                })

                vertsIndex++
            }
        }

        function addTex (x, y) {
            const ninety = {
                x: (.5 / width) * .99,
                y: (.5 / height) * .99,
            }

            const center = {
                x: x / width + .5 / width,
                y: y / height - .5 / height,
            }

            {
                const key = `${x}-${y}-0`

                if (!texs.has(key)) {
                    texs.set(key, {
                        index: texsIndex + 1,
                        x: center.x - ninety.x,
                        y: center.y - ninety.y,
                    })

                    texsIndex++
                }
            }

            {
                const key = `${x}-${y}-1`

                if (!texs.has(key)) {
                    texs.set(key, {
                        index: texsIndex + 1,
                        x: center.x - ninety.x,
                        y: center.y + ninety.y,
                    })

                    texsIndex++
                }
            }

            {
                const key = `${x}-${y}-2`

                if (!texs.has(key)) {
                    texs.set(key, {
                        index: texsIndex + 1,
                        x: center.x + ninety.x,
                        y: center.y + ninety.y,
                    })

                    texsIndex++
                }
            }

            {
                const key = `${x}-${y}-3`

                if (!texs.has(key)) {
                    texs.set(key, {
                        index: texsIndex + 1,
                        x: center.x + ninety.x,
                        y: center.y - ninety.y,
                    })

                    texsIndex++
                }
            }
        }

        function addNormals () {
            normals.push(
                { x: 0, y: 0, z: 1 },
                { x: -1, y: 0, z: 0 },
                { x: 0, y: 0, z: -1 },
                { x: 1, y: 0, z: 0 },
                { x: 0, y: -1, z: 0 },
                { x: 0, y: 1, z: 0 },
            )
        }

        function addFaces () {
            points.forEach(({ x, y }) => {
                const texIndicesCw = [
                    texs.get(`${x}-${y}-0`).index,
                    texs.get(`${x}-${y}-1`).index,
                    texs.get(`${x}-${y}-2`).index,
                    texs.get(`${x}-${y}-3`).index,
                ]

                const texIndicesCcw = [
                    texIndicesCw[0],
                    texIndicesCw[3],
                    texIndicesCw[2],
                    texIndicesCw[1],
                ]

                faces.push({
                    // cw
                    vertIndices: [
                        verts.get(`${x + 0}-${y + 0}-0`).index,
                        verts.get(`${x + 0}-${y + 1}-0`).index,
                        verts.get(`${x + 1}-${y + 1}-0`).index,
                        verts.get(`${x + 1}-${y + 0}-0`).index,
                    ],
                    texIndices: texIndicesCw,
                    normalIndices: [3, 3, 3, 3],
                })

                faces.push({
                    // ccw
                    vertIndices: [
                        verts.get(`${x + 0}-${y + 0}-1`).index,
                        verts.get(`${x + 1}-${y + 0}-1`).index,
                        verts.get(`${x + 1}-${y + 1}-1`).index,
                        verts.get(`${x + 0}-${y + 1}-1`).index,
                    ],
                    texIndices: texIndicesCcw,
                    normalIndices: [1, 1, 1, 1],
                })

                if (!points.has(`${x}-${y - 1}`)) {
                    // ccw
                    faces.push({
                        vertIndices: [
                            verts.get(`${x + 0}-${y + 0}-0`).index,
                            verts.get(`${x + 1}-${y + 0}-0`).index,
                            verts.get(`${x + 1}-${y + 0}-1`).index,
                            verts.get(`${x + 0}-${y + 0}-1`).index,
                        ],
                        texIndices: texIndicesCcw,
                        normalIndices: [5, 5, 5, 5],
                    })
                }

                if (!points.has(`${x - 1}-${y}`)) {
                    // ccw
                    faces.push({
                        vertIndices: [
                            verts.get(`${x + 0}-${y + 0}-0`).index,
                            verts.get(`${x + 0}-${y + 0}-1`).index,
                            verts.get(`${x + 0}-${y + 1}-1`).index,
                            verts.get(`${x + 0}-${y + 1}-0`).index,
                        ],
                        texIndices: texIndicesCcw,
                        normalIndices: [2, 2, 2, 2],
                    })
                }

                if (!points.has(`${x}-${y + 1}`)) {
                    // cw
                    faces.push({
                        vertIndices: [
                            verts.get(`${x + 0}-${y + 1}-0`).index,
                            verts.get(`${x + 0}-${y + 1}-1`).index,
                            verts.get(`${x + 1}-${y + 1}-1`).index,
                            verts.get(`${x + 1}-${y + 1}-0`).index,
                        ],
                        texIndices: texIndicesCw,
                        normalIndices: [6, 6, 6, 6],
                    })
                }

                if (!points.has(`${x + 1}-${y}`)) {
                    // cw
                    faces.push({
                        vertIndices: [
                            verts.get(`${x + 1}-${y + 0}-0`).index,
                            verts.get(`${x + 1}-${y + 1}-0`).index,
                            verts.get(`${x + 1}-${y + 1}-1`).index,
                            verts.get(`${x + 1}-${y + 0}-1`).index,
                        ],
                        texIndices: texIndicesCw,
                        normalIndices: [4, 4, 4, 4],
                    })
                }
            })
        }

        function stringify (verts, faces, width, height) {
            const vertsOffset = (() => {
                const vertsRange = {
                    min: {
                        x: Infinity,
                        y: Infinity,
                        z: Infinity,
                    },
                    max: {
                        x: 0,
                        y: 0,
                        z: 0,
                    },
                }

                verts.forEach(({ x, y, z }) => {
                    vertsRange.min.x = Math.min(x, vertsRange.min.x)
                    vertsRange.min.y = Math.min(y, vertsRange.min.y)
                    vertsRange.min.z = Math.min(z, vertsRange.min.z)

                    vertsRange.max.x = Math.max(x, vertsRange.max.x)
                    vertsRange.max.y = Math.max(y, vertsRange.max.y)
                    vertsRange.max.z = Math.max(z, vertsRange.max.z)
                })

                return {
                    x: (vertsRange.max.x + vertsRange.min.x) / 2,
                    y: (vertsRange.max.y + vertsRange.min.y) / 2,
                    z: (vertsRange.max.z + vertsRange.min.z) / 2,
                }
            })()

            const vertsString = [...verts.values()].map(({ x, y, z }) => `v ${x - vertsOffset.x} ${y - vertsOffset.y} ${z - vertsOffset.z}`).join('\n')
            const texsString = [...texs.values()].map(({ x, y }) => `vt ${x} ${y + 1 / height}`).join('\n')
            // sort by index!
            // values is not guaranteed to be in order

            const normalsString = normals.map(({ x, y, z }) => `vn ${x} ${y} ${z}`).join('\n')

            const facesString = faces.map(({ vertIndices, texIndices, normalIndices }) =>
                `f ${vertIndices[0]}/${texIndices[0]}/${normalIndices[0]} ${vertIndices[1]}/${texIndices[1]}/${normalIndices[1]} ${vertIndices[2]}/${texIndices[2]}/${normalIndices[2]} ${vertIndices[3]}/${texIndices[3]}/${normalIndices[3]}`
            ).join('\n')

            return `\ng default\n# verts\n${vertsString}\n\n# texs\n${texsString}\n\n# normals\n${normalsString}\n\n# faces\n${facesString}`
        }

        return (() => {
            for (let y = 0; y < height; y++) {
                for (let x = 0; x < width; x++) {
                    const index = (width * (height - y - 1) + x) << 2

                    const alpha = data[index + 3]

                    if (alpha > 0) {
                        points.set(`${x}-${y}`, { x, y })

                        addVert(x + 0, y + 0, 0)
                        addVert(x + 0, y + 1, 0)
                        addVert(x + 1, y + 1, 0)
                        addVert(x + 1, y + 0, 0)

                        addVert(x + 0, y + 0, 1)
                        addVert(x + 0, y + 1, 1)
                        addVert(x + 1, y + 1, 1)
                        addVert(x + 1, y + 0, 1)

                        addTex(x, y)
                    }
                }
            }

            addNormals()
            addFaces()
            return stringify(verts, faces, width, height)
        })()
    }

    Object.assign(typeof art !== 'undefined' ? art : module.exports, {
        extrude,
    })
})()