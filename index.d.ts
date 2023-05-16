export enum XmlErrorLevel {
  /* No error */
  XML_ERR_NONE = 0,
  /* A simple warning */
  XML_ERR_WARNING = 1,
  /* A recoverable error */
  XML_ERR_ERROR = 2,
  /* A fatal error */
  XML_ERR_FATAL = 3,
}

export interface XmlError {
  message: string;
  level: XmlErrorLevel;
  column: number;
  file: string;
  line: number;
  int1: number;
}

export interface XmlDtdResult {
  name: string;
  externalId: string;
  systemId: string;
}

export class Libxml {
  /**
   * Loads an XML from a given URL which is generally a filepath
   * @param url The file to load from
   * @returns True if the load was successful and false otherwise. If failed than a property ```wellformedErrors``` appears
   *          on the Libxml object and contains the errors. In case of success the property do not presented on the Libxml
   *          object after this call.
   */
  loadXml(url: string): boolean;

  /**
   * Loads an XML from a string.
   * @param content The content of an XML
   * @returns True if the load was successful and false otherwise. If failed than a property ```wellformedErrors``` appears
   *          on the Libxml object and contains the errors. In case of success the property do not presented on the Libxml
   *          object after this call.
   */
  loadXmlFromString(content: string): boolean;

  /**
   * Loads DTDs. Silently drops elements from input array which are not strings
   * @param dtds The array which contains DTDs
   * @returns It has no return value but in case there were any error with any of the given DTDs than a property with name
   *          ```dtdsLoadedErrors``` appears on the Libxml object. If load was successful than no such property presented
   * REMARKS:
   * * Can call multiple time and all correct will be stored.
   * * Can load same DTD multiple time but has no good use just increase memory consumption.
   */
  loadDtds(dtds: string[]): void;

  /**
   * Loads schemas. Silently drops elements from input array which are not strings
   * @param schemas The schemas to load
   * @returns It has no return value but in case there were any error with any of the given schemas than a property with name
   *          ```schemasLoadedErrors``` appears on the Libxml object. There will be no such property presented on successful load.
   */
  loadSchemas(schemas: string[]): void;

  /**
   * Validate loaded XML against loaded DTDs
   * @param maxErrorNumber The maximum number of errors that should collected and returned on a dtd per basis.
   * @returns
   * * ```null``` if not any DTDs loaded correctly at all. (No load at all or all of it was wrong)
   * * A string which is the name of the DTD if it has any name
   * * True if the DTD against which it is matched has no name. (No SystemID presented)
   * * False otherwise and a property with name ```validationDtdErrors``` appears on Libxml object
   *
   * REMARKS:
   * * In case of successful load (When return with a string or true) no property with name ```validationDtdErrors```
   *  will be presented on the Libxml object
   * * In case of ```null``` return value ```validationDtdErrors``` property will remain as it was
   */
  validateAgainstDtds(maxErrorNumber?: number): null | boolean | string;

  /**
   * Validate loaded XML against loaded schemas
   * @param maxErrorNumber The maximum number of errors that should collected and returned on a schema per basis.
   * @returns
   * * ```null``` if not any schemas loaded correctly at all. (No load at all or all of it was wrong)
   * * A string which is the name of the schema if it has any name
   * * True if the schema against which it is matched has no name. (No URL presented in doc)
   * * False otherwise and a property with name ```validationSchemaErrors``` appears on Libxml object
   *
   * REMARKS:
   * * In case of successful load (When return with a string or true) no property with name ```validationSchemaErrors```
   *  will be presented on the Libxml object
   * * In case of ```null``` return value ```validationSchemaErrors``` property will remain as it was
   */
  validateAgainstSchemas(maxErrorNumber?: number): null | boolean | string;

  /**
   * Evaluate an xPath on a previously loaded XML.
   * @param selector The xPath selector
   * @returns
   * * ```null``` if no match found
   * * boolean OR string OR number value if matched.
   * @throws
   * * If not given any param
   * * If exception happens during evaluation
   */
  xpathSelect(selector: string): null | boolean | number | string;

  /**
   * Get data from the loaded XML
   * @returns
   * * ```null``` if no XML loaded or it has no such data
   * * An object if valid XML loaded. Although it could has all property value ```null```
   */
  getDtd(): null | XmlDtdResult;

  /**
   * Drops memory correlated with the loaded XML.
   *
   * Also delete property ```wellformedErrors``` from Libxml object
   */
  freeXml(): void;

  /**
   * Drop memory correlated with loaded DTDs.
   *
   * Also delete property ```dtdsLoadedErrors``` and ```validationDtdErrors``` from the Libxml object
   */
  freeDtds(): void;

  /**
   * Drop memory correlated with loaded schemas.
   *
   * Also delete property ```schemasLoadedErrors``` and ```validationSchemasErrors``` from the Libxml object
   */
  freeSchemas(): void;

  /**
   * Free up all memory used by the lib.
   *
   * Also drops all properties of the Libxml object
   * * ```wellformedErrors```
   * * ```dtdsLoadedErrors``` and ```validationDtdErrors```
   * * ```schemasLoadedErrors``` and ```validationSchemasErrors```
   */
  clearAll(): void;

  /**
   * @returns The maximum number of error which will be stores
   */
  getMaxErrorNumber(): number;

  /**
   * Sets the maximum number of error which wil be stored.
   *
   * The default value is 100
   *
   * @param max The desired max number
   * @returns The actual number set.
   * In case if it was not a number or if it is too big maybe you got other result as expected.
   * If param was not a number or non positive than max number will not change.
   */
  setMaxErrorNumber(max: number): number;

  wellformedErrors?: XmlError[];
  dtdsLoadedErrors?: (string | XmlError)[];
  schemasLoadedErrors?: (string | XmlError)[];
  validationDtdErrors?: Record<string /*SystemId*/, XmlError[]>;
  validationSchemaErrors?: Record<string /*SchemaName*/, XmlError[]>;
}
