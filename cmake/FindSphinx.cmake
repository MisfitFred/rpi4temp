
find_program(SPHINX_EXEC
             NAMES sphinx-build
             DOC "Path to sphinx exec")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Sphinx
                                  "sphinx-build not found"
                                  SPHINX_EXEC)