You can set up the CANbus interface using 

```
sudo ip link set can0 up type can bitrate 1000000
```

or by running `csp_client.exe` with the `-s` option (this will only work if you run as root)

## installing the CSP library
Clone the libCSP repo (https://github.com/libcsp/libcsp) and follow the install instructions in doc/INSTALL.md

In brief, in that directory:
```
cmake -B builddir -DCSP_BUILD_SAMPLES=ON
cmake --build builddir
```

then, as root:
```
cmake --install builddir
ldconfig
```

## installing the CANbus adapter driver
Download the Linux driver at https://www.systec-electronic.com/en/products/interfaces-gateways-amp-controls/sysworxx-usb-can-module1 and follow the install instructions in README.md.

as root:
```
dkms install .
```
