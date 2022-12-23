<!--
.\" Copyright (C) 2019 VirtualSquare. Project Leader: Renzo Davoli
.\"
.\" This is free documentation; you can redistribute it and/or
.\" modify it under the terms of the GNU General Public License,
.\" as published by the Free Software Foundation, either version 2
.\" of the License, or (at your option) any later version.
.\"
.\" The GNU General Public License's references to "object code"
.\" and "executables" are to be interpreted as the output of any
.\" document formatting or typesetting system, including
.\" intermediate and printed output.
.\"
.\" This manual is distributed in the hope that it will be useful,
.\" but WITHOUT ANY WARRANTY; without even the implied warranty of
.\" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
.\" GNU General Public License for more details.
.\"
.\" You should have received a copy of the GNU General Public
.\" License along with this manual; if not, write to the Free
.\" Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
.\" MA 02110-1301 USA.
.\"
-->
# NAME

iplink(1) -- add/delete generic interfaces

# SYNOPSIS

`iplink add` [ [ `name` ] _NAME_ ] [ `index` _IDX_ ] [ `data` _DATA_ ] `type` _TYPE_

`iplink delete` [ _DEVICE_ \| `dev` _DEVICE_ ] [ `index` _IDX_ ] [ `type` _TYPE_ ]

`iplink del` [ _DEVICE_ \| `dev` _DEVICE_ ] [ `index` _IDX_ ] [ `type` _TYPE_ ]

# DESCRIPTION

`iplink` allow users to add or delete interfaces whose types are not supported by `ip-link`(8).

### iplink add
`iplink add` adds a generic interface of type _TYPE_.

   `name` _NAME_
: sets the name of the new interface. (the tag `name` can be omitted).

  `index` _IDX_
: specifies the desired index of the new virtual device.

  `data` _DATA_ 
: adds specific data to configure the interface. The meaning depends upon the type of the interface.

###  iplink del -- iplink delete
`iplink del` or `iplink delete` deletes an interface. At least one option between `dev` or `index` must
be selected.

  `dev` _DEVICE_
: select the interface by its name. (the tag `dev` can be omitted).

  `index` _IDX_
: select the interface by its index.

  `type` _TYPE_
: specify the interface type (currently ignored).

# EXANPLE

In a `umvu` session:
```
$ vu_insmod vunet
$ mount -t vunetpicox none /dev/n
$ vustack /dev/n iplink add vde1 type vde data vxvde://234.0.0.1
$ vustack /dev/n ip link
2090479455: loop: *UP* mtu 1500
: link/netrom 00:00:00:00:00:00 brd ff:ff:ff:ff:ff:ff
2090826453: vde1: *UP* mtu 1500
: link/netrom 80:00:47:2f:13:d8 brd ff:ff:ff:ff:ff:ff
```

# SEE ALSO
umvu(1), ip(8), ip-link(8)

# AUTHOR
VirtualSquare. Project leader: Renzo Davoli.
