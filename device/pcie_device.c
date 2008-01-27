/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pcie.h>

static void pcie_tune_dev(struct device *dev)
{
	unsigned int cap;
	cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!cap) {
		/* error... */
		return;
	}
	printk(BIOS_DEBUG, "PCIe: tuning %s\n", dev_path(dev));
#warning "IMPLEMENT PCI EXPRESS TUNING"
}

unsigned int pcie_scan_bus(struct bus *bus, unsigned int min_devfn,
			   unsigned int max_devfn, unsigned int max)
{
	struct device *child;
	max = pci_scan_bus(bus, min_devfn, max_devfn, max);
	for (child = bus->children; child; child = child->sibling) {
		if ((child->path.u.pci.devfn < min_devfn) ||
		    (child->path.u.pci.devfn > max_devfn)) {
			continue;
		}
		pcie_tune_dev(child);
	}
	return max;
}

unsigned int pcie_scan_bridge(struct device *dev, unsigned int max)
{
	return do_pci_scan_bridge(dev, max, pcie_scan_bus);
}

/** Default device operations for PCI Express bridges. */
static const struct pci_operations pcie_bus_ops_pci = {
	.set_subsystem = 0,
};

const struct device_operations default_pcie_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = pcie_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pcie_bus_ops_pci,
};
