一、 内存分配
    AXI SRAM:
    |内存区域 (AXI SRAM)|用途预估大小|备注
    |0x24000000|LVGL 帧缓冲区1|261 KB|LTDC图层1直接指向此地址
    |0x24041400|LVGL 帧缓冲区2|261 KB|用于双缓冲或图层2
    |0x24082800|LVGL 绘图缓冲区|80 KB|用于实时渲染，大小可调
    |0x24096800|LVGL 对象与数据|剩余空间|存储控件、样式等
    |0x240FFFFF|结束地址|1MB总空间	
二、状态机流程
```mermaid
stateDiagram-v2
    [*] --> ReadBootInfo : 上电/复位

    state ReadBootInfo {
        [*] --> CRC_Check : 读取boot_info
        CRC_Check --> BOOT_INFO_ERROR : CRC校验失败
        CRC_Check --> CheckBootStatus : CRC校验成功
    }

    BOOT_INFO_ERROR --> FactoryMode : 打印错误，重新初始化boot_info
    FactoryMode --> [*] : 进入无限循环（LED闪烁）

    state CheckBootStatus {
        [*] --> CheckStatusValue : 读取boot_status字段
    }

    CheckStatusValue --> BOOT_APP_READY : boot_status == 0x10
    CheckStatusValue --> BOOT_FACTORY_READY : boot_status == 0x11
    CheckStatusValue --> BOOT_OK : boot_status == 0xF0
    CheckStatusValue --> BOOT_APP_CHECK_ERROR : boot_status == 0xF4
    CheckStatusValue --> BOOT_APP_UPDATE_ERROR : boot_status == 0xFF
    CheckStatusValue --> Unknown : 其他值

    state BOOT_APP_READY {
        [*] --> CheckErrorCount : 上次启动失败（APP）
    }
    state BOOT_FACTORY_READY {
        [*] --> CheckErrorCount : 上次启动失败（工厂）
    }

    CheckErrorCount --> ErrorCountLe3 : 错误计数 ≤ 3
    CheckErrorCount --> ErrorCountGt3 : 错误计数 > 3

    ErrorCountLe3 --> IncrementCount : 错误计数+1
    IncrementCount --> WriteBootInfo : 写入Flash
    WriteBootInfo --> DelayAndReboot : 延时2秒
    DelayAndReboot --> [*] : 软重启

    ErrorCountGt3 --> SetBOOT_ERROR : 置为严重错误
    SetBOOT_ERROR --> WriteBootInfoError : 写入Flash
    WriteBootInfoError --> FactoryMode : 延时后进入工厂模式

    BOOT_OK --> PrepareApp : 上次启动成功
    PrepareApp --> SetAppReady : boot_status = BOOT_APP_READY
    SetAppReady --> IncrementBootCount : 启动次数+1
    IncrementBootCount --> WriteBootInfoOK : 写入Flash
    WriteBootInfoOK --> ClearScreen : 清屏
    ClearScreen --> Delay : 延时2秒
    Delay --> JumpToApp : 跳转应用程序（当前被注释）
    JumpToApp --> [*] : 执行应用（不再返回）

    BOOT_APP_CHECK_ERROR --> PrintError : 应用校验错误
    BOOT_APP_UPDATE_ERROR --> PrintError : 应用更新错误
    PrintError --> [*] : 函数退出（可能导致未知行为）

    Unknown --> Halt : 打印未知状态
    Halt --> [*] : 死循环

    note right of FactoryMode
        factory_mode() 会重新初始化boot_info
        然后进入while(1)并闪烁LED
    end note

    note right of DelayAndReboot
        错误计数≤3时，重启尝试
    end note

    note right of PrintError
        代码中这些分支仅打印信息，
        未做进一步处理，函数返回后
        可能跑飞或执行其他代码
    end note
```