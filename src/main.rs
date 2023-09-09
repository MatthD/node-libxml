extern crate xmltree;

use std::fs::File;
use std::io::Read;

use xmltree::Element;

fn main() {
    // Nom du fichier XML à lire
    let filename = "tests/data/test-not-wellformed.xml";

    if let Some(root) = xml_read_file(filename){
        // Par exemple, afficher le nom de la balise racine
        println!("Balise racine : {}", root.name);
    }
}

fn xml_read_file(path: &str) -> Option<Element> {
    dbg!(&path);
    // Open the XML file
    let mut file = File::open(path).expect("Error opening XML file.");

    // Read the content of the XML file into a string
    let mut xml_string = String::new();
    file.read_to_string(&mut xml_string).expect("Error reading XML file.");

    // Parse the XML string
    Some(Element::parse(xml_string.as_bytes()).expect("Error parsing XML."))
}


#[cfg(test)]
mod tests {
    use super::*;
    

    #[test]
    fn test_xml_read_file_wellformed_file() {

        // Read the XML file using the xml_read_file function
        // We dont cover the case the file does not exist and return none its another test..
        if let Some(root_element) = xml_read_file("./tests/data/test-default.xml"){
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
        xml_read_file("tests/data/non_existent_file.xml");
    }
    #[test]
    #[should_panic(expected = "Error parsing XML.")]
    fn test_xml_read_file_not_wellformed_file() {
        // Try to read a not wellformed XML file that does not exist
        xml_read_file("tests/data/test-not-wellformed.xml");
    }
}
