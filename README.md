# vunetioth
A VUOS vunet module for ioth

`vunetioth` is a `vunet` submodule: any `libioth` supported networking stack can be
*mounted* in VUOS.

**vumount -t vunetioth** [**-o** *options*]  *source* *destination*

**mount -t vunetioth** [**-o** *options*]  *source* *destination*

The arguments for `mount` or `vumount` are:

* *source*: the name of the stack implementation
* *destination*: the mountpoint, i.e. the vunet special file
* *options*: options of the stack and interface definitions:
    * **if=***VNL*, e.g. `if=vde:///tmp/sw` or `if=eth0:vxvde://234.0.0.1`. Interfaces without
names (e.g. eth0) are usually named `vde0`, `vde1` etc...
    * **if** can be omitted, any option without assignment symbols including
a VNL is interpreted as an interface definition e.g. `vde:///tmp/sw` or `eth0:vxvde://234.0.0.1`


### Example:

In a `umvu` session:
```
$$ vu_insmod vunet
$$ vumount -t vunetioth -o vxvde://234.0.0.1 vdestack /dev/net/mystack
```
The command vustack gives access to the new stack.
```
$$ vustack /dev/net/mystack ip addr
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: vde0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN group default qlen 1000
    link/ether 8e:a6:fd:a1:db:38 brd ff:ff:ff:ff:ff:ff
```

The *source* argument of the `mount` command selects the implementation of the networking stack:
```
$ vu_insmod vunet
$  vumount -t vunetioth -o vxvde://234.0.0.1 picox /dev/net/mystack
$ vustack /dev/net/mystack ip addr
2090479455: loop: <UP> mtu 1500
    link/netrom 00:00:00:00:00:00 brd ff:ff:ff:ff:ff:ff
    inet6 ::1/128 scope host dynamic loop
2090826452: vde0: <UP> mtu 1500
    link/netrom 80:00:0d:ee:cf:1e brd ff:ff:ff:ff:ff:ff
    inet6 fe80::8200:dff:feee:cf1e/64 scope link dynamic vde0
```

Note: `picoxnet` uses large integers as interface identifiers. It is not an error.

Interface can be added at run-time by `iplink` (if the stack supports this feature):

```
$ vu_insmod vunet
$  vumount -t vunetioth picox /dev/net/mystack
$ vustack /dev/net/mystack bash
$ ip addr
2090479455: loop: <UP> mtu 1500
    link/netrom 00:00:00:00:00:00 brd ff:ff:ff:ff:ff:ff
    inet6 ::1/128 scope host dynamic loop
$ iplink add vde0 type vde data vxvde://
$ ip addr
2090479455: loop: <UP> mtu 1500
    link/netrom 00:00:00:00:00:00 brd ff:ff:ff:ff:ff:ff
    inet6 ::1/128 scope host dynamic loop
2090826452: vde0: <UP> mtu 1500
    link/netrom 80:00:3e:b4:46:a1 brd ff:ff:ff:ff:ff:ff
    inet6 fe80::8200:3eff:feb4:46a1/64 scope link dynamic vde0
```
