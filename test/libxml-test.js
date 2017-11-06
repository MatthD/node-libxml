'use strict';

const Libxml = require('../index.js');
const expect = require('chai').expect;

describe('Node-Libxml', function () {
  // Wellformed & valid
  it('Should return wellformed & valid on a wellformed & valid xml', function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml('test/data/test-default.xml');
    let testDefaultV = libxml.validateAgainstDtd('test/dtd/mydoctype.dtd');
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.true;
  });
  it('should return weelformed on a wellformed XMl in utf8-bom & other encoding', function () {
    let libxml = new Libxml();
    let testNotUtf8 = libxml.loadXml('test/data/test-default-not-utf8.xml');
    expect(testNotUtf8).to.be.true
  });
  // Wellformed & invalid
  it('Should return wellformed & invalid on a wellformed BUT invalid xml', function () {
    let libxml = new Libxml();
    let testInvalidWf = libxml.loadXml('test/data/test-not-valid-dtd.xml');
    let testInvalid = libxml.validateAgainstDtd('test/dtd/mydoctype.dtd');
    expect(testInvalidWf).to.be.true;
    expect(testInvalid).to.be.false;
    expect(libxml).to.have.property('validationErrors');
    expect(libxml.validationErrors).to.be.an('array');
    expect(libxml.validationErrors.length).to.be.at.least(1);
  });
  // not wellformed
  it('Should return Not Wellformed & invalid on a not wellformed xml', function () {
    let libxml = new Libxml();
    let notWellformed = libxml.loadXml('test/data/test-not-wellformed.xml');
    let notwellformedV = libxml.validateAgainstDtd('test/dtd/mydoctype.dtd');
    expect(notWellformed).to.be.false;
    expect(notwellformedV).to.be.false;
    expect(libxml).to.have.property('wellformedErrors');
    expect(libxml.wellformedErrors).to.be.an('array');
    expect(libxml.wellformedErrors.length).to.be.at.least(1);
  });
  it('Should return an object contaning DTD', function () {
    let libxml = new Libxml();
    let xmlfile = libxml.loadXml('test/data/test-default.xml');
    let currentDtd = libxml.getDtd();
    expect(currentDtd).to.be.an('object');
    expect(currentDtd).to.have.property('name', 'article');
    expect(currentDtd).to.have.property('externalId', 'my doctype of doom');
    expect(currentDtd).to.have.property('systemId', 'mydoctype.dtd');
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
  });
  it('should not free XML memory & infos when asked in manual mod On not wellformed xml', function () {
    let libxml = new Libxml();
    let notWellformed = libxml.loadXml('test/data/test-not-wellformed.xml');
    expect(notWellformed).to.be.false;
    expect(libxml).to.have.property('wellformedErrors');
    expect(libxml.wellformedErrors).to.be.an('array');
    expect(libxml.wellformedErrors.length).to.be.at.least(1);
    libxml.freeXml();
    expect(libxml.wellformedErrors).to.be.an('array');
    expect(libxml.wellformedErrors.length).to.be.at.least(1)
  });
  it('should free XML memory & infos when asked in manual mod On not wellformed xml', function () {
    let libxml = new Libxml(true);
    let notWellformed = libxml.loadXml('test/data/test-not-wellformed.xml');
    expect(notWellformed).to.be.false;
    expect(libxml).to.have.property('wellformedErrors');
    expect(libxml.wellformedErrors).to.be.an('array');
    expect(libxml.wellformedErrors.length).to.be.at.least(1);
    libxml.freeXml();
    expect(libxml).not.to.have.property('wellformedErrors');
  });
  it('should free XML memory & infos when asked in manual mod wellformed xml ', function () {
    let libxml = new Libxml(true);
    let wellFormed = libxml.loadXml('test/data/test-default.xml');
    expect(wellFormed).to.be.true;
    expect(libxml).not.to.have.property('wellformedErrors');
    libxml.freeXml();
    expect(libxml).not.to.have.property('wellformedErrors');
  });
});