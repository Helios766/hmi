#include "bsp_dma2d.h"

DMA2D_HandleTypeDef hdma2d;

void MX_DMA2D_Init(void)
{
    hdma2d.Instance                      = DMA2D;
    hdma2d.Init.Mode                     = DMA2D_M2M;
    hdma2d.Init.ColorMode                = DMA2D_OUTPUT_RGB565;
    hdma2d.Init.OutputOffset             = 0;
    hdma2d.LayerCfg[1].InputOffset       = 0;
    hdma2d.LayerCfg[1].InputColorMode    = DMA2D_INPUT_RGB565;
    hdma2d.LayerCfg[1].AlphaMode         = DMA2D_NO_MODIF_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha        = 0;
    hdma2d.LayerCfg[1].AlphaInverted     = DMA2D_REGULAR_ALPHA;
    hdma2d.LayerCfg[1].RedBlueSwap       = DMA2D_RB_REGULAR;
    hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;

    if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* dma2dHandle)
{
    if(dma2dHandle->Instance==DMA2D)
    {
        __HAL_RCC_DMA2D_CLK_ENABLE();
    }
}

void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* dma2dHandle)
{
    if(dma2dHandle->Instance==DMA2D)
    {
        __HAL_RCC_DMA2D_CLK_DISABLE();
    }
}
