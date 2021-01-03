{
  'targets': [
    {
      "target_name": "<(module_name)",
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': { 'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      },
      'product_dir': '<(module_path)',
      'include_dirs': [
        '<!@(node -p "require(\'node-addon-api\').include")',
        './vendor/libxml/include',
      ],
      'cflags': [ '-Wall' ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        'OTHER_CFLAGS': [ '-Wall' ]
      },
      'defines': [
        'NAPI_VERSION=<(napi_build_version)',
      ],
      'sources': [
        'libxml.cpp',
        'libxml-syntax-error.cpp',
        'vendor/libxml/buf.c',
        'vendor/libxml/catalog.c',
        'vendor/libxml/chvalid.c',
        'vendor/libxml/dict.c',
        'vendor/libxml/encoding.c',
        'vendor/libxml/entities.c',
        'vendor/libxml/error.c',
        'vendor/libxml/globals.c',
        'vendor/libxml/hash.c',
        'vendor/libxml/HTMLparser.c',
        'vendor/libxml/HTMLtree.c',
        'vendor/libxml/legacy.c',
        'vendor/libxml/list.c',
        'vendor/libxml/parser.c',
        'vendor/libxml/parserInternals.c',
        'vendor/libxml/pattern.c',
        'vendor/libxml/relaxng.c',
        'vendor/libxml/SAX2.c',
        'vendor/libxml/SAX.c',
        'vendor/libxml/tree.c',
        'vendor/libxml/threads.c',
        'vendor/libxml/uri.c',
        'vendor/libxml/valid.c',
        'vendor/libxml/xinclude.c',
        'vendor/libxml/xlink.c',
        'vendor/libxml/xmlIO.c',
        'vendor/libxml/xmlmemory.c',
        'vendor/libxml/xmlmodule.c',
        'vendor/libxml/xmlreader.c',
        'vendor/libxml/xmlregexp.c',
        'vendor/libxml/xmlsave.c',
        'vendor/libxml/xmlschemas.c',
        'vendor/libxml/xmlschemastypes.c',
        'vendor/libxml/xmlstring.c',
        'vendor/libxml/xmlunicode.c',
        'vendor/libxml/xmlwriter.c',
        'vendor/libxml/xpath.c',
        'vendor/libxml/xpointer.c'
      ],
      'conditions': [
        ['OS=="mac"', {
          'cflags+': ['-fvisibility=hidden'],
          'xcode_settings': {
            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
            'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
            'OTHER_LDFLAGS': [
              '-undefined dynamic_lookup'
            ],
          },
        }]
      ]
    }
  ]
}
