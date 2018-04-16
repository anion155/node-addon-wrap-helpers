import qbs
import Node

Node.AddonProduct {
    cpp.cxxLanguageVersion: "c++14"
    Depends { name: 'NAWHLib' }

    files: [
        'module/**/*.hpp',
        'module/**/*.cpp',
    ]
}
