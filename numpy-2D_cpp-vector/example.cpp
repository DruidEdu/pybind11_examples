#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>

// -------------
// pure C++ code
// -------------

std::vector<double> length ( std::vector<double> x , std::vector<double> y )
{

  std::vector<double> output(x.size());

  for ( int i=0 ; i<(int)x.size() ; i++ )
    output[i] = pow(x[i]*y[i],.5);

  for ( int i=0 ; i<(int)x.size() ; i++ )
    printf("%16.8e,%16.8e,%16.8e\n",x[i],y[i],output[i]);

  return output;

}

// ----------------
// Python interface
// ----------------

namespace py = pybind11;

// wrap C++ function with NumPy array IO
py::array_t<double> py_length(py::array_t<double, py::array::c_style | py::array::forcecast> array) {

  // allocate std::vector (to pass to the C++ function)
  std::vector<double> x(array.shape()[0]);
  std::vector<double> y(array.shape()[0]);

  // copy py::array -> std::vector
  for ( int i=0 ; i<(int)array.shape()[0] ; i++ ) {
    x[i] = array.data()[i*array.shape()[1]+0];
    y[i] = array.data()[i*array.shape()[1]+1];
  }

  // call pure C++ function
  std::vector<double> result_vec = length(x,y);

  // allocate py::array (to pass the result of the C++ function to Python)
  auto result        = py::array_t<double>(result_vec.size());
  auto result_buffer = result.request();
  double *result_ptr = (double *) result_buffer.ptr;

  // copy std::vector -> py::array
  std::memcpy(result_ptr,result_vec.data(),result_vec.size()*sizeof(double));

  return result;

}

// wrap as Python module
PYBIND11_PLUGIN(example) {

  py::module m("example", "pybind11 example plugin");

  m.def("length", &py_length, "Calculate the length of an array of vectors");

  return m.ptr();

}

