Procedure to Build CR7 loader

1	Download 32bit compiler gcc-linaro-5.1-2015.08-x86_64_arm-eabi.tar.xz to local folder and unpack:

	$ wget https://releases.linaro.org/components/toolchain/binaries/5.1-2015.08/arm-eabi/gcc-linaro-5.1-2015.08-x86_64_arm-eabi.tar.xz .
	$ tar xfJ gcc-linaro-5.1-2015.08-x86_64_arm-eabi.tar.xz

2	Build for boot CR7 and CA53:

	$ export CROSS_COMPILE=./gcc-linaro-5.1-2015.08-x86_64_arm-eabi/bin/arm-eabi-
	$ make RCAR_DRAM_SPLIT=0 RCAR_KICK_MAIN_CPU=2 LSI=V3H RCAR_SECURE_BOOT=0

3	Deploy the following files:

	$ cp bootparam_sa0.srec ../../../../build/tmp/deploy/images/condor/bootparam_sa0.srec
	$ cp cr7_loader.srec ../../../../build/tmp/deploy/images/condor/cr7-condor.srec
	$ cp cert_header_sa3.srec ../../../../build/tmp/deploy/images/condor/cert_header_sa3.srec

	$ cp bootparam_sa0.bin ../../../../build/tmp/deploy/images/condor/bootparam_sa0.bin
	$ cp cr7_loader.bin ../../../../build/tmp/deploy/images/condor/cr7-condor.bin
	$ cp cert_header_sa3.bin ../../../../build/tmp/deploy/images/condor/cert_header_sa3.bin





