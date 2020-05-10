#include "ahci.h"

#include "asm.h"

AHCI *g_ahci;

static void start_cmd(AHCI_HBA_PORT *port);

char to_hex(char n) {
	assert(n >= 0 && n < 16);
	if (n < 10) {
		return '0' + n;
	} else {
		return 'a' + (n - 10);
	}
}

char *dump_bytes(void *p_, size_t size) {
	assert(size < 0x10000);

	static char buf[8000];
	char *s = buf;
	uint8_t *p = (uint8_t *)p_;

	for (size_t i = 0; i < size; i++) {
		if (i % 16 == 0) {
			s += snprintf(s, 8, "%04x: ", (int)i);
		}
		*(s++) = to_hex(p[i] >> 4);
		*(s++) = to_hex(p[i] & 0x0f);
		*(s++) = ' ';
		if (i % 16 == 15) {
			*(s++) = '\n';
		}
	}

	*s++ = '\0';
	return buf;
}

uint64_t int_merge64(uint32_t high, uint32_t low) { return ((uint64_t)low) | ((uint64_t)high << 32); }

uint32_t uint64_high(uint64_t n) { return (uint32_t)(n >> 32); }

uint32_t uint64_low(uint64_t n) { return (uint32_t)n; }

#include "hpet.h"

void ahci_init() {
	PCI_DeviceInfo *pci = pci_find_device(0x01, 0x06);
	assert(pci);
	g_ahci = malloc(sizeof(g_ahci));
	bzero(g_ahci, sizeof(AHCI));

	AHCI *d = g_ahci;
	d->hba = (volatile AHCI_HBA *)((uint64_t)pci->reg.x.base_addr5);
	d->capability.raw = d->hba->cap;

	// klog("%s", dump_bytes((void *)(uint64_t)pci->reg.x.base_addr5, 0x200));
	klog("cap %x", d->capability.raw);

	AHCI_Capability *c = &d->capability;
	klog("ports %d", c->x.number_of_ports);

	d->ports = malloc(sizeof(AHCI_Port) * c->x.number_of_ports);
	bzero(d->ports, sizeof(AHCI_Port) * c->x.number_of_ports);
	for (int i = 0; i <= c->x.number_of_ports; i++) {
		AHCI_HBA_PORT *p = &d->hba->ports[i];
		AHCI_Port *port = &d->ports[i];
		port->info = p;
		port->command = (void *)int_merge64(p->clbu, p->clb);
		port->fis = (void *)int_merge64(p->fbu, p->fb);

		klog("%d: clb %p", i, port->command);
		klog("%d: fb  %p", i, port->fis);
		// klog("%s", dump_bytes((void*)p, sizeof(AHCI_HBA_PORT)));
		uint32_t state = p->ssts;
		klog("%d: 0x%03x", i, state);

		start_cmd(p);
	}

	// PIO IENTIFY_DEVICE
	{
		char *buf = (char *)(0x0000000011000000);
		int port_num = 0;
		int slot_num = 0;
		AHCI_Port *port = &d->ports[port_num];
		HBA_CMD_HEADER *command = port->command + slot_num;
		command->cfl = sizeof(FIS_PIO_SETUP) / sizeof(uint32_t);
		command->w = 0;
		command->prdtl = 1 + 1;

		HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(int_merge64(command->ctbau, command->ctba));
		bzero(cmdtbl, sizeof(HBA_CMD_TBL) + (command->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

		cmdtbl->prdt_entry[0].dba = uint64_low((uint64_t)buf);
		cmdtbl->prdt_entry[0].dbau = uint64_high((uint64_t)buf);
		klog("dba %x %x %p", cmdtbl->prdt_entry[0].dba, cmdtbl->prdt_entry[0].dbau, buf);
		cmdtbl->prdt_entry[0].dbc = 0x200;
		cmdtbl->prdt_entry[0].i = 1;

		FIS_REG_H2D *fis = (FIS_REG_H2D *)(&cmdtbl->cfis[0]);
		klog("%p", fis);
		bzero(fis, sizeof(FIS_REG_H2D));
		fis->fis_type = FIS_TYPE_REG_H2D;
		fis->c = 1;
		fis->command = ATACMD_IDENTIFY_DEVICE;
		fis->device = 0; // 1 << 4;

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
		while (port->info->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ))
			;

		volatile AHCI_HBA_PORT *info = port->info;
		info->ci = 1 << slot_num;

#define HBA_PxIS_TFES (1 << 30)

		// klog("wait");
		while (1) {
			if ((info->ci & (1 << slot_num)) == 0) break;
			klog("%x %x %x %x", info->ci, info->is, info->cmd, info->serr);
			if (info->is & HBA_PxIS_TFES) {
				kpanic("error");
			}
		}
		klog("wait end");
		klog("%s", dump_bytes(buf, 0x200));
	}
}

#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR 0x4000
#define HBA_PxCMD_CR 0x8000

// Start command engine
static void start_cmd(AHCI_HBA_PORT *port) {
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR)
		;

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}
