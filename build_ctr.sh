rm -rf ../ctr
mkdir -p ../ctr/bin ../ctr/include ../ctr/lib

CONFIG="precompiled_runtime_product"

OUT="out/ReleaseXARMV6"

LIBS="$OUT/obj/runtime/libdart_$CONFIG.a $OUT/obj/runtime/platform/libdart_platform_$CONFIG.a $OUT/obj/runtime/vm/libdart_lib_$CONFIG.a $OUT/obj/runtime/vm/libdart_vm_$CONFIG.a $OUT/obj/runtime/third_party/double-conversion/src/libdouble_conversion.a"

# Build Dart

./tools/build.py -m release -a armv6 -j8 libdart_precompiled_runtime_product

cp -t ../ctr/lib $LIBS

# Build gen_snapshot

ninja -C out/ProductIA32 -v gen_snapshot

cp -t ../ctr/bin out/ProductIA32/gen_snapshot

# Copy headers

cp -t ../ctr/include runtime/include/*.h

# Compile Kernel DILL compiler to a DILL

./tools/sdks/dart-sdk/bin/dart --snapshot=../ctr/bin/kernel.snapshot pkg/vm/bin/gen_kernel.dart
cp -t ../ctr/bin out/ReleaseXARMV6/vm_platform_strong.dill out/ReleaseXARMV6/vm_outline_strong.dill

# Note the required Dart version

cp tools/sdks/dart-sdk/version ../ctr/req_dart_sdk_version

# Make .tar.gz

tar -cvzf ../libdart_sdk_ctr.tar.gz ../ctr
