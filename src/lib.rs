extern crate xml;

use std::fs::File;
use std::io::BufReader;
use std::collections::HashMap;

use xml::reader::{EventReader, XmlEvent};

pub fn is_wellformed_from_file(path: &str) -> Result<(), String> {
    // Open the XML file
    let file = File::open(path).expect("Error opening XML file.");
    let reader = BufReader::new(file);

    let parser = EventReader::new(reader);
    let mut element_stack: Vec<String> = Vec::new();
    let mut error_map: HashMap<String, Vec<String>> = HashMap::new();

    for event in parser {
        match event {
            Ok(XmlEvent::StartElement { name, .. }) => {
                // Element is opened
                let element_name = name.local_name;
                element_stack.push(element_name.clone());

                // Increment the element count for this element
                let count = error_map.entry(element_name.clone()).or_insert(vec![]);
                count.push("opened".to_string());
            }
            Ok(XmlEvent::EndElement { name, .. }) => {
                // Element is closed
                let element_name = name.local_name;

                // Decrement the element count for this element
                if let Some(count) = error_map.get_mut(&element_name) {
                    count.pop();
                }

                // Pop the element from the stack
                if let Some(last) = element_stack.pop() {
                    // Check if the element had errors
                    if let Some(errors) = error_map.get(&last) {
                        if errors.is_empty() {
                            error_map.remove(&last);
                        }
                    }
                }
            }
            Ok(XmlEvent::Characters(text)) => {
                // Handle character data here if needed
            }
            Err(e) => {
                // Record any parsing errors
                let current_element = element_stack.last().cloned().unwrap_or_default();
                let error_list = error_map.entry(current_element).or_insert(vec![]);
                error_list.push(e.to_string());
            }
            _ => {}
        }
    }

    // Check if there are any remaining errors
    if !error_map.is_empty() {
        return Err("XML is not well-formed".to_string());
    }

    Ok(())
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_is_wellformed_from_file_wellformed_file() {
        // Test with a well-formed XML file
        if let Err(err) = is_wellformed_from_file("./tests/data/test-default.xml") {
            panic!("{}", err);
        };
    }

    // #[test]
    // fn test_is_wellformed_from_file_not_wellformed_file() {
    //     // Test with a not well-formed XML file
    //     if let Ok(()) = is_wellformed_from_file("./tests/data/test-not-wellformed.xml") {
    //         panic!("Expected an error, but got Ok(())");
    //     };
    // }

    // #[test]
    // fn test_is_wellformed_from_file_with_errors() {
    //     // Test with a well-formed XML file that contains errors
    //     if let Ok(()) = is_wellformed_from_file("./tests/data/test-with-errors.xml") {
    //         panic!("Expected an error, but got Ok(())");
    //     };
    // }

    // #[test]
    // fn test_is_wellformed_from_file_empty_file() {
    //     // Test with an empty XML file
    //     if let Ok(()) = is_wellformed_from_file("./tests/data/empty.xml") {
           


}
    // Rest of your code remains the same


// #[cfg(test)]
// mod tests {
//     use super::*;
    

//     #[test]
//     fn test_is_wellformed_from_file_wellformed_file() {

//         // Read the XML file using the is_wellformed_from_file function
//         // We dont cover the case the file does not exist and return none its another test..
//         if let Ok(root_element) = is_wellformed_from_file("./tests/data/test-default.xml"){
//             // Check that the root element is correct
//             assert_eq!(root_element.name, "xpath");
//             let infosdust = root_element.get_child("to").unwrap().get_child("my").unwrap().get_child("infos").unwrap().get_text().unwrap();
//             // let infodust_element = root.get_element("to/my/infodust").expect("Element 'infodust' not found.");
//             assert_eq!(infosdust, "trezaq");
//         }

//     }

//     #[test]
//     #[should_panic(expected = "Error opening XML file.")]
//     fn test_is_wellformed_from_file_not_existing_file() {
//         // Try to read a not wellformed XML file that does not exist
//         if let Err(err) = is_wellformed_from_file("tests/data/non_existent_file.xml") {
//             panic!("{}", err);
//         };
//     }
//     #[test]
//     #[should_panic(expected = "Error parsing XML.")]
//     fn test_is_wellformed_from_file_not_wellformed_file() {
//         // Try to read a not wellformed XML file that does not exist
//         if let Err(err)  = is_wellformed_from_file("tests/data/test-not-wellformed.xml") {
//             panic!("{}", err);
//         }
//     }
// }
