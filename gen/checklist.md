New Unicode version checklist:

1. Change Unicode version in gen_header variable in gen/gen.h file
2. Generate data files using gen/main.cpp and gen/blank files
3. Copy and replace generated files to include/uni_algo/impl/data
5. Generate gen/nm_result.txt (x86 Release with UNI_ALGO_DISABLE_SYSTEM_LOCALE define)
6. Change Unicode version in: include/uni_algo/impl/impl_unicode_version.h
7. Change collation names in sqlite/unisqlite.c
8. Change Unicode test files version in: test/CMakeLists.txt
10. Change library minor version + 1 in: src/impl/impl_cpp_lib_version.h
11. Change library minor version + 1 in: CMakeLists.txt
12. Compile and run tests

3 Commits to Git must be done:
1. Files from step 1, 3 with text: Regenerate Unicode data files
2. Files from steps 5, 6, 7, 8 with text: Update to Unicode A.B.C
3. Files from step 10, 11 with text: Bump minor version to X.Y.Z

Add annotated tag vX.Y.Z to the latest commit.
