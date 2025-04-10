#include "binders.hpp"

namespace nb = nanobind;

NB_MODULE(_conduitpy, m) {
    m.doc() = "A wrapper around 6328's conduit library";

    // Register the PyDsReader class
    nb::class_<PyDsReader>(m, "DsReader")
        .def(nb::init<>())
        .def("read", &PyDsReader::read, "Read driver station data");

    // Register the PyPDPReader class
    nb::class_<PyPDPReader>(m, "PDPReader")
        .def(nb::init<>())
        .def("read", &PyPDPReader::read, "Read power distribution panel data");

    // Register the PySystemReader class
    nb::class_<PySystemReader>(m, "SystemReader")
        .def(nb::init<>())
        .def("read", &PySystemReader::read, "Read system data");

    // Register the PyWPILibIO class
    nb::class_<PyWPILibIO>(m, "WPILibIO")
        .def(nb::init<>())
        .def("start", &PyWPILibIO::start, "Start the WPILib IO system")
        .def("capture_data", &PyWPILibIO::capture_data, "Capture all data from WPILib IO");
}