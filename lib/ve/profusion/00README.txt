How to use Profusion-CMU


1. Before using Profusion-CMU = right after the OS boot-up

   1.1. PCI device check

        Run 'lspci' and see if Profusion-CMU appears or not.

          $ lspci | grep 1066
          04:00.0 Multimedia controller: Device 1a63:1066
          26:00.0 Multimedia controller: Device 1a63:1066

        If your PC has two Profusion-CMUs connected, you should get
        two lines as shown above (PCI buses at 04 and 26). Otherwise, check 
        the cable connection and power supply to the PCI card.

        If your Profusion-CMU is connected via PCI-extender, Linux (and BIOS) 
        may not find it automatically. In this case, try boot with the 
        kernel parameters like

          pci=assign-busses pci=lastbus=N

        to force Linux kernel scan up to N-th bus (for Dell T5500 + Adnaco, 
        use N=28). You can edit the kernel parameter by pressing SHIFT key at
        boot time and entering GRUB.

        Once you find a nice kernel parameter, write it in /etc/default/grub
        as

          GRUB_CMDLINE_LINUX="pci=assign-busses,lastbus=28,ioapicreroute,check_enable_amd_mmconf"

        and run update-grub as root.

          $ sudo update-grub

        This will make the OS boot with this parameter by default.


   1.2. Kernel module

        You need to load kernel module vp1066.ko by

          $ cd lib/libviewplus/linux
          $ make
          $ sudo ./vpcpro_load

        The shell script vpcpro_load does

          - insmod vp1066.ko and
          - mknod /dev/vpcpro[0-3]

        so if you are really sure about what you are doing, you can automate
        this process by writing your own rc.local or udev rule.

        NOTICE: This kernel module can be loaded even when no Profusion-CMUs
        are connected / visible. Successful driver loading tells nothing
        about the camera status.


2. Capture


