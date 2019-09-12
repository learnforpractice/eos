## SORT .cpp by most likely to change / break compile
add_library( boost_1_67_0
             STATIC
             "iostreams/src/zlib.cpp"
             "filesystem/src/codecvt_error_category.cpp"
             "filesystem/src/operations.cpp"
             "filesystem/src/path_traits.cpp"
             "filesystem/src/path.cpp"
             "filesystem/src/portability.cpp"
             "filesystem/src/unique_path.cpp"
             "filesystem/src/utf8_codecvt_facet.cpp"
             "system/src/error_code.cpp"
             "system/src/error_code.cpp"
             "chrono/src/chrono.cpp"
             "chrono/src/process_cpu_clocks.cpp"
             "chrono/src/thread_clock.cpp"
             "date_time/src/gregorian/date_generators.cpp"
             "date_time/src/gregorian/greg_month.cpp"
             "date_time/src/gregorian/greg_weekday.cpp"
             "date_time/src/gregorian/gregorian_types.cpp"
             "date_time/src/posix_time/posix_time_types.cpp"
             "program_options/src/cmdline.cpp"

             "program_options/src/convert.cpp"
             "program_options/src/parsers.cpp"
             "program_options/src/split.cpp"
             "program_options/src/value_semantic.cpp"
             "program_options/src/winmain.cpp"
             "program_options/src/config_file.cpp"
             "program_options/src/options_description.cpp"
             "program_options/src/positional_options.cpp"
             "program_options/src/utf8_codecvt_facet.cpp"
             "program_options/src/variables_map.cpp"

             "test/src/compiler_log_formatter.cpp"
             "test/src/debug.cpp"
             "test/src/decorator.cpp"
             "test/src/execution_monitor.cpp"
             "test/src/framework.cpp"
             "test/src/plain_report_formatter.cpp"
             "test/src/progress_monitor.cpp"
             "test/src/results_collector.cpp"
             "test/src/results_reporter.cpp"
             "test/src/test_framework_init_observer.cpp"
             "test/src/test_tools.cpp"
             "test/src/test_tree.cpp"
             "test/src/unit_test_log.cpp"
             "test/src/unit_test_main.cpp"
             "test/src/unit_test_monitor.cpp"
             "test/src/unit_test_parameters.cpp"
             "test/src/junit_log_formatter.cpp"
             "test/src/xml_log_formatter.cpp"
             "test/src/xml_report_formatter.cpp"
             )

#target_link_libraries( boost_1_67_0 PUBLIC fc )

target_include_directories( boost_1_67_0
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/includes"
                            )

