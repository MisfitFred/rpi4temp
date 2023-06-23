add_test( faultStatusTest.updateFaultStatus /home/matthias/work/spi_test/build/max31865/faultStatus_test [==[--gtest_filter=faultStatusTest.updateFaultStatus]==] --gtest_also_run_disabled_tests)
set_tests_properties( faultStatusTest.updateFaultStatus PROPERTIES WORKING_DIRECTORY /home/matthias/work/spi_test/build/max31865 SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( faultStatus_test_TESTS faultStatusTest.updateFaultStatus)
