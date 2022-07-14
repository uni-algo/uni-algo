New Unicode version checklist:

1. Change Unicode version in: gen/*.h_blank files
2. Generate data files using gen/main.cpp
3. Copy and replace generated files to src/impl
5. Generate gen/nm_result.md
6. Change Unicode version in: src/impl/impl_unicode_version.h
7. Change collation names in sqlite/unisqlite.c
8. Change library minor version + 1 in: src/impl/impl_cpp_lib_version.h
9. Change library minor version in: CMakeLists.txt
10. Compile and run tests.

3 Commits to Github must be done:
1. Files from step 1, 3 with text: Regenerate Unicode data files
2. Files from steps 5, 6, 7 with text: Update to Unicode A.B.C
3. Files from step 8, 9 with text: Bump minor version to X.Y.Z
Add tag vX.Y.Z to the latest commit.