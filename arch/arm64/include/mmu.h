#ifndef _ASM_ARMV8_MMU_H_
#define _ASM_ARMV8_MMU_H_

/***************************************************************/
/*
 * The following definitions are related each other, shoud be
 * calculated specifically.
 */

#define VA_BITS			CONFIG_SYS_VA_BITS
#define PTE_BLOCK_BITS		CONFIG_SYS_PTL2_BITS

/*
 * block/section address mask and size definitions.
 */

#ifndef UL
#define UL(_v)	(unsigned long)(_v)
#endif

/* PAGE_SHIFT determines the page size */
#undef  PAGE_SIZE
#define PAGE_SHIFT		12
#define PAGE_SIZE		(1 << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

/***************************************************************/

/*
 * Memory types
 */
#define MT_DEVICE_NGNRNE	0
#define MT_DEVICE_NGNRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		4

#define MEMORY_ATTRIBUTES	((0x00 << (MT_DEVICE_NGNRNE * 8)) |	\
				(0x04 << (MT_DEVICE_NGNRE * 8))   |	\
				(0x0c << (MT_DEVICE_GRE * 8))     |	\
				(0x44 << (MT_NORMAL_NC * 8))      |	\
				(UL(0xff) << (MT_NORMAL * 8)))

/*
 * Hardware page table definitions.
 *
 */

#define PTE_TYPE_MASK		(3 << 0)
#define PTE_TYPE_FAULT		(0 << 0)
#define PTE_TYPE_TABLE		(3 << 0)
#define PTE_TYPE_BLOCK		(1 << 0)
#define PTE_TYPE_VALID		(1 << 0)

#define PTE_TABLE_PXN		(1UL << 59)
#define PTE_TABLE_XN		(1UL << 60)
#define PTE_TABLE_AP		(1UL << 61)
#define PTE_TABLE_NS		(1UL << 63)

/*
 * Block
 */
#define PTE_BLOCK_MEMTYPE(x)	((x) << 2)
#define PTE_BLOCK_NS            (1 << 5)
#define PTE_BLOCK_NON_SHARE	(0 << 8)
#define PTE_BLOCK_OUTER_SHARE	(2 << 8)
#define PTE_BLOCK_INNER_SHARE	(3 << 8)
#define PTE_BLOCK_SHARE_MASK	(3 << 8)
#define PTE_BLOCK_AF		(1 << 10)
#define PTE_BLOCK_NG		(1 << 11)
#define PTE_BLOCK_PXN		(UL(1) << 53)
#define PTE_BLOCK_UXN		(UL(1) << 54)

/*
 * AttrIndx[2:0]
 */
#define PMD_ATTRINDX(t)		((t) << 2)
#define PMD_ATTRINDX_MASK	(7 << 2)
#define PMD_ATTRMASK		(PTE_BLOCK_PXN		| \
				 PTE_BLOCK_UXN		| \
				 PTE_BLOCK_SHARE_MASK	| \
				 PMD_ATTRINDX_MASK	| \
				 PTE_TYPE_VALID)

/*
 * TCR flags.
 */
#define TCR_T0SZ(x)		((64 - (x)) << 0)
#define TCR_IRGN_NC		(0 << 8)
#define TCR_IRGN_WBWA		(1 << 8)
#define TCR_IRGN_WT		(2 << 8)
#define TCR_IRGN_WBNWA		(3 << 8)
#define TCR_IRGN_MASK		(3 << 8)
#define TCR_ORGN_NC		(0 << 10)
#define TCR_ORGN_WBWA		(1 << 10)
#define TCR_ORGN_WT		(2 << 10)
#define TCR_ORGN_WBNWA		(3 << 10)
#define TCR_ORGN_MASK		(3 << 10)
#define TCR_SHARED_NON		(0 << 12)
#define TCR_SHARED_OUTER	(2 << 12)
#define TCR_SHARED_INNER	(3 << 12)
#define TCR_TG0_4K		(0 << 14)
#define TCR_TG0_64K		(1 << 14)
#define TCR_TG0_16K		(2 << 14)
#define TCR_EPD1_DISABLE	(1 << 23)

#define TCR_EL1_RSVD		(1 << 31)
#define TCR_EL2_RSVD		(1 << 31 | 1 << 23)
#define TCR_EL3_RSVD		(1 << 31 | 1 << 23)

#ifndef __ASSEMBLY__
static inline void set_ttbr_tcr_mair(int el, uint64_t table, uint64_t tcr, uint64_t attr)
{
	asm volatile("dsb sy");
	if (el == 1) {
		asm volatile("msr ttbr0_el1, %0" : : "r" (table) : "memory");
		asm volatile("msr tcr_el1, %0" : : "r" (tcr) : "memory");
		asm volatile("msr mair_el1, %0" : : "r" (attr) : "memory");
	} else if (el == 2) {
		asm volatile("msr ttbr0_el2, %0" : : "r" (table) : "memory");
		asm volatile("msr tcr_el2, %0" : : "r" (tcr) : "memory");
		asm volatile("msr mair_el2, %0" : : "r" (attr) : "memory");
	} else if (el == 3) {
		asm volatile("msr ttbr0_el3, %0" : : "r" (table) : "memory");
		asm volatile("msr tcr_el3, %0" : : "r" (tcr) : "memory");
		asm volatile("msr mair_el3, %0" : : "r" (attr) : "memory");
	} else {
		while(1);
	}
	asm volatile("isb");
}

struct mm_region {
	uint64_t virt;
	uint64_t phys;
	uint64_t size;
	uint64_t attrs;
};

void setup_pgtables(void);
uint64_t get_tcr(int el, uint64_t *pips, uint64_t *pva_bits);
#endif

#define min(x, y) ({                \
	typeof(x) _min1 = (x);          \
	typeof(y) _min2 = (y);          \
	(void) (&_min1 == &_min2);      \
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({                \
		typeof(x) _max1 = (x);          \
		typeof(y) _max2 = (y);          \
		(void) (&_max1 == &_max2);      \
		_max1 > _max2 ? _max1 : _max2; })

#endif /* _ASM_ARMV8_MMU_H_ */
