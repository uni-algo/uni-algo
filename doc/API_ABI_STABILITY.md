## API Stability

The API of the library follows [Semantic Versioning](https://semver.org) rules.<br>
This means the API is stable across major versions.<br>
Note that `detail` namespace is not a part of the public API.

The API of extensions of the library (files inside `include/uni_algo/ext` directory) is only stable across minor versions.<br>
They are not a part of the API of the library and use different main namespace: `unx` instead of `una`.

## ABI Stability

The ABI of the library is only stable across minor versions for now.<br>
This means internal representation of objects of the library might change in minor versions and passing them outside should be avoided.<br>
The only guarantee is the ABI will never change across patch versions.

When compiling static/shared library it must be used with the exact same config defines it was compiled.<br>
The only exception is config defines that do not start with `UNI_ALGO_ENABLE/DISABLE`, those defines do not affect object file.<br>
Header-only version is not affected by this because it always used with the exact same config defines.
