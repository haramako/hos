#include "acpi.h"

#include "acpi/rsdt.h"
#include "boot_param.h"

void *acpi_find_rsdt(const char *signature) {
	ACPI_XSDT *xsdt = g_boot_param->rsdt->xsdt;
	const int rsdt_num = (xsdt->h.length - kDescriptionHeaderSize) >> 3;
	for (int i = 0; i < rsdt_num; i++) {
		if (strncmp(xsdt->entry[i]->signature, signature, 4) == 0) {
			return (void *)xsdt->entry[i];
		}
	}
	return NULL;
}
