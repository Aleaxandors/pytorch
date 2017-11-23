#pragma once

#include <Python.h>
#include <stdint.h>

#include <limits>

// largest integer that can be represented consecutively in a double
const int64_t DOUBLE_INT_MAX = 9007199254740992;

inline PyObject* THPUtils_packInt64(int64_t value) {
  using lll = std::numeric_limits<long long>;
  using i64l = std::numeric_limits<int64_t>;
  static_assert(lll::max() == i64l::max() && lll::min() == i64l::min(),
                "packInt64 may silently over/underflow");
#if PY_MAJOR_VERSION == 2
  if (value <= INT32_MAX && value >= INT32_MIN) {
    return PyInt_FromLong(static_cast<long>(value));
  }
#endif
  return PyLong_FromLongLong(value);
}

inline PyObject* THPUtils_packUInt64(uint64_t value) {
  using ulll = std::numeric_limits<unsigned long long>;
  using ui64l = std::numeric_limits<uint64_t>;
  static_assert(ulll::max() == ui64l::max() && ulll::min() == ui64l::min(),
                "packUInt64 may silently over/underflow");
#if PY_MAJOR_VERSION == 2
  if (value <= static_cast<uint64_t>(LONG_MAX)) {
    return PyInt_FromLong(static_cast<long>(value));
  }
#endif
  return PyLong_FromUnsignedLongLong(value);
}

inline bool THPUtils_checkLong(PyObject* obj) {
#if PY_MAJOR_VERSION == 2
  return (PyLong_Check(obj) || PyInt_Check(obj)) && !PyBool_Check(obj);
#else
  return PyLong_Check(obj) && !PyBool_Check(obj);
#endif
}

inline int64_t THPUtils_unpackLong(PyObject* obj) {
  if (PyLong_Check(obj)) {
    int overflow;
    long long value = PyLong_AsLongLongAndOverflow(obj, &overflow);
    if (overflow != 0) {
      throw std::runtime_error("Overflow when unpacking long");
    }
    return (int64_t)value;
  }
#if PY_MAJOR_VERSION == 2
  if (PyInt_Check(obj)) {
    return PyInt_AS_LONG(obj);
  }
#endif
  throw std::runtime_error("Could not unpack long");
}

inline bool THPUtils_checkDouble(PyObject* obj) {
#if PY_MAJOR_VERSION == 2
  return PyFloat_Check(obj) || PyLong_Check(obj) || PyInt_Check(obj);
#else
  return PyFloat_Check(obj) || PyLong_Check(obj);
#endif
}

inline double THPUtils_unpackDouble(PyObject* obj) {
  if (PyFloat_Check(obj)) {
    return PyFloat_AS_DOUBLE(obj);
  }
  if (PyLong_Check(obj)) {
    int overflow;
    long long value = PyLong_AsLongLongAndOverflow(obj, &overflow);
    if (overflow != 0) {
      throw std::runtime_error("Overflow when unpacking double");
    }
    if (value > DOUBLE_INT_MAX || value < -DOUBLE_INT_MAX) {
      throw std::runtime_error("Precision loss when unpacking double");
    }
    return (double)value;
  }
#if PY_MAJOR_VERSION == 2
  if (PyInt_Check(obj)) {
    return (double)PyInt_AS_LONG(obj);
  }
#endif
  throw std::runtime_error("Could not unpack double");
}
