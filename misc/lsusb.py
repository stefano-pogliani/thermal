#!/usr/bin/env python
# Python 3 script to list USB devices and their bus & port numbers.
from usb.core import find


def column_widths(lines):
    return (
            max(len(line[0]) for line in lines) + 2,
            max(len(line[1]) for line in lines) + 2,
            max(len(line[2]) for line in lines) + 2,
            max(len(line[3]) for line in lines) + 2,
            max(len(line[4]) for line in lines) + 2
    )


def device_line(device):
    manufacturer = device.manufacturer or 'Unkown manufacturer'
    product = device.product or 'Unkown product'
    return (
            str(device.bus),
            str(device.port_number),
            '{0} {1}'.format(manufacturer, product),
            '%#06x' % (device.idVendor,),
            '%#06x' % (device.idProduct,)
    )


def header():
    return (
            'Bus', 'Port', 'Description',
            'Vendor ID', 'Product ID'
    )


def print_line(line, widths):
    formatted = ''
    for (column, width) in zip(line, widths):
        padding = width - len(column)
        formatted += "{0}{1}".format(column, ' ' * padding)
    print(formatted)


def main():
    devices = find(find_all=True)
    lines = [header()]

    for device in devices:
        try:
            lines.append(device_line(device))
        except Exception:
            pass

    widths = column_widths(lines)
    for line in lines:
        print_line(line, widths)


if __name__ == '__main__':
    main()
