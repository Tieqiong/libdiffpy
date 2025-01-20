#include <stdexcept>
#include <boost/throw_exception.hpp>

namespace boost {
    void throw_exception(std::exception const & e)
    {
        // You can also customize the behavior here if needed.
        throw e;
    }
}
