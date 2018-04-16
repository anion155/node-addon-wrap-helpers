import qbs
import Node

StaticLibrary {
    id: nawhLib
    property bool arraysExactSize: true
    property bool exceptionsToJs: true

    Depends { name: 'cpp' }
    cpp.cxxLanguageVersion: 'c++14'
    cpp.commonCompilerFlags: [ '-pedantic' ]
    cpp.includePaths: [ 'include/' ]
    cpp.defines: {
        var defs = [];
        if (arraysExactSize) defs.push('NAWH_OPTION_ARRAY_EXACT_SIZE');
        if (exceptionsToJs) defs.push('NAWH_OPTION_EXCEPTIONS_TO_JS');
        return defs;
    }
    Depends { name: 'Node'; submodules: [ 'Headers', 'Nan' ] }

    files: [
        'include/**/*.hpp',
        'src/**/*.cpp',
    ]

     Export {
         Depends { name: 'cpp' }
//         cpp.systemIncludePaths: [ 'include/' ]
         cpp.includePaths: [ 'include/' ]
         cpp.defines: nawhLib.cpp.defines
     }
}
