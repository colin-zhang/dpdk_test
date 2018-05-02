#### dpdk
```
sudo modprobe uio_pci_generic
sudo insmod kmod/igb_uio.ko
```

```
mkdir /mnt/huge
echo 512 >/sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
mount -t hugetlbfs nodev /mnt/huge
```
