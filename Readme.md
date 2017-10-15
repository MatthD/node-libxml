Node-LibXML
==========

LibXML2 Node.js Wrapper to Check: Wellformed, Validity against DTD & get doctype

## Requirements: 

Node-Libxml has nos any dependencies, it's fully bundled, so you can just use it as it comes :)

## Use

```javascript
  const Libxml = require('libxml');
  let libxml = new Libxml();

  // load function return true if wellformed, false if not
  let xmlIsWellformed = libxml.load('path/to/xml');

  // load dtd & valid against it return true if valid, false if not, debug option is a boolean to set debug to true or not | default is false
  let xmlIsValid = libxml.validateAgainstDtd('path:to/dtd',debug);

  // getDtd() return an object containing DTD info:  { "name": "article","externalId": "my doctype of doom","systemId": "mydoctype.dtd"}
  // Info: systemID could be a publicId too
  let dtdFile = libxml.getDtd();
```

## Todo:
- get Xpaths
- Travis config
- Build for all os..