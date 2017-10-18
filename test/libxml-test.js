'use strict';

const Libxml = require('../index.js');
const expect = require('chai').expect;

describe('popplonode', function () {
  // Wellformed & valid
  it('Should return wellformed & valid on a wellformed & valid xml', function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.load('test/data/test-default.xml');
    let testDefaultV = libxml.validateAgainstDtd('test/dtd/mydoctype.dtd');
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.true;
  });
  // Wellformed & invalid
  it('Should return wellformed & invalid on a wellformed BUT invalid xml', function () {
    let libxml = new Libxml();
    let testInvalidWf = libxml.load('test/data/test-not-valid-dtd.xml');
    let testInvalid = libxml.validateAgainstDtd('test/dtd/mydoctype.dtd');
    expect(testInvalidWf).to.be.true;
    expect(testInvalid).to.be.false;
  });
  // not wellformed
  it('Should return Not Wellformed & invalid on a not wellformed xml', function () {
    let libxml = new Libxml();
    let notWellformed = libxml.load('test/data/test-not-wellformed.xml');
    let notwellformedV = libxml.validateAgainstDtd('test/dtd/mydoctype.dtd');
    expect(notWellformed).to.be.false;
    expect(notWellformed).to.be.false;
  });
  it('Should return an object contaning DTD', function () {
    let libxml = new Libxml();
    let xmlfile = libxml.load('test/data/test-default.xml');
    let currentDtd = libxml.getDtd();
    expect(currentDtd).to.be.an('object');
    expect(currentDtd).to.have.property('name', 'article');
    expect(currentDtd).to.have.property('externalId', 'my doctype of doom');
    expect(currentDtd).to.have.property('systemId', 'mydoctype.dtd');
  });
  it('Shoud return correct xpath values', function () {
    let libxml = new Libxml(),
    xmlfile = libxml.load('test/data/test-default.xml'),
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

  })
});

