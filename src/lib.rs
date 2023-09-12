extern crate xmltree;

use std::fs::File;
use std::io::Read;


use xmltree::Element;


pub fn xml_read_file(path: &str) -> Result<Element, String> {
    dbg!(&path);
    // Open the XML file
    let mut file = File::open(path).expect("Error opening XML file.");

    // Read the content of the XML file into a string
    let mut xml_string = String::new();
    file.read_to_string(&mut xml_string).expect("Error reading XML file.");

    // Parse the XML string
    Ok(Element::parse(xml_string.as_bytes()).expect("Error parsing XML."))
}


#[cfg(test)]
mod tests {
    use super::*;
    

    #[test]
    fn test_xml_read_file_wellformed_file() {

        // Read the XML file using the xml_read_file function
        // We dont cover the case the file does not exist and return none its another test..
        if let Ok(root_element) = xml_read_file("./tests/data/test-default.xml"){
            // Check that the root element is correct
            assert_eq!(root_element.name, "xpath");
            let infosdust = root_element.get_child("to").unwrap().get_child("my").unwrap().get_child("infos").unwrap().get_text().unwrap();
            // let infodust_element = root.get_element("to/my/infodust").expect("Element 'infodust' not found.");
            assert_eq!(infosdust, "trezaq");
        }

    }

    #[test]
    #[should_panic(expected = "Error opening XML file.")]
    fn test_xml_read_file_not_existing_file() {
        // Try to read a not wellformed XML file that does not exist
        if let Err(err) = xml_read_file("tests/data/non_existent_file.xml") {
            panic!("{}", err);
        };
    }
    #[test]
    #[should_panic(expected = "Error parsing XML.")]
    fn test_xml_read_file_not_wellformed_file() {
        // Try to read a not wellformed XML file that does not exist
        if let Err(err)  = xml_read_file("tests/data/test-not-wellformed.xml") {
            panic!("{}", err);
        }
    }
}
