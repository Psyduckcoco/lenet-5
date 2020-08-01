# vivado文件

### IP

包含导出的zip的IP文件



### sdk_files

裸机跑网络时，sdk的代码部分，其中的	“裸机SD卡文件” 需要放到SD中，供SDK代码读取



### .bit  .hwh  .tcl

bit是vivado生成的最终bit流，可与hwh文件放入到jupyter中作为overlay

在vivado中，添加好IP，执行tcl文件，可以自动创建好BD文件

