#include "ahci.h"

#include "asm.h"
#include "page.h"
#include "physical_memory.h"

AHCI *g_ahci;

static void start_cmd(AHCI_HBA_PORT *port);

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES (1 << 30)

static void send_h2d_command_(AHCI *d, int port_num, int slot_num, FIS_REG_H2D *fis, void *out_buf) {
	AHCI_Port *port = &d->ports[port_num];
	HBA_CMD_HEADER *command = port->command + slot_num;
	command->cfl = sizeof(FIS_PIO_SETUP) / sizeof(uint32_t);
	command->w = 0;
	command->prdtl = 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(int_merge64(command->ctbau, command->ctba));
	bzero(cmdtbl, sizeof(HBA_CMD_TBL) + (command->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

	uint64_t paddr_out_buf = page_v2p(page_current_pml4(), out_buf);
	cmdtbl->prdt_entry[0].dba = uint64_low(paddr_out_buf);
	cmdtbl->prdt_entry[0].dbau = uint64_high(paddr_out_buf);
	cmdtbl->prdt_entry[0].dbc = 0x200;
	cmdtbl->prdt_entry[0].i = 1;

	FIS_REG_H2D *fis_reg = (FIS_REG_H2D *)(&cmdtbl->cfis[0]);
	memcpy(fis_reg, fis, sizeof(FIS_REG_H2D));

	while (port->info->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ))
		;

	volatile AHCI_HBA_PORT *info = port->info;
	info->ci = 1 << slot_num;

	// klog("wait");
	while (1) {
		if ((info->ci & (1 << slot_num)) == 0) break;
		if (info->is & HBA_PxIS_TFES) {
			kpanic("error");
		}
	}
}

void ahci_init() {
	PCI_DeviceInfo *pci = pci_find_device(0x01, 0x06);
	assert(pci);
	g_ahci = malloc(sizeof(AHCI));
	kcheck(g_ahci, "Cant alloc g_ahci");
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

		// klog("%d: clb %p", i, port->command);
		// klog("%d: fb  %p", i, port->fis);
		// klog("%s", dump_bytes((void*)p, sizeof(AHCI_HBA_PORT)));
		uint32_t state = p->ssts;
		klog("%d: 0x%03x", i, state);

		start_cmd(p);
	}

	// PIO IENTIFY_DEVICE
	{
		FIS_REG_H2D fis = {
			.fis_type = FIS_TYPE_REG_H2D,
			.c = 1,
			.command = ATA_CMD_IDENTIFY_DEVICE,
			.device = 0, // 1 << 4;
		};
		// char *buf = (char *)malloc(512);
		uint16_t *buf = (uint16_t *)physical_memory_alloc(1);
		send_h2d_command_(d, 0, 0, &fis, buf);
		// klog("%s", dump_bytes(buf, 0x200));
		// klog("%s", buf + 0x14); // name
		uint64_t sectors = *(uint32_t *)(buf + 60);
		// uint64_t sectors = *(uint64_t*)(buf + 100);
		klog("sectors %s", humanize_size(sectors * 512));
	}

	// PIO IENTIFY_DEVICE
	{
		FIS_REG_H2D fis = {
			.fis_type = FIS_TYPE_REG_H2D,
			.c = 1,
			.command = ATA_CMD_IDENTIFY_DEVICE,
			.device = 0, // 1 << 4;
		};
		// char *buf = (char *)malloc(512);
		char *buf = (char *)physical_memory_alloc(1);
		send_h2d_command_(d, 2, 0, &fis, buf);
		uint64_t sectors = (uint64_t)(*(uint32_t *)(buf + 100 * 2));
		klog("sectors %s", humanize_size(sectors * 512));
		// klog("%s", dump_bytes(buf, 0x200));
	}

	for (int i = 0x3f; i < 0x40; i++)
	// for( int i=0; i<1; i++)
	{
		uint64_t sec = i;
		FIS_REG_H2D fis = {
			.fis_type = FIS_TYPE_REG_H2D,
			.c = 1,
			.command = ATA_CMD_READ_DMA_EX,
			.lba0 = (uint32_t)(sec & ((1 << 24) - 1)),
			.lba1 = (uint32_t)(sec >> 24),
			.device = 1 << 6, // lba mode
			.count = 1,
		};
		char *buf = (char *)physical_memory_alloc(1);
		send_h2d_command_(d, 0, 0, &fis, buf);
		klog("%s", dump_bytes(buf, 0x200));
		klog("");
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
