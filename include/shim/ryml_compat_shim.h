#pragma once
#include <ryml/ryml.hpp>

//
// RapidYAML Compatibility Shim for articuno
// Recreates deprecated API removed in ryml 0.5+
//

// -------------------------------------------
// YAML: compat for ryml::parse(csubstr)
// -------------------------------------------
namespace ryml {

inline Tree parse(csubstr src) {
    // parse_in_arena fully supports immutable buffers
    return parse_in_arena(src);
}

inline Tree parse(csubstr filename, csubstr src) {
    return parse_in_arena(filename, src);
}

} // namespace ryml


// -------------------------------------------
// JSON parse compatibility
// articuno historically used ryml::parse_json(...)
// -------------------------------------------
namespace ryml {

inline Tree parse_json(csubstr src) {
    return parse_json_in_arena(src);
}

inline Tree parse_json(csubstr filename, csubstr src) {
    return parse_json_in_arena(filename, src);
}

} // namespace ryml
