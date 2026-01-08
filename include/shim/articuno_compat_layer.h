#pragma once

#include <ryml/ryml.hpp>
#include <articuno/articuno.h>

//
// articuno compatibility wrappers for modern ryml
//

namespace articuno {

// parse YAML string into Tree, then into articuno object
template <typename T>
bool from_yaml(const std::string& yaml, T& out)
{
    ryml::Tree tree = ryml::parse(ryml::to_csubstr(yaml));
    return articuno::from_yaml(tree, out);
}

template <typename T>
bool from_json(const std::string& json, T& out)
{
    ryml::Tree tree = ryml::parse_json(ryml::to_csubstr(json));
    return articuno::from_json(tree, out);
}

// convenience function
template <typename T>
T load_yaml(const std::string& yaml)
{
    T result{};
    articuno::from_yaml(yaml, result);
    return result;
}

template <typename T>
T load_json(const std::string& json)
{
    T result{};
    articuno::from_json(json, result);
    return result;
}

} // namespace articuno
