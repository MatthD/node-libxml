'use strict';

const Libxml = require('../index.js');
const expect = require('chai').expect;

describe('Node-Libxml', function () {
  it('Should return a list of errored path if dtd path is bad', function () {
    let libxml = new Libxml();
    libxml.loadDtds(['test/dtd/mydoctype-not-existing.dtd']);
    expect(libxml).to.have.a.property('dtdsLoadedErrors');
    expect(libxml.dtdsLoadedErrors).to.be.a('array');
    expect(libxml.dtdsLoadedErrors).to.include('test/dtd/mydoctype-not-existing.dtd');
    libxml.freeXml();
    libxml.freeDtds();
  });
  // Wellformed & valid
  it('Should return wellformed & valid on a wellformed & valid xml', function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml('test/data/test-default.xml');
    libxml.loadDtds(['test/dtd/mydoctype.dtd']);
    let testDefaultV = libxml.validateAgainstDtds();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.a('string');
    libxml.freeXml();
    libxml.freeDtds();
  });
  it('should return wellformed on a wellformed XMl in utf8-bom & other encoding', function () {
    let libxml = new Libxml();
    let testNotUtf8 = libxml.loadXml('test/data/test-default-not-utf8.xml');
    expect(testNotUtf8).to.be.true
    libxml.freeXml();
  });
  // Wellformed & invalid
  it('Should return wellformed & invalid on a wellformed BUT invalid xml', function () {
    let libxml = new Libxml();
    let testInvalidWf = libxml.loadXml('test/data/test-not-valid-dtd.xml');
    libxml.loadDtds(['test/dtd/mydoctype.dtd']);
    let testInvalid = libxml.validateAgainstDtds();
    expect(testInvalidWf).to.be.true;
    expect(testInvalid).to.be.false;
    expect(libxml).to.have.property('validationDtdErrors');
    expect(libxml.validationDtdErrors).to.be.an('object');
    libxml.freeDtds();
    libxml.freeXml();
  });
  // not wellformed
  it('Should return Not Wellformed & invalid on a not wellformed xml', function () {
    let libxml = new Libxml();
    let wellformed = libxml.loadXml('test/data/test-not-wellformed.xml');
    libxml.loadDtds(['test/dtd/mydoctype.dtd']);
    let wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.false;
    expect(wellformedV).to.be.false;
    expect(libxml).to.have.property('wellformedErrors');
    expect(libxml.wellformedErrors).to.be.an('array');
    libxml.freeDtds();
    libxml.freeXml();
  });
  it('Should return an object contaning DTD', function () {
    let libxml = new Libxml();
    let xmlfile = libxml.loadXml('test/data/test-default.xml');
    let currentDtd = libxml.getDtd();
    expect(currentDtd).to.be.an('object');
    expect(currentDtd).to.have.property('name', 'article');
    expect(currentDtd).to.have.property('externalId', 'my doctype of doom');
    expect(currentDtd).to.have.property('systemId', 'mydoctype.dtd');
    libxml.freeXml();
  });
  it('Shoud return correct xpath values', function () {
    let libxml = new Libxml(),
    xmlfile = libxml.loadXml('test/data/test-default.xml'),
    info = libxml.xpathSelect('string(//infos)'),
    infodust = libxml.xpathSelect('number(//infosdust)'),
    attrib = libxml.xpathSelect('string(//infos/@attrib)'),
    infoExist = libxml.xpathSelect('boolean(//infos)'),
    info23NotExist = libxml.xpathSelect('boolean(//infos23)'),
    xpathToMyExist = libxml.xpathSelect('boolean(/xpath/to/my)'),
    nbElementsInMy = libxml.xpathSelect('count(//my//*)'),
    nbElementsInXpath = libxml.xpathSelect('count(/xpath//*)');

    expect(info).to.be.a.string;
    expect(info).to.have.string('trezaq');
    expect(infodust).to.be.a('number');
    expect(infodust).to.equal(23);
    expect(attrib).to.be.a.string;
    expect(attrib).to.have.string('example');
    expect(infoExist).to.be.a('boolean');
    expect(infoExist).to.be.true;
    expect(info23NotExist).to.be.a('boolean');
    expect(info23NotExist).to.be.false;
    expect(xpathToMyExist).to.be.a('boolean');
    expect(xpathToMyExist).to.be.true;
    expect(nbElementsInMy).to.be.a('number');
    expect(nbElementsInMy).to.equal(2);
    expect(nbElementsInXpath).to.be.a('number');
    expect(nbElementsInXpath).to.equal(4);
    libxml.freeXml();
  });
  // // ABOVE IS ALL THE SAME WITH MEMORY MANUAL MANADGEMENT
  it('should free XML memory & infos when asked in manual mod On not wellformed xml', function () {
    let libxml = new Libxml(true);
    let wellformed = libxml.loadXml('test/data/test-not-wellformed.xml');
    expect(wellformed).to.be.false;
    expect(libxml).to.have.property('wellformedErrors');
    expect(libxml.wellformedErrors).to.be.an('array');
    expect(libxml.wellformedErrors.length).to.be.at.least(1);
    libxml.freeXml();
    expect(libxml).not.to.have.property('wellformedErrors');
  });
  it('should  not crash when freeUp memory xml multiple time!', function () {
    let libxml = new Libxml(true);
    let wellFormed = libxml.loadXml('test/data/test-default.xml');
    expect(wellFormed).to.be.true;
    expect(libxml).not.to.have.property('wellformedErrors');
    libxml.freeXml();
    libxml.freeXml();
    libxml.freeXml();
    expect(libxml).not.to.have.property('wellformedErrors');
  });
  it('Should return intended values after multiple CLEAN!', function () {
    let libxml = new Libxml();
    let wellformed = libxml.loadXml('test/data/test-not-valid-dtd.xml');
    libxml.loadDtds(['test/dtd/mydoctype.dtd']);
    let wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.true;
    expect(wellformedV).to.be.false;
    expect(libxml).to.not.have.property('wellformedErrors');
    libxml.freeXml();
    libxml.freeDtds();
    libxml.freeDtds();
    wellformed = libxml.loadXml('test/data/test-default.xml');
    libxml.loadDtds(['test/dtd/mydoctype.dtd', 'test/dtd/myBADdoctype.dtd']);
    wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.true;
    expect(wellformedV).to.be.a('string');
    expect(wellformedV).to.be.equal('test/dtd/mydoctype.dtd');
    expect(libxml).to.not.have.property('wellformedErrors');
  });
  it('should not crash when loads multiple dtd it two times', function () {
    let libxml = new Libxml();
    let wellformed = libxml.loadXml('test/data/test-not-valid-dtd.xml');
    libxml.loadDtds(['test/dtd/mydoctype.dtd', 'test/dtd/myBADdoctype.dtd']);
    libxml.loadDtds(['test/dtd/mydoctype2.dtd', 'test/dtd/myBADdoctype2.dtd']);
    let wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.true;
    expect(wellformedV).to.be.false;
    expect(libxml).to.not.have.property('wellformedErrors');
    expect(libxml).to.have.property('validationDtdErrors');
    expect(libxml.validationDtdErrors).to.have.property('test/dtd/mydoctype.dtd');
    expect(libxml.validationDtdErrors).to.have.property('test/dtd/mydoctype2.dtd');
    expect(libxml.validationDtdErrors).to.have.property('test/dtd/mydoctype2.dtd');
    expect(libxml.validationDtdErrors).to.have.property('test/dtd/myBADdoctype2.dtd');
    libxml.freeXml();
    libxml.freeDtds();
  });
  // SCHEMAS
  it('Should return wellformed & valid on a wellformed & valid xml SCHEMA', function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml('test/data/test-valid-schema.xml');
    libxml.loadSchemas(['test/xsd/MARC21slim.xsd']);
    let testDefaultV = libxml.validateAgainstSchemas();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.a('string');
    libxml.freeXml();
    libxml.freeSchemas();
  });
  it('Should return errors for invalid against schema', function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml('test/data/test-valid-schema.xml');
    libxml.loadSchemas(['test/xsd/MARC21slim-bad.xsd']);
    let testDefaultV = libxml.validateAgainstSchemas();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.false;
    expect(libxml).to.have.a.property('validationSchemaErrors');
    expect(libxml.validationSchemaErrors).to.be.a('object');
    expect(libxml.validationSchemaErrors).to.have.a.property('test/xsd/MARC21slim-bad.xsd');
    expect(libxml.validationSchemaErrors['test/xsd/MARC21slim-bad.xsd']).to.be.a('array');
    expect(libxml.validationSchemaErrors['test/xsd/MARC21slim-bad.xsd'][0]).to.have.a.property('column');
    expect(libxml.validationSchemaErrors['test/xsd/MARC21slim-bad.xsd'][0]).to.have.a.property('line');
    expect(libxml.validationSchemaErrors['test/xsd/MARC21slim-bad.xsd'][0]).to.have.a.property('message');
    libxml.freeXml();
    libxml.freeSchemas();
  });
  it('Should return expected value and not crash when multiple clean of schemas', function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml('test/data/test-valid-schema.xml');
    libxml.loadSchemas(['test/xsd/MARC21slim-bad.xsd','test/xsd/MARC21slim.xsd']);
    let testDefaultV = libxml.validateAgainstSchemas();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.a('string');
    libxml.freeXml();
    libxml.freeSchemas();
  });
  it('Should not throw error when use clearAll function to clear all memory libxml', function () {
    let libxml = new Libxml();
    libxml.clearAll();
  });
});