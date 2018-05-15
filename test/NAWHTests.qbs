import qbs
import Node

Node.AddonProduct {
    Depends { name: 'cpp' }
    cpp.cxxLanguageVersion: 'c++17'
    Depends { name: 'NAWHLib' }

    files: [
        'module/**/*.hpp',
        'module/**/*.cpp',
    ]
}
