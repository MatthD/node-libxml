"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const __1 = require("../");
const fs_1 = __importDefault(require("fs"));
describe("Node-Libxml", function () {
    it("Should return a list of errored path if dtd path is bad", function () {
        let libxml = new __1.Libxml();
        libxml.loadDtds(["tests/dtd/mydoctype-not-existing.dtd"]);
        expect(libxml).toHaveProperty("dtdsLoadedErrors");
        expect(Array.isArray(libxml.dtdsLoadedErrors)).toBe(true);
        expect(libxml.dtdsLoadedErrors).toContain("tests/dtd/mydoctype-not-existing.dtd");
        libxml.freeXml();
        libxml.freeDtds();
    });
    // Wellformed & valid
    it("Should return wellformed & valid on a wellformed & valid xml", function () {
        let libxml = new __1.Libxml();
        let testDefaultWf = libxml.loadXml("tests/data/test-default.xml");
        libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
        let testDefaultV = libxml.validateAgainstDtds();
        expect(testDefaultWf).toBeTruthy;
        expect(typeof testDefaultV).toBe("string");
        libxml.freeXml();
        libxml.freeDtds();
    });
    it("should return wellformed on a wellformed XMl in utf8-bom & other encoding", function () {
        let libxml = new __1.Libxml();
        let testNotUtf8 = libxml.loadXml("tests/data/test-default-not-utf8.xml");
        expect(testNotUtf8).toBeTruthy;
        expect(libxml).not.toHaveProperty("wellformedErrors");
        libxml.freeXml();
    });
    it("should return wellformed on a wellformed XMl in utf8-bom & other encoding FROM STRING", function () {
        let libxml = new __1.Libxml();
        let defaultNotUtf8 = fs_1.default.readFileSync("tests/data/test-default-not-utf8.xml", "utf8");
        let testNotUtf8 = libxml.loadXmlFromString(defaultNotUtf8);
        expect(testNotUtf8).toBeTruthy;
        expect(libxml).not.toHaveProperty("wellFormedErrors");
        libxml.freeXml();
    });
    // Wellformed & invalid
    it("Should return wellformed & invalid on a wellformed BUT invalid xml", function () {
        var _a;
        let libxml = new __1.Libxml();
        let testInvalidWf = libxml.loadXml("tests/data/test-not-valid-dtd.xml");
        libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
        expect(libxml.getMaxErrorNumber()).toEqual(100);
        let testInvalid = libxml.validateAgainstDtds(3);
        expect(libxml.getMaxErrorNumber()).toEqual(100);
        expect(testInvalidWf).toBeTruthy;
        expect(testInvalid).toBeFalsy;
        expect(libxml).toHaveProperty("validationDtdErrors");
        expect(typeof libxml.validationDtdErrors).toBe("object");
        expect((_a = libxml.validationDtdErrors) === null || _a === void 0 ? void 0 : _a["tests/dtd/mydoctype.dtd"].length).toEqual(3);
        libxml.freeDtds();
        libxml.freeXml();
    });
    it("Should return wellformed & invalid on a wellformed BUT invalid xml FROM STRING XML", function () {
        var _a;
        let libxml = new __1.Libxml();
        let testInvalidWfStr = fs_1.default.readFileSync("tests/data/test-not-valid-dtd.xml", "utf8");
        let testInvalidWf = libxml.loadXmlFromString(testInvalidWfStr);
        libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
        libxml.setMaxErrorNumber(30);
        expect(libxml.getMaxErrorNumber()).toEqual(30);
        let testInvalid = libxml.validateAgainstDtds(3);
        expect(libxml.getMaxErrorNumber()).toEqual(30);
        expect(testInvalidWf).toBeTruthy;
        expect(testInvalid).toBeFalsy;
        expect(libxml).toHaveProperty("validationDtdErrors");
        expect(typeof libxml.validationDtdErrors).toBe("object");
        expect((_a = libxml.validationDtdErrors) === null || _a === void 0 ? void 0 : _a["tests/dtd/mydoctype.dtd"].length).toEqual(3);
        libxml.freeDtds();
        libxml.freeXml();
    });
    // not wellformed
    it("Should return Not Wellformed & invalid on a not wellformed xml", function () {
        let libxml = new __1.Libxml();
        let wellformed = libxml.loadXml("tests/data/test-not-wellformed.xml");
        libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
        let wellformedV = libxml.validateAgainstDtds();
        expect(wellformed).toBeFalsy;
        expect(wellformedV).toBeFalsy;
        expect(libxml).toHaveProperty("wellformedErrors");
        expect(Array.isArray(libxml.wellformedErrors)).toBe(true);
        libxml.freeXml();
        expect(libxml).not.toHaveProperty("wellformedErrors");
    });
    it("Should return an object containing DTD", function () {
        let libxml = new __1.Libxml();
        let loaded = libxml.loadXml("tests/data/test-default.xml");
        expect(loaded).toBeTruthy;
        let currentDtd = libxml.getDtd();
        expect(typeof currentDtd).toBe("object");
        expect(currentDtd).toHaveProperty("name", "article");
        expect(currentDtd).toHaveProperty("externalId", "my doctype of doom");
        expect(currentDtd).toHaveProperty("systemId", "mydoctype.dtd");
        libxml.freeXml();
    });
    it("Should return correct xpath values", function () {
        let libxml = new __1.Libxml();
        let xmlfile = libxml.loadXml("tests/data/test-default.xml");
        expect(xmlfile).toBeTruthy;
        let info = libxml.xpathSelect("string(//infos)"), infodust = libxml.xpathSelect("number(//infosdust)"), attrib = libxml.xpathSelect("string(//infos/@attrib)"), infoExist = libxml.xpathSelect("boolean(//infos)"), info23NotExist = libxml.xpathSelect("boolean(//infos23)"), xpathToMyExist = libxml.xpathSelect("boolean(/xpath/to/my)"), nbElementsInMy = libxml.xpathSelect("count(//my//*)"), nbElementsInXpath = libxml.xpathSelect("count(/xpath//*)");
        expect(info).toContain("trezaq");
        expect(infodust).toEqual(23);
        expect(attrib).toContain("example");
        expect(typeof infoExist).toBe("boolean");
        expect(infoExist).toBeTruthy;
        expect(typeof info23NotExist).toBe("boolean");
        expect(info23NotExist).toBeFalsy;
        expect(typeof xpathToMyExist).toBe("boolean");
        expect(xpathToMyExist).toBeTruthy;
        expect(typeof nbElementsInMy).toBe("number");
        expect(nbElementsInMy).toEqual(2);
        expect(typeof nbElementsInXpath).toBe("number");
        expect(nbElementsInXpath).toEqual(4);
        libxml.freeXml();
    });
    // // // ABOVE IS ALL THE SAME WITH MEMORY MANUAL MANAGEMENT
    it("should free XML memory & infos when asked in manual mod On not wellformed xml", function () {
        var _a;
        let libxml = new __1.Libxml();
        let wellformed = libxml.loadXml("tests/data/test-not-wellformed.xml");
        expect(wellformed).toBeFalsy;
        expect(libxml).toHaveProperty("wellformedErrors");
        expect(Array.isArray(libxml.wellformedErrors)).toBe(true);
        expect((_a = libxml.wellformedErrors) === null || _a === void 0 ? void 0 : _a.length).toBeGreaterThanOrEqual(1);
        libxml.freeXml();
        expect(libxml).not.toHaveProperty("wellformedErrors");
    });
    it("should  not crash when freeUp memory xml multiple time!", function () {
        let libxml = new __1.Libxml();
        let wellFormed = libxml.loadXml("tests/data/test-default.xml");
        expect(wellFormed).toBeTruthy;
        expect(libxml).not.toHaveProperty("wellformedErrors");
        libxml.freeXml();
        libxml.freeXml();
        libxml.freeXml();
        expect(libxml).not.toHaveProperty("wellformedErrors");
    });
    it("Should return intended values after multiple CLEAN!", function () {
        let libxml = new __1.Libxml();
        let wellformed = libxml.loadXml("tests/data/test-not-valid-dtd.xml");
        libxml.loadDtds(["tests/dtd/mydoctype.dtd"]);
        let wellformedV = libxml.validateAgainstDtds();
        expect(wellformed).toBeTruthy;
        expect(wellformedV).toBeFalsy;
        expect(libxml).not.toHaveProperty("wellformedErrors");
        libxml.freeXml();
        libxml.freeDtds();
        libxml.freeDtds();
        wellformed = libxml.loadXml("tests/data/test-default.xml");
        libxml.loadDtds(["tests/dtd/mydoctype.dtd", "tests/dtd/myBADdoctype.dtd"]);
        wellformedV = libxml.validateAgainstDtds();
        expect(wellformed).toBeTruthy;
        expect(typeof wellformedV).toBe("string");
        expect(wellformedV).toEqual("tests/dtd/mydoctype.dtd");
        expect(libxml).not.toHaveProperty("wellformedErrors");
    });
    it("should not crash when loads multiple dtd in two rounds", function () {
        var _a, _b, _c, _d, _e;
        let libxml = new __1.Libxml();
        let wellformed = libxml.loadXml("tests/data/test-not-valid-dtd.xml");
        libxml.loadDtds(["tests/dtd/mydoctype.dtd", "tests/dtd/myBADdoctype.dtd"]);
        libxml.loadDtds([
            "tests/dtd/mydoctype2.dtd",
            "tests/dtd/myBADdoctype2.dtd",
        ]);
        let wellformedV = libxml.validateAgainstDtds();
        expect(wellformed).toBeTruthy;
        expect(wellformedV).toBeFalsy;
        expect(libxml).not.toHaveProperty("wellformedErrors");
        expect(libxml).toHaveProperty("validationDtdErrors");
        (_a = libxml.validationDtdErrors) === null || _a === void 0 ? void 0 : _a["tests/dtd/mydoctype.dtd"];
        expect((_b = libxml.validationDtdErrors) === null || _b === void 0 ? void 0 : _b["tests/dtd/mydoctype.dtd"]).toHaveLength(5);
        expect((_c = libxml.validationDtdErrors) === null || _c === void 0 ? void 0 : _c["tests/dtd/myBADdoctype.dtd"]).toHaveLength(5);
        expect((_d = libxml.validationDtdErrors) === null || _d === void 0 ? void 0 : _d["tests/dtd/myBADdoctype2.dtd"]).toHaveLength(5);
        expect((_e = libxml.validationDtdErrors) === null || _e === void 0 ? void 0 : _e["tests/dtd/mydoctype2.dtd"]).toHaveLength(5);
        libxml.freeXml();
        libxml.freeDtds();
    });
    it("Should return wellformed & valid on a wellformed & valid xml SCHEMA", function () {
        let libxml = new __1.Libxml();
        let testDefaultWf = libxml.loadXml("tests/data/test-valid-schema.xml");
        libxml.loadSchemas(["tests/xsd/MARC21slim.xsd"]);
        let testDefaultV = libxml.validateAgainstSchemas();
        expect(testDefaultWf).toBeTruthy;
        expect(typeof testDefaultV).toBe("string");
        libxml.freeXml();
        libxml.freeSchemas();
    });
    it("Should return errors for invalid against schema", function () {
        var _a, _b, _c, _d, _e;
        let libxml = new __1.Libxml();
        let testDefaultWf = libxml.loadXml("tests/data/test-valid-schema.xml");
        libxml.loadSchemas(["tests/xsd/MARC21slim-bad.xsd"]);
        let testDefaultV = libxml.validateAgainstSchemas();
        expect(testDefaultWf).toBeTruthy;
        expect(testDefaultV).toBeFalsy;
        expect(libxml).toHaveProperty("validationSchemaErrors");
        expect(typeof libxml.validationSchemaErrors).toBe("object");
        expect((_a = libxml.validationSchemaErrors) === null || _a === void 0 ? void 0 : _a["tests/xsd/MARC21slim-bad.xsd"])
            .toBeTruthy;
        expect(Array.isArray((_b = libxml.validationSchemaErrors) === null || _b === void 0 ? void 0 : _b["tests/xsd/MARC21slim-bad.xsd"])).toBe(true);
        expect((_c = libxml.validationSchemaErrors) === null || _c === void 0 ? void 0 : _c["tests/xsd/MARC21slim-bad.xsd"][0]).toHaveProperty("column");
        expect((_d = libxml.validationSchemaErrors) === null || _d === void 0 ? void 0 : _d["tests/xsd/MARC21slim-bad.xsd"][0]).toHaveProperty("line");
        expect((_e = libxml.validationSchemaErrors) === null || _e === void 0 ? void 0 : _e["tests/xsd/MARC21slim-bad.xsd"][0]).toHaveProperty("message");
        libxml.freeXml();
        libxml.freeSchemas();
    });
    it("Should return expected value and not crash when multiple clean of schemas", function () {
        let libxml = new __1.Libxml();
        let testDefaultWf = libxml.loadXml("tests/data/test-valid-schema.xml");
        libxml.loadSchemas([
            "tests/xsd/MARC21slim-bad.xsd",
            "tests/xsd/MARC21slim.xsd",
        ]);
        let testDefaultV = libxml.validateAgainstSchemas();
        expect(testDefaultWf).toBeTruthy;
        expect(typeof testDefaultV).toBe("string");
        libxml.freeXml();
        libxml.freeSchemas();
    });
    it("Should not throw error when use clearAll function to clear all memory libxml", function () {
        let libxml = new __1.Libxml();
        libxml.clearAll();
    });
});
