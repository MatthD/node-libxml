# Node-LibXML

#### For old node.js versions please use verison < 4.0.0 Already for old LTS node (4,6,8) `npm install node-libxml@3.2.5`;

#### For new node.js versions (10,12,14,16,18,20) we are using n-api so please install `npm install node-libxml@5.0.0`, its compatible with arm proc too for version >= 16 (you wil notice a free memoery message when using free memory function after node >16) 🙂

Node-Libxml is a LibXML2 Node.js Wrapper

It can perform:

- Wellformed check & rerror report
- Validity against DTD/XSD(Schema) check,
- Get doctype informations (about dtd)
- GetXpath Values
- Load XMl from string or file path

## Requirements:

Node-Libxml has nos any dependencies, it's fully bundled, so you can just use it as it comes :)

Works on : Linux, Osx & Windows (no 32bits, **just 64 bits**)

## Why use it

Node-libxml has been thought differently than [libxmljs](https://github.com/libxmljs/libxmljs); whereas libxmljs can be use in commun uses, you can use node-libxml if:

- You wan to validate against DTD (libxmljs can't)
- You want a silent program if xml is not wellformed (node-libxml returns wellformed error in object; libxmljs throws)
- You want to do xml processing in parallel forks
- You want to load XML from file path OR string
- You want to validate against DTD / schema on multiple documents with just ONE dtd/schema loaded in memory (libxmljs loads it on each validation request), so it's clearly by far fastest!

## Install

```bash
  npm i node-libxml
```

## Use

```javascript
  const Libxml = require('node-libxml');
  let libxml = new Libxml();

  let xmlIsWellformed = libxml.loadXml('path/to/xml');
  let xmlIsWellformedStr = libxml.loadXmlFromString('<name>test</name>');

  console.log(xmlIsWellformed);
  console.log(libxml.wellformedErrors);

  console.log(libxml.getDtd());

  libxml.loadDtds(['path/to/dtd1', 'path/to/dtd2']);
  let xmlIsValid = libxml.validateAgainstDtds();
  console.log(xmlIsValid);
  console.log(libxml.validationDtdErrors);

  //Get some xpaths;
  let aRandomPathBoolean = libxml.xpathSelect('boolean(//some/path'));
  let aRandomPathNumber = libxml.xpathSelect('number(//some/path'));
  let countSomeElements = libxml.xpathSelect('count(//some/path'));
  //... & all xpath could do I think
```

check [tests](./test/libxml-test.js) for more examples

## API

### Loading functions

##### loadXml(string)

A function of libxml to load the XML file
`TAKE a path & RETURN true if wellformed | false if not`
`SET a an array 'wellformedErrors' in libxml element containing wellformed errors`

##### loadXmlFromString(string)

A function of libxml to create the xml Dom from a string
`TAKE a string containing xml & RETURN true if wellformed | false if not`
`SET a an array 'wellformedErrors' in libxml element containing wellformed errors`

##### loadDtds(array)

A function of libxml to load the DTDs files
`TAKE an array of path of & RETURN nothing`
`SET a an array 'dtdsLoadedErrors' in libxml element IF error happend on loading dtd(s)`

##### loadSchemas(array)

A function of libxml to load the XSDs files
`TAKE an array of path of & RETURN nothing`
`SET a an array 'schemasLoadedErrors' in libxml element IF error happend on loading xsd(s)`
[ex](./test/libxml-test.js#L157)

### Validating functions

##### validateAgainstDtd()

A function of libxml to validate against the previously loaded DTD(s)
`TAKE nothing & RETURN a string which is the name of the first dtd which has validated`
`RETURN false if no dtd(s) have validate the xml`
`RETURN null if no any dtd have been corectly loaded`
`SET a an array 'validationDtdErrors' in libxml element if no dtd has validate`

##### validateAgainstSchemas()

A function of libxml to validate against the previously loaded DTD(s)
`TAKE nothing & RETURN a string which is the name of the first dtd which has validated`
`RETURN false if no dtd(s) have validate the xml`
`RETURN null if no any schema have been corectly loaded`
`SET a an array 'validationDtdErrors' in libxml element if no dtd has validate`

### Get informations of the the XML

#### getDtd()

A function of libxml to evaluate the xpath on the previously loaded XML
`TAKE nothin & RETURN an object containing name,externalId & systemId`

#### xpathSelect(string)

A function of libxml to evaluate the xpath on the previously loaded XML
`TAKE string & RETURN value depending on what you asked (number, boolean..)`
`RETURN null if no match`
[ex](./test/libxml-test.js#L69)

### Memory management

Use those functions when you have finished jobs of elements, or before overwrite them.
[ex](./test/libxml-test.js#100)

##### freeXml()

A function of libxml to free XML memory file
`TAKE nothing & RETURN nothin`
`FREE memory & clear 'wellformedErrors' property on libxml instance`

##### freeDtds()

A function of libxml to free all the DTD in memory
`TAKE nothing & RETURN nothin`
`FREE memory & clear 'dtdsLoadedErrors' property on libxml instance`
`FREE memory & clear 'validationDtdErrors' property on libxml instance`

##### freeSchemas()

A function of libxml to free all the Schema in memory
`TAKE nothing & RETURN nothin`
`FREE memory & clear 'schemasLoadedErrors' property on libxml instance`
`FREE memory & clear 'validationSchemaErrors' property on libxml instance`

##### clearAll()

A function of libxml to free all the memory taken by libxml2
`TAKE nothing & RETURN nothin`
free all memory in all instance in all fork using by libxml.

## Contribute

### Install

- `npm i`
- `npm test` should pass 😎
- `npm run publish`

## Important notes

Travis & appveyor cannot be used anymore with n-api because auto-publish from node-pre-gyp-github not working anymore + we have now n release for one node version
