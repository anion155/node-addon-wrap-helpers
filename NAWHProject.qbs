import qbs

Project {
    qbsSearchPaths: [
        'node_modules/node-qbs/qbs/',
    ]

    references: [
        'NAWHLib/NAWHLib.qbs',
        'tests/NAWHTests.qbs',
    ]
}
