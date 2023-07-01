#pragma once

namespace Botcraft
{
    namespace Utilities
    {
        /// @brief Demangle a class name
        /// @param name Raw output of typeid().name()
        /// @param remove_template If true, will simplify the name to get only the class name without any template <parameters> or namespace prefix
        /// @return Demangled name
        std::string Demangle(const std::string& name, const bool simplify_output);
    }
}