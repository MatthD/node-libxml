"use strict";

import { expect } from "chai";
import { Libxml } from "../";
import fs from "fs";

describe("Node-Libxml", function () {
  it("Should return a list of errored path if dtd path is bad", function () {
    let libxml = new Libxml();
    libxml.loadDtds(["tests/dtd/mydoctype-not-existing.dtd"]);
    expect(libxml).to.have.a.property("dtdsLoadedErrors");
    expect(libxml.dtdsLoadedErrors).to.be.a("array");
    expect(libxml.dtdsLoadedErrors).to.include(
      "tests/dtd/mydoctype-not-existing.dtd"
    );
    libxml.freeXml();
    libxml.freeDtds();
  });
  // Wellformed & valid
  it("Should return wellformed & valid on a wellformed & valid xml", function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml("tests/data/test-default.xml");
    libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
    let testDefaultV = libxml.validateAgainstDtds();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.a("string");
    libxml.freeXml();
    libxml.freeDtds();
  });
  // // it('Should return wellformed & valid on a wellformed & valid xml From String', function () {
  // //   let libxml = new Libxml();
  // //   let testDefaultStr = fs.readFileSync('tests/data/test-default.xml','utf8');
  // //   let myDtd = "<!ELEMENT xpath (to)>\n" +
  // //     "<!ELEMENT to (my)>\n" +
  // //     "<!ELEMENT my (infos,infosdust)>\n" +
  // //     "<!ATTLIST infos\n" +
  // //     "xmlns CDATA #FIXED ''\n" +
  // //     "attrib NMTOKEN #IMPLIED>\n" +
  // //     "<!ELEMENT infos (#PCDATA)>\n" +
  // //     "<!ELEMENT infosdust (#PCDATA)>";
  // //   let testDefaultWf = libxml.loadXmlFromString(testDefaultStr);
  // //   libxml.loadDtdsFromString([myDtd]);
  // //   let testDefaultV = libxml.validateAgainstDtds();
  // //   expect(testDefaultWf).to.be.true;
  // //   expect(testDefaultV).to.be.a('string');
  // //   libxml.freeXml();
  // //   libxml.freeDtds();
  // // });
  it("should return wellformed on a wellformed XMl in utf8-bom & other encoding", function () {
    let libxml = new Libxml();
    let testNotUtf8 = libxml.loadXml("tests/data/test-default-not-utf8.xml");
    expect(testNotUtf8).to.be.true;
    expect(libxml).not.to.have.property("wellformedErrors");
    libxml.freeXml();
  });
  it("should return wellformed on a wellformed XMl in utf8-bom & other encoding FROM STRING", function () {
    let libxml = new Libxml();
    let defaultNotUtf8 = fs.readFileSync(
      "tests/data/test-default-not-utf8.xml",
      "utf8"
    );
    let testNotUtf8 = libxml.loadXmlFromString(defaultNotUtf8);
    expect(testNotUtf8).to.be.true;
    expect(libxml).not.have.property("wellFormedErrors");
    libxml.freeXml();
  });
  // Wellformed & invalid
  it("Should return wellformed & invalid on a wellformed BUT invalid xml", function () {
    let libxml = new Libxml();
    let testInvalidWf = libxml.loadXml("tests/data/test-not-valid-dtd.xml");
    libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
    expect(libxml.getMaxErrorNumber()).to.be.equal(100);
    let testInvalid = libxml.validateAgainstDtds(3);
    expect(libxml.getMaxErrorNumber()).to.be.equal(100);
    expect(testInvalidWf).to.be.true;
    expect(testInvalid).to.be.false;
    expect(libxml).to.have.property("validationDtdErrors");
    expect(libxml.validationDtdErrors).to.be.an("object");
    expect(
      libxml.validationDtdErrors["tests/dtd/mydoctype.dtd"].length
    ).to.be.equal(3);
    libxml.freeDtds();
    libxml.freeXml();
  });
  it("Should return wellformed & invalid on a wellformed BUT invalid xml FROM STRING XML", function () {
    let libxml = new Libxml();
    let testInvalidWfStr = fs.readFileSync(
      "tests/data/test-not-valid-dtd.xml",
      "utf8"
    );
    let testInvalidWf = libxml.loadXmlFromString(testInvalidWfStr);
    libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
    libxml.setMaxErrorNumber(30);
    expect(libxml.getMaxErrorNumber()).to.be.equal(30);
    let testInvalid = libxml.validateAgainstDtds(3);
    expect(libxml.getMaxErrorNumber()).to.be.equal(30);
    expect(testInvalidWf).to.be.true;
    expect(testInvalid).to.be.false;
    expect(libxml).to.have.property("validationDtdErrors");
    expect(libxml.validationDtdErrors).to.be.an("object");
    expect(
      libxml.validationDtdErrors["tests/dtd/mydoctype.dtd"].length
    ).to.be.equal(3);
    libxml.freeDtds();
    libxml.freeXml();
  });
  // not wellformed
  it("Should return Not Wellformed & invalid on a not wellformed xml", function () {
    let libxml = new Libxml();
    let wellformed = libxml.loadXml("tests/data/test-not-wellformed.xml");
    libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
    let wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.false;
    expect(wellformedV).to.be.false;
    expect(libxml).to.have.property("wellformedErrors");
    expect(libxml.wellformedErrors).to.be.an("array");
    libxml.freeXml();
    expect(libxml).not.to.have.property("wellformedErrors");
  });
  it("Should return an object containing DTD", function () {
    let libxml = new Libxml();
    let loaded = libxml.loadXml("tests/data/test-default.xml");
    expect(loaded).to.be.true;
    let currentDtd = libxml.getDtd();
    expect(currentDtd).to.be.an("object");
    expect(currentDtd).to.have.property("name", "article");
    expect(currentDtd).to.have.property("externalId", "my doctype of doom");
    expect(currentDtd).to.have.property("systemId", "mydoctype.dtd");
    libxml.freeXml();
  });
  it("Should return correct xpath values", function () {
    let libxml = new Libxml();
    let xmlfile = libxml.loadXml("tests/data/test-default.xml");
    expect(xmlfile).to.be.true;
    let info = libxml.xpathSelect("string(//infos)"),
      infodust = libxml.xpathSelect("number(//infosdust)"),
      attrib = libxml.xpathSelect("string(//infos/@attrib)"),
      infoExist = libxml.xpathSelect("boolean(//infos)"),
      info23NotExist = libxml.xpathSelect("boolean(//infos23)"),
      xpathToMyExist = libxml.xpathSelect("boolean(/xpath/to/my)"),
      nbElementsInMy = libxml.xpathSelect("count(//my//*)"),
      nbElementsInXpath = libxml.xpathSelect("count(/xpath//*)");

    expect(info).to.be.a.string;
    expect(info).to.have.string("trezaq");
    expect(infodust).to.be.a("number");
    expect(infodust).to.equal(23);
    expect(attrib).to.be.a.string;
    expect(attrib).to.have.string("example");
    expect(infoExist).to.be.a("boolean");
    expect(infoExist).to.be.true;
    expect(info23NotExist).to.be.a("boolean");
    expect(info23NotExist).to.be.false;
    expect(xpathToMyExist).to.be.a("boolean");
    expect(xpathToMyExist).to.be.true;
    expect(nbElementsInMy).to.be.a("number");
    expect(nbElementsInMy).to.equal(2);
    expect(nbElementsInXpath).to.be.a("number");
    expect(nbElementsInXpath).to.equal(4);
    libxml.freeXml();
  });
  // // // ABOVE IS ALL THE SAME WITH MEMORY MANUAL MANAGEMENT
  it("should free XML memory & infos when asked in manual mod On not wellformed xml", function () {
    let libxml = new Libxml(true);
    let wellformed = libxml.loadXml("tests/data/test-not-wellformed.xml");
    expect(wellformed).to.be.false;
    expect(libxml).to.have.property("wellformedErrors");
    expect(libxml.wellformedErrors).to.be.an("array");
    expect(libxml.wellformedErrors.length).to.be.at.least(1);
    libxml.freeXml();
    expect(libxml).not.to.have.property("wellformedErrors");
  });
  it("should  not crash when freeUp memory xml multiple time!", function () {
    let libxml = new Libxml(true);
    let wellFormed = libxml.loadXml("tests/data/test-default.xml");
    expect(wellFormed).to.be.true;
    expect(libxml).not.to.have.property("wellformedErrors");
    libxml.freeXml();
    libxml.freeXml();
    libxml.freeXml();
    expect(libxml).not.to.have.property("wellformedErrors");
  });
  it("Should return intended values after multiple CLEAN!", function () {
    let libxml = new Libxml();
    let wellformed = libxml.loadXml("tests/data/test-not-valid-dtd.xml");
    libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
    let wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.true;
    expect(wellformedV).to.be.false;
    expect(libxml).to.not.have.property("wellformedErrors");
    libxml.freeXml();
    libxml.freeDtds();
    libxml.freeDtds();
    wellformed = libxml.loadXml("tests/data/test-default.xml");
    libxml.loadDtds(["tests/dtd/mydoctype.dtd", "tests/dtd/myBADdoctype.dtd"]);
    wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.true;
    expect(wellformedV).to.be.a("string");
    expect(wellformedV).to.be.equal("tests/dtd/mydoctype.dtd");
    expect(libxml).to.not.have.property("wellformedErrors");
  });
  it("should not crash when loads multiple dtd in two rounds", function () {
    let libxml = new Libxml();
    let wellformed = libxml.loadXml("tests/data/test-not-valid-dtd.xml");
    libxml.loadDtds(["tests/dtd/mydoctype.dtd", "tests/dtd/myBADdoctype.dtd"]);
    libxml.loadDtds([
      "tests/dtd/mydoctype2.dtd",
      "tests/dtd/myBADdoctype2.dtd",
    ]);
    let wellformedV = libxml.validateAgainstDtds();
    expect(wellformed).to.be.true;
    expect(wellformedV).to.be.false;
    expect(libxml).to.not.have.property("wellformedErrors");
    expect(libxml).to.have.property("validationDtdErrors");
    expect(libxml.validationDtdErrors).to.have.property(
      "tests/dtd/mydoctype.dtd"
    );
    expect(libxml.validationDtdErrors).to.have.property(
      "tests/dtd/myBADdoctype.dtd"
    );
    expect(libxml.validationDtdErrors).to.have.property(
      "tests/dtd/mydoctype2.dtd"
    );
    expect(libxml.validationDtdErrors).to.have.property(
      "tests/dtd/myBADdoctype2.dtd"
    );
    libxml.freeXml();
    libxml.freeDtds();
  });
  // // SCHEMAS
  it("Should return wellformed & valid on a wellformed & valid xml SCHEMA", function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml("tests/data/test-valid-schema.xml");
    libxml.loadSchemas(["tests/xsd/MARC21slim.xsd"]);
    let testDefaultV = libxml.validateAgainstSchemas();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.a("string");
    libxml.freeXml();
    libxml.freeSchemas();
  });
  it("Should return errors for invalid against schema", function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml("tests/data/test-valid-schema.xml");
    libxml.loadSchemas(["tests/xsd/MARC21slim-bad.xsd"]);
    let testDefaultV = libxml.validateAgainstSchemas();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.false;
    expect(libxml).to.have.a.property("validationSchemaErrors");
    expect(libxml.validationSchemaErrors).to.be.a("object");
    expect(libxml.validationSchemaErrors).to.have.a.property(
      "tests/xsd/MARC21slim-bad.xsd"
    );
    expect(
      libxml.validationSchemaErrors["tests/xsd/MARC21slim-bad.xsd"]
    ).to.be.a("array");
    expect(
      libxml.validationSchemaErrors["tests/xsd/MARC21slim-bad.xsd"][0]
    ).to.have.a.property("column");
    expect(
      libxml.validationSchemaErrors["tests/xsd/MARC21slim-bad.xsd"][0]
    ).to.have.a.property("line");
    expect(
      libxml.validationSchemaErrors["tests/xsd/MARC21slim-bad.xsd"][0]
    ).to.have.a.property("message");
    libxml.freeXml();
    libxml.freeSchemas();
  });
  it("Should return expected value and not crash when multiple clean of schemas", function () {
    let libxml = new Libxml();
    let testDefaultWf = libxml.loadXml("tests/data/test-valid-schema.xml");
    libxml.loadSchemas([
      "tests/xsd/MARC21slim-bad.xsd",
      "tests/xsd/MARC21slim.xsd",
    ]);
    let testDefaultV = libxml.validateAgainstSchemas();
    expect(testDefaultWf).to.be.true;
    expect(testDefaultV).to.be.a("string");
    libxml.freeXml();
    libxml.freeSchemas();
  });
  it("Should not throw error when use clearAll function to clear all memory libxml", function () {
    let libxml = new Libxml();
    libxml.clearAll();
  });
});
