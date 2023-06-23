add_test( max31865Test.readRegister /home/matthias/work/spi_test/build/max31865/max31865_test [==[--gtest_filter=max31865Test.readRegister]==] --gtest_also_run_disabled_tests)
set_tests_properties( max31865Test.readRegister PROPERTIES WORKING_DIRECTORY /home/matthias/work/spi_test/build/max31865 SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( max31865_test_TESTS max31865Test.readRegister)
