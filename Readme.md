# Node-LibXML

### Update July 2025: Works on node v22/v24 👌🏻 

#### For new node.js versions (10,12,14,16,18,20) we are using n-api so please install `npm install node-libxml@5.0.0` at minimum, its compatible with arm proc too for version >= 16 (you wil notice a free memory message when using free memory function after node >16) 🙂


#### For old node.js versions please use version < 4.0.0 Already for old LTS node (4,6,8) `npm install node-libxml@3.2.5`;

_Node-Libxml_ is a _[libxml2]_ Node.js Wrapper

It can perform:

- Well-formed check & error report
- Validity against DTD/XSD(Schema) check,
- Get doctype information (about DTD)
- Get XPath Values
- Load XMl from string or file path

## Requirements:

Node-Libxml has no dependencies; it's fully bundled, so you can just use it as it comes :)

Works on: Linux, macOS & Windows (no 32bits, **just 64 bits**)

## Why use it

_Node-LibXML_ has been thought differently than _[libxmljs]_; whereas _libxmljs_ can be use in common uses, you can use _node-libxml_ if:

- You want to validate against DTD (_libxmljs_ can't)
- You want a silent program if XML is not well-formed (_node-libxml_ returns well-formed error in object; _libxmljs_ throws)
- You want to do XML processing in parallel forks
- You want to load XML from file path *or* string
- You want to validate against DTD / schema on multiple documents with just *one* DTD/schema loaded in memory (_libxmljs_ loads it on each validation request), so it's clearly by far fastest!

## Install

```bash
  npm i node-libxml
```

## Use

```javascript
  const Libxml = require('node-libxml').Libxml; // or import {Libxml} from 'node-libxml'
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

See our [tests](tests/libxml-test.spec.ts) for more examples.

## API

### Loading functions

##### `loadXml(string)`

A function of _libxml2_ to load the XML file
* TAKE a path & RETURN `true` if well-formed | `false` if not
* SET an array `wellformedErrors` in _libxml2_ element containing well-formed errors

##### `loadXmlFromString(string)`

A function of _libxml2_ to create the XML DOM from a string
* TAKE a string containing XML & RETURN `true` if well-formed | `false` if not
* SET an array `wellformedErrors` in _libxml2_ element containing well-formed errors

##### `loadDtds(array)`

A function of _libxml2_ to load the DTD(s) file(s)
* TAKE an array of path of & RETURN nothing
* SET an array `dtdsLoadedErrors` in _libxml2_ element IF error happened on loading DTD(s)

##### `loadSchemas(array)`

A function of _libxml2_ to load the XSD(s) file(s)  
[ex](tests/libxml-test.spec.ts#L206)
* TAKE an array of path of & RETURN nothing
* SET an array `schemasLoadedErrors` in _libxml2_ element IF error happened on loading XSD(s)


### Validating functions

##### `validateAgainstDtd()`

A function of _libxml2_ to validate against the previously loaded DTD(s)
* TAKE nothing & RETURN a string which is the name of the first DTD which has validated
* RETURN `false` if no DTD(s) have validated the XML
* RETURN `null` if not any DTD have been correctly loaded
* SET an array `validationDtdErrors` in _libxml2_ element if no DTD has validate

##### `validateAgainstSchemas()`

A function of _libxml2_ to validate against the previously loaded DTD(s)
* TAKE nothing & RETURN a string which is the name of the first DTD which has validated
* RETURN `false` if no DTD(s) have validated the XML
* RETURN `null` if no any schema have been correctly loaded
* SET an array `validationDtdErrors` in _libxml2_ element if no DTD has validated

### Get information of the XML

#### `getDtd()`

A function of _libxml2_ to evaluate the xpath on the previously loaded XML
* TAKE nothing & RETURN an object containing name, externalId & systemId

#### `xpathSelect(string)`

A function of _libxml2_ to evaluate the xpath on the previously loaded XML  
[ex](tests/libxml-test.spec.ts#L109)
* TAKE string & RETURN value depending on what you asked (number, boolean,...)
* RETURN `null` if no match

### Memory management

Use those functions when you have finished jobs of elements, or before overwrite them.  
[ex](tests/libxml-test.spec.tss#138)

##### `freeXml()`

A function of _libxml2_ to free XML memory file
* TAKE nothing & RETURN nothing
* FREE memory & clear `wellformedErrors` property on _libxml2_ instance

##### `freeDtds()`

A function of _libxml2_ to free all the DTD(s) in memory
* TAKE nothing & RETURN nothing
* FREE memory & clear `dtdsLoadedErrors` property on _libxml2_ instance
* FREE memory & clear `validationDtdErrors` property on _libxml2_ instance

##### `freeSchemas()`

A function of _libxml2_ to free all the Schema in memory
* TAKE nothing & RETURN nothing
* FREE memory & clear 'schemasLoadedErrors' property on _libxml2_ instance
* FREE memory & clear 'validationSchemaErrors' property on _libxml2_ instance

##### `clearAll()`

A function of _libxml2_ to free all the memory taken by _libxml2_
* TAKE nothing & RETURN nothing
* free all memory in all instance in all fork using by _libxml2_

## Contribute

### Install

- `npm i`
- `npm test` should pass 😎
- `npm run publish`

## Important notes

Travis & appveyor cannot be used anymore with n-api because auto-publish from node-pre-gyp-github not working anymore + we have now n release for one node version


[libxml2]: https://gitlab.gnome.org/GNOME/libxml2
[libxmljs]: https://github.com/libxmljs/libxmljs
