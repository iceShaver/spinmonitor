ARG="$1"
if [ "$ARG" = "debug" ]; then
  cd cmake-build-debug && cmake --DCMAKE_BUILD_TYPE=Debug .. && make -j8
elif [ "$ARG" = "release" ]; then
  cd cmake-build-release &&  cmake --DCMAKE_BUILD_TYPE=Release .. && make -j8
else
  echo "Bad arg"
fi;


