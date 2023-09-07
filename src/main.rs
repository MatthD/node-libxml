use libxml::{tree::Document, parser::XmlParseError};



fn main() {
    let parser = libxml::parser::Parser::default(); 
    let xml_file = parser.parse_file("tests/data/test-not-wellformed.xml");
    // This should return false and it return true !! 🙃
    dbg!(is_wellformed(xml_file));
}

fn is_wellformed(doc: Result<Document, XmlParseError>)-> bool{
    match doc {
        Err(_error) => {
            false
        },
        Ok(_doc) => {
            true
        },
    }
}
