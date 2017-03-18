#ifndef MOEM_GUI_COMMON_HPP
#define MOEM_GUI_COMMON_HPP

#include <Eigen/Core>
#include <algorithm>

#if defined(MOEM_GUI_SHARED)
#  if defined(_WIN32)
#    if defined(MOEM_GUI_BUILD)
#      define MOEM_GUI_EXPORT __declspec(dllexport)
#    else
#      define MOEM_GUI_EXPORT __declspec(dllimport)
#    endif
#  elif defined(MOEM_GUI_BUILD)
#    define MOEM_GUI_EXPORT __attribute__ ((visibility("default")))
#  else
#    define MOEM_GUI_EXPORT
#  endif
#else
/**
* If the build flag ``MOEM_GUI_SHARED`` is defined, this directive will expand
* to be the platform specific shared library import / export command depending
* on the compilation stage.  If undefined, it expands to nothing. **Do not**
* define this directive on your own.
*/
#    define MOEM_GUI_EXPORT
#endif

namespace moem {
	namespace gui {
		using PointType = Eigen::Vector2f;
		using SizeType = Eigen::Vector2f;

		struct once_true
		{
			typedef bool result_type;

			template <typename InputIterator>
			bool operator()(InputIterator first, InputIterator last) const
			{
				std::vector<bool> v(first, last);
				return std::find(v.begin(), v.end(), true) != v.end();
			}
		};
	}
}

#endif