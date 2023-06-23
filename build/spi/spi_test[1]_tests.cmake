add_test( templateTest.simpleAdd /home/matthias/work/spi_test/build/spi/spi_test [==[--gtest_filter=templateTest.simpleAdd]==] --gtest_also_run_disabled_tests)
set_tests_properties( templateTest.simpleAdd PROPERTIES WORKING_DIRECTORY /home/matthias/work/spi_test/build/spi SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( spi_test_TESTS templateTest.simpleAdd)
