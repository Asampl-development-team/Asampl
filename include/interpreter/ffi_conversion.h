#pragma once

#include <interpreter/value.h>

extern "C" {
#include <asampl-ffi/types.h>
}


AsaValueContainer convert_to_ffi(const ValuePtr& value);
ValuePtr convert_from_ffi(const AsaValueContainer& value);

#ifdef ASAMPL_ENABLE_PYTHON

#include <boost/python.hpp>

boost::python::object convert_to_python(const ValuePtr& value);
ValuePtr convert_from_python(boost::python::object value);

#endif
