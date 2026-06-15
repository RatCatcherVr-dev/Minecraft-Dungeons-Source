(() => {
    'use strict'

    const { makeContext } = validator

    const editor = setupEditor('editor-in')
    const status = setupStatus('status')

    function setupEditor (elementId) {
        const editor = ace.edit(elementId)
        editor.getSession().setMode('ace/mode/json')
        editor.getSession().setUseWrapMode(true)
        editor.getSession().setOption('useWorker', false)
        editor.setTheme('ace/theme/monokai')
        editor.setFontSize(18)
        editor.setValue('...')
        editor.on('input', validate)

        return editor
    }

    const state = {
        currentFile: {
            name: null,
            element: null,
        },
        loading: 0,
        annotations: [],
    }

    function setupStatus (elementId) {
        const status = document.getElementById(elementId)
        status.addEventListener('click', () => {
            if (state.annotations.length <= 0) {
                return
            }

            const middle = Math.ceil((editor.getFirstVisibleRow() + editor.getLastVisibleRow()) / 2)
            const nextAnnotation = state.annotations.find(({ row }) => row > middle)

            if (nextAnnotation == null) {
                editor.scrollToLine(state.annotations[0].row, true, true, () => {})
            } else {
                editor.scrollToLine(nextAnnotation.row, true, true, () => {})
            }
        })

        return {
            ok: () => {
                status.textContent = 'No errors found'
                status.classList.add('ok')
                status.classList.remove('error')
                status.classList.remove('warning')
            },
            error: () => {
                const errorCount = state.annotations.filter(({ type }) => type === 'error').length

                status.classList.remove('ok')
                if (errorCount > 0) {
                    status.textContent = `Errors: ${errorCount} Warnings: ${state.annotations.length - errorCount}`
                    status.classList.add('error')
                } else {
                    status.textContent = `Warnings: ${state.annotations.length - errorCount}`
                    status.classList.add('warning')
                }
            },
            exception: () => {
                status.classList.remove('ok')
                status.classList.add('error')
                status.textContent = 'Parse error'
            },
        }
    }

    function collectOutline (ast) {
        if (ast.type === 'ArrayExpression') {
            return ast.elements.map(({ properties }) => {
                const property = properties.find(({ key }) => key.value === 'id')

                return {
                    name: property.value.value,
                    line: property.key.loc.start.line,
                }
            })
        } else if (ast.type === 'ObjectExpression') {
            return ast.properties.map(({ key }) => ({
                name: key.value,
                line: key.loc.start.line,
            }))
        } else {
            return []
        }
    }

    function parse (source) {
        return esprima.parse(`(${source})`, { loc: true })
    }

    function extractJson (program) {
        return program.body[0].expression
    }

    function validate () {
        try {
            const ast = extractJson(parse(editor.getValue()))

            const context = makeContext()
            if (ast.type === 'ArrayExpression') {
                validator.mobGroups(ast, context)
            } else if (ast.type === 'ObjectExpression') {
                validator.level(ast, context)

                if (context.isEmpty()) {
                    const data = validator.convert(ast)
                    validator.levelMore(data, context)
                }
            }

            if (context.isEmpty()) {
                state.annotations = []
                editor.getSession().setAnnotations(state.annotations)
                status.ok()
            } else {
                state.annotations = context.getIssues().map(({ message, extras, type }) => ({
                    row: extras.loc.start.line - 1,
                    text: message,
                    type,
                }))
                editor.getSession().setAnnotations(state.annotations)
                status.error()
            }

            setOutline(collectOutline(ast))
        } catch (ex) {
            state.annotations = [{
                row: ex.lineNumber - 1,
                text: ex.description,
                type: 'error',
            }]
            editor.getSession().setAnnotations(state.annotations)
            status.exception()
            console.warn(ex)
        }
    }

    function setFiles (files) {
        // clean up
        const filesList = document.getElementById('files')

        while (filesList.firstChild) {
            filesList.removeChild(filesList.firstChild)
        }

        files.forEach((file) => {
            const fileElement = document.createElement('li')

            fileElement.textContent = file

            fileElement.addEventListener('click', () => {
                if (state.loading > 0) {
                    return
                }

                state.loading++

                state.currentFile.element.classList.remove('selected')

                fetch(`${location.protocol}//${location.hostname}:8007/${state.currentFile.name}`, {
                    method: 'post',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: editor.getValue(),
                })

                editor.setValue('"... loading ..."', -1)

                state.currentFile.name = file
                state.currentFile.element = fileElement
                state.currentFile.element.classList.add('selected')

                fetch(`${location.protocol}//${location.hostname}:8007/${state.currentFile.name}`)
                    .then((response) => response.text())
                    .then((source) => {
                        editor.setValue(source, -1)
                        state.loading--
                    })
            })

            filesList.appendChild(fileElement)
        })

        // first file
        state.loading++

        fetch(`${location.protocol}//${location.hostname}:8007/${files[0]}`)
            .then((response) => response.text())
            .then((source) => {
                editor.setValue(source, -1)
                state.loading--
            })

        state.currentFile.name = files[0]
        state.currentFile.element = filesList.firstChild
        state.currentFile.element.classList.add('selected')
    }

    function setOutline (entries) {
        // clean up
        const outlineList = document.getElementById('outline')

        while (outlineList.firstChild) {
            outlineList.removeChild(outlineList.firstChild)
        }

        entries.forEach(({ name, line }) => {
            const outlineElement = document.createElement('li')

            outlineElement.textContent = name

            outlineElement.addEventListener('click', () => {
                if (state.loading > 0) {
                    return
                }

                editor.scrollToLine(line, true, true, () => {})
            })

            outlineList.appendChild(outlineElement)
        })
    }

    function fetchIndex () {
        state.loading++
        fetch(`${location.protocol}//${location.hostname}:8007`)
            .then((response) => response.json())
            .then((files) => {
                state.loading--
                setFiles(files)
            })
    }

    fetchIndex()
})()