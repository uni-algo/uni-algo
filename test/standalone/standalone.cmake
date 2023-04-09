# Test that every file can be compiled as standalone include.
# Main test cannot test this because it includes all files in big main.cpp file.
# Also tests that all low-level files can be compiled with minimum dependencies.

target_sources(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/standalone/config.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/version.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/conv.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/locale.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/case.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/norm.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/prop.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/script.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ranges.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ranges_conv.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ranges_norm.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ranges_grapheme.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ranges_word.cpp"
    # Extensions
    "${PROJECT_SOURCE_DIR}/standalone/ext/ascii.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ext/translit/macedonian_to_latin_docs.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/ext/translit/japanese_kana_to_romaji_hepburn.cpp"
    # Low-level (just in case)
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_iter.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_conv.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_locale.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_case.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_case_locale.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_norm.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_prop.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_script.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_segment_grapheme.cpp"
    "${PROJECT_SOURCE_DIR}/standalone/impl/impl_segment_word.cpp"
)
