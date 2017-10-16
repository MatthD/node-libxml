{
  'targets': [
    {
      'target_name': 'xml',
      'product_extension': 'node',
      'type': 'shared_library',
      'include_dirs': [
        'vendor/libxml/include',
        "<!(node -e \"require('nan')\")"
      ],
      'cflags': [ '-Wall' ],
      'xcode_settings': {
        'OTHER_CFLAGS': [ '-Wall' ]
      },
      'sources': [
        'libxml.cpp',
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
          'xcode_settings': {
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
