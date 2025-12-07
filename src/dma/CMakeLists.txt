# DMA library source files
set(DMA_SOURCES
    dma_virt_to_phys.c
    dma_prepare_descriptor.c
    dma_set_read.c
    dma_set_read_virt.c
    dma_set_write.c
    dma_set_write_virt.c
    dma_start_transfer.c
)

zvb_add_library(zvb_dma "${DMA_SOURCES}")
zvb_export_sources(ZVB_DMA_SOURCES "${DMA_SOURCES}")
