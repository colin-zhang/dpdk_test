#!/bin/sh
set -e

FIRST_DIR=`pwd`
VENDORS_DIR=$FIRST_DIR/vendors
OUTPUT_DIR=$FIRST_DIR/output
INSTALL_DIR=$FIRST_DIR/output/local
RTE_SDK_TARGET=x86_64-native-linuxapp-gcc

# curl -LO https://fast.dpdk.org/rel/dpdk-16.11.5.tar.xz
# curl -LO https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
# curl -LO https://github.com/intel/hyperscan/archive/v4.7.0.tar.gz#/hyperscan-4.7.0.tar.gz

pre_env()
{
    if [[ ! -d $VENDORS_DIR ]]; then
        mkdir -p $VENDORS_DIR
    fi

    if [[ ! -d $INSTALL_DIR ]]; then
        mkdir -p $INSTALL_DIR
    fi
}

#[dpdk 17.11.2] (https://fast.dpdk.org/rel/dpdk-17.11.2.tar.xz)
build_dpdk()
{
    build_dir=$VENDORS_DIR/dpdk-stable-17.11.2

    if [[ ! -d $build_dir ]]; then
        wget https://fast.dpdk.org/rel/dpdk-17.11.2.tar.xz   -O  $VENDORS_DIR/dpdk-17.11.2.tar.xz
        tar -xf $VENDORS_DIR/dpdk-17.11.2.tar.xz -C $VENDORS_DIR
        #statements
    fi

    cd ${build_dir}
    make -j4 install T=$RTE_SDK_TARGET CONFIG_RTE_KNI_KMOD=n CONFIG_RTE_LIBRTE_KNI=n   DESTDIR=$OUTPUT_DIR/dpdk
    cd  $FIRST_DIR
}

get_hyperscan()
{
    if [[ ! -f  $VENDORS_DIR/boost_1_67_0.tar.gz ]]; then
        wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz  -O  $VENDORS_DIR/boost_1_67_0.tar.gz
    fi

    if [[ ! -d  $VENDORS_DIR/boost_1_67_0 ]]; then
        tar -xvf $VENDORS_DIR/boost_1_67_0.tar.gz -C $VENDORS_DIR
    fi

    cd  $VENDORS_DIR
    if [[ ! -d  hyperscan ]]; then
        git clone https://github.com/intel/hyperscan
    else
        cd hyperscan && git pull
    fi
    cd  $FIRST_DIR
}

build_hyperscan()
{
    build_dir=$VENDORS_DIR/hyperscan/build
    mkdir -p ${build_dir} && cd  ${build_dir}
    cmake -DBOOST_ROOT=$VENDORS_DIR/boost_1_67_0  -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR ..
    make -j4 install
    cd  $FIRST_DIR
}

build_libdnet()
{
    build_dir=$VENDORS_DIR/libdnet
    cd  ${build_dir}
    ./configure --enable-static --prefix=$INSTALL_DIR
    make  && make install
    cd  $FIRST_DIR
}    

#[libnet-1.2-rc3] (https://github.com/sam-github/libnet/archive/libnet-1.2-rc3.tar.gz)
#[libnet-1.2.rc3] (https://sourceforge.net/projects/libnet-dev/files/libnet-1.2-rc3.tar.gz)
build_libnet()
{
    build_dir=$VENDORS_DIR/libnet-1.2-rc3

    if [[ ! -d $build_dir ]]; then
        wget https://sourceforge.net/projects/libnet-dev/files/libnet-1.2-rc3.tar.gz   -O  $VENDORS_DIR/libnet-1.2-rc3.tar.gz
        tar -xf $VENDORS_DIR/libnet-1.2-rc3.tar.gz -C $VENDORS_DIR
        #statements
    fi

    cd  ${build_dir}
    ./configure --enable-static --prefix=$INSTALL_DIR
    make  && make install
    cd  $FIRST_DIR
}

pre_env
get_hyperscan
build_hyperscan
build_libnet
build_dpdk
