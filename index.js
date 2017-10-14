const LibXML = require('bindings')('xml').Libxml;
let libxml = new LibXML();
console.log(libxml);
// Wellformed & valid
let test = libxml.load("test/data/test-default.xml");
let testValid = libxml.validateAgainstDtd("test/dtd/mydoctype.dtd");
console.log(test,testValid);
// Get DTD
let dtd = libxml.getDtd();
console.log(dtd);
// Wellformed & invalid
let test2 = libxml.load("test/data/test-not-valid-dtd.xml");
let testValid2 = libxml.validateAgainstDtd("test/dtd/mydoctype.dtd");
console.log(test2,testValid2);

// not wellformed
let test3 = libxml.load("test/data/test-not-wellformed.xml");
let testValid3 = libxml.validateAgainstDtd("test/dtd/mydoctype.dtd");
console.log(test3,testValid3);
module.exports = LibXML;