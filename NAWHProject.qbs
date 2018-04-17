import qbs

Project {
    property string nodeQbsSearchPath: 'node_modules/node-qbs/qbs'
    qbsSearchPaths: [ nodeQbsSearchPath ]
    references: [
        'NAWHLib/NAWHLib.qbs',
        'test/NAWHTests.qbs',
    ]
}
