set -eu

FIRST_DIR=`pwd`
BUILD_DIR=$FIRST_DIR/output/build

if [[ ! -d $BUILD_DIR ]]; then
    mkdir -p $BUILD_DIR
fi

#cd output/build && cmake $FIRST_DIR -DEXECUTABLE_OUTPUT_PATH=$FIRST_DIR/output && make -j2
cd $BUILD_DIR && cmake $FIRST_DIR  && make -j2

