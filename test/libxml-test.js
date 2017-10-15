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
});

