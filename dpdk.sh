#!/bin/sh
kPWD=`pwd`
kUpDir=`dirname $kPWD`
kOutPutDir=local
kRTE_SDK_TARGET=x86_64-native-linuxapp-gcc

vFLAG_BUILD_DPDK=0
vFLAG_MLX=0
vDPDK_CONFIG=" CONFIG_RTE_KNI_KMOD=n CONFIG_RTE_LIBRTE_KNI=n "
vDPDK_VERSON="dpdk-stable-16.11.7"

down_dpdk()
{
    dpdk_list="dpdk-18.05 dpdk-16.11.7 dpdk-17.11.3 dpdk-18.02.2"  
    mkdir -p $kUpDir/vendors

    for i in $dpdk_list; do
        if [[ ! -f $kUpDir/vendors/$i.tar.xz ]]; then
            wget https://fast.dpdk.org/rel/$i.tar.xz -O  $kUpDir/vendors/$i.tar.xz
        fi
        tar -xf $kUpDir/vendors/$i.tar.xz -C $kUpDir/vendors
    done
}


mk_dpdk_dir()
{
    dpdk_version=$1
    if [[ ! -d $kUpDir/vendors ]]; then
        echo -e "no vendors dir in $kUpDir !!!"
    fi

    rm -rf ./dpdk

    if [[ ! -d $kUpDir/vendors/$dpdk_version ]]; then
        echo "download first !!!";exit 1;
    else
        ln -s $kUpDir/vendors/$dpdk_version dpdk 
    fi
}


build_dpdk()
{
    build_dir=$kPWD/dpdk
    cd $build_dir
    echo "dpdk dir :$build_dir"
    make -j16 install T=$kRTE_SDK_TARGET DESTDIR=$kOutPutDir $vDPDK_CONFIG
    cd  $kPWD
}

parse_args()
{
    for arg in "$@"
    do
        length=${#arg}
        if [[ $length -gt $"3" ]]; then
            substr1=${arg:0:4}
            substr2=${arg:4}

            vFLAG_BUILD_DPDK=1

            if [[ "x"$substr2 == "x18.05" ]]; then
                vDPDK_VERSON="dpdk-18.05"

            elif [[ "x"$substr2 == "x18.02.2" ]]; then
                vDPDK_VERSON="dpdk-stable-18.02.2"

            elif [[ "x"$substr2 == "x17.11.3" ]]; then
                vDPDK_VERSON="dpdk-stable-17.11.3"

            elif [[ "x"$substr2 == "x16.11.7" ]]; then
                vDPDK_VERSON="dpdk-stable-16.11.7"

            else
                vDPDK_VERSON="dpdk-stable-16.11.7"
            fi
        fi
       
        if [ $arg == "mlx" ]; then    
            vFLAG_MLX=1
            vFLAG_BUILD_DPDK=1
        fi        

        if [ $arg == "download" ]; then    
            down_dpdk;exit 0;
        fi
        
    done
}

build()
{
    if [[ $vFLAG_MLX == 1 ]]; then
        vDPDK_CONFIG+=" CONFIG_RTE_LIBRTE_MLX4_PMD=y CONFIG_RTE_LIBRTE_MLX5_PMD=y "
    fi

    if [[ $vFLAG_BUILD_DPDK == 1 ]]; then
        mk_dpdk_dir $vDPDK_VERSON
        build_dpdk
    fi
}

parse_args $@;
echo "$vDPDK_VERSON"
build
