#include "bindings.hpp"

PyObject* py::symbols::address(core::Core& core, PyObject* args)
{
    auto obj    = static_cast<PyObject*>(nullptr);
    auto module = static_cast<const char*>(nullptr);
    auto symbol = static_cast<const char*>(nullptr);
    auto ok     = PyArg_ParseTuple(args, "Sss", &obj, &module, &symbol);
    if(!ok)
        return nullptr;

    const auto opt_proc = py::from_bytes<proc_t>(obj);
    if(!opt_proc)
        return nullptr;

    module             = module ? module : "";
    symbol             = symbol ? symbol : "";
    const auto opt_ptr = ::symbols::address(core, *opt_proc, module, symbol);
    if(!opt_ptr)
        Py_RETURN_NONE;

    return PyLong_FromUnsignedLongLong(*opt_ptr);
}

PyObject* py::symbols::struc_names(core::Core& core, PyObject* args)
{
    auto obj    = static_cast<PyObject*>(nullptr);
    auto module = static_cast<const char*>(nullptr);
    auto ok     = PyArg_ParseTuple(args, "Ss", &obj, &module);
    if(!ok)
        return nullptr;

    const auto opt_proc = py::from_bytes<proc_t>(obj);
    if(!opt_proc)
        return nullptr;

    module       = module ? module : "";
    auto py_list = PyList_New(0);
    if(!py_list)
        return nullptr;

    PYREF(py_list);
    ::symbols::struc_names(core, *opt_proc, module, [&](std::string_view name)
    {
        const auto py_name = PyUnicode_FromStringAndSize(name.data(), name.size());
        if(!py_name)
            return;

        PYREF(py_name);
        PyList_Append(py_list, py_name);
    });
    Py_INCREF(py_list);
    return py_list;
}

PyObject* py::symbols::struc_size(core::Core& core, PyObject* args)
{
    auto obj    = static_cast<PyObject*>(nullptr);
    auto module = static_cast<const char*>(nullptr);
    auto struc  = static_cast<const char*>(nullptr);
    auto ok     = PyArg_ParseTuple(args, "Sss", &obj, &module, &struc);
    if(!ok)
        return nullptr;

    const auto opt_proc = py::from_bytes<proc_t>(obj);
    if(!opt_proc)
        return nullptr;

    module              = module ? module : "";
    struc               = struc ? struc : "";
    const auto opt_size = ::symbols::struc_size(core, *opt_proc, module, struc);
    if(!opt_size)
        return py::fail_with(nullptr, PyExc_RuntimeError, "unable to read struc size");

    return PyLong_FromUnsignedLongLong(*opt_size);
}

PyObject* py::symbols::struc_members(core::Core& core, PyObject* args)
{
    auto obj    = static_cast<PyObject*>(nullptr);
    auto module = static_cast<const char*>(nullptr);
    auto struc  = static_cast<const char*>(nullptr);
    auto ok     = PyArg_ParseTuple(args, "Sss", &obj, &module, &struc);
    if(!ok)
        return nullptr;

    const auto opt_proc = py::from_bytes<proc_t>(obj);
    if(!opt_proc)
        return nullptr;

    module       = module ? module : "";
    struc        = struc ? struc : "";
    auto py_list = PyList_New(0);
    if(!py_list)
        return nullptr;

    PYREF(py_list);
    ::symbols::struc_members(core, *opt_proc, module, struc, [&](std::string_view name)
    {
        const auto py_name = PyUnicode_FromStringAndSize(name.data(), name.size());
        if(!py_name)
            return;

        PYREF(py_name);
        PyList_Append(py_list, py_name);
    });
    Py_INCREF(py_list);
    return py_list;
}

PyObject* py::symbols::member_offset(core::Core& core, PyObject* args)
{
    auto obj    = static_cast<PyObject*>(nullptr);
    auto module = static_cast<const char*>(nullptr);
    auto struc  = static_cast<const char*>(nullptr);
    auto member = static_cast<const char*>(nullptr);
    auto ok     = PyArg_ParseTuple(args, "Ssss", &obj, &module, &struc, &member);
    if(!ok)
        return nullptr;

    const auto opt_proc = py::from_bytes<proc_t>(obj);
    if(!opt_proc)
        return nullptr;

    module                = module ? module : "";
    struc                 = struc ? struc : "";
    member                = member ? member : "";
    const auto opt_offset = ::symbols::member_offset(core, *opt_proc, module, struc, member);
    if(!opt_offset)
        return py::fail_with(nullptr, PyExc_RuntimeError, "unable to read struc offset");

    return PyLong_FromUnsignedLongLong(*opt_offset);
}

PyObject* py::symbols::string(core::Core& core, PyObject* args)
{
    auto obj = static_cast<PyObject*>(nullptr);
    auto ptr = uint64_t{};
    auto ok  = PyArg_ParseTuple(args, "SK", &obj, &ptr);
    if(!ok)
        return nullptr;

    const auto opt_proc = py::from_bytes<proc_t>(obj);
    if(!opt_proc)
        return nullptr;

    const auto sym = ::symbols::find(core, *opt_proc, ptr);
    const auto txt = ::symbols::to_string(sym);
    return PyUnicode_FromStringAndSize(txt.data(), txt.size());
}
