Feucht
======

[RIOT][1] application utilising an [HDC1000][2] sensor.

Backends
--------

The humidity value read from the [HDC1000][2] is transmitted to a
central server. The protocol used for this transmission can be
configured using the `FEUCHT_PROTO` environment variable. Currently
valid values are:

1. `9P` for using the [9P][5] protocol with [9RIOT][3]
2. `COAP` for using the [CoAP][6] protocol with [gcoap][4]

Besides the address and port of the protocol server need to be
configured using the `FEUCHT_HOST` and `FEUCHT_PORT` environment
variable. The latter is optional.

Known Bugs
----------

The `9P` backend is currently broken because `9P` is not shipped as a
[RIOT][1] pkg at the moment.

License
-------

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.

[1]: http://riot-os.org/
[2]: http://www.ti.com/lit/ds/symlink/hdc1000.pdf
[3]: https://github.com/nmeum/9RIOT
[4]: http://riot-os.org/api/group__net__gcoap.html
[5]: http://9p.cat-v.org/
[6]: https://tools.ietf.org/html/rfc7228
