#!/bin/bash

ESC="\x1B"
COLOR="${ESC}[32m"
RESET="${ESC}[0m"

declare -a build_types=("Release" "Debug")

rm build* -rf

find *.cmake -print0 | while IFS= read -r -d '' toolchain; do
  toolchain_name="${toolchain%.cmake}"

  echo -e "${COLOR}BUILDING ${toolchain_name}${RESET}"

  for build_type in "${build_types[@]}"; do
    build_dir="build_${toolchain_name}_${build_type}"

    cmake -S . -B ${build_dir} -G Ninja -DCMAKE_BUILD_TYPE=${build_type} --toolchain ${toolchain}
    cmake --build ${build_dir} --parallel
  done

  echo ""
done

