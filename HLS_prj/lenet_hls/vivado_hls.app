<project xmlns="com.autoesl.autopilot.project" name="lenet_hls" top="top_fun">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" clean="true" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../main.cpp" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="lenet_hls/conv.h" sc="0" tb="false" cflags="" blackbox="false"/>
        <file name="lenet_hls/conv.cpp" sc="0" tb="false" cflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="inactive"/>
        <solution name="s_pipe" status="active"/>
    </solutions>
</project>

