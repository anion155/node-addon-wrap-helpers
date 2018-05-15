import qbs
import Node

Node.AddonProduct {
    Depends { name: 'cpp' }
    cpp.cxxLanguageVersion: 'c++14'
    Depends { name: 'NAWHLib' }

    files: [
        'module/**/*.hpp',
        'module/**/*.cpp',
    ]
}
