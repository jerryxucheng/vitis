############# gs 1 #############
$(XCLBIN)/readEdgesCU1.$(TARGET).$(DSA).xo: $(GS_KERNEL_PATH)/scatter_gather_top_1.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k readEdgesCU1 -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/readEdgesCU1.$(TARGET).$(DSA).xo
BINARY_LINK_OBJS    += --nk  readEdgesCU1:1
BINARY_LINK_OBJS    += --sp  readEdgesCU1_1.edgesHeadArray:HBM[0]
BINARY_LINK_OBJS    += --sp  readEdgesCU1_1.edgesTailArray:HBM[1]
BINARY_LINK_OBJS    += --sp  readEdgesCU1_1.vertexPushinProp:HBM[2]
BINARY_LINK_OBJS    += --sp  readEdgesCU1_1.tmpVertexProp:HBM[3]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS    += --sp  readEdgesCU1_1.edgeProp:HBM[4]
endif
BINARY_LINK_OBJS    += --slr readEdgesCU1_1:SLR$(GS1_SLR)

############# gs 2 #############
$(XCLBIN)/readEdgesCU2.$(TARGET).$(DSA).xo: $(GS_KERNEL_PATH)/scatter_gather_top_2.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k readEdgesCU2 -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/readEdgesCU2.$(TARGET).$(DSA).xo
BINARY_LINK_OBJS    += --nk  readEdgesCU2:1
BINARY_LINK_OBJS    += --sp  readEdgesCU2_1.edgesHeadArray:HBM[8]
BINARY_LINK_OBJS    += --sp  readEdgesCU2_1.edgesTailArray:HBM[9]
BINARY_LINK_OBJS    += --sp  readEdgesCU2_1.vertexPushinProp:HBM[10]
BINARY_LINK_OBJS    += --sp  readEdgesCU2_1.tmpVertexProp:HBM[11]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS    += --sp  readEdgesCU2_1.edgeProp:HBM[12]
endif
BINARY_LINK_OBJS    += --slr readEdgesCU2_1:SLR$(GS2_SLR)

############# gs 3 #############
$(XCLBIN)/readEdgesCU3.$(TARGET).$(DSA).xo: $(GS_KERNEL_PATH)/scatter_gather_top_3.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k readEdgesCU3 -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/readEdgesCU3.$(TARGET).$(DSA).xo
BINARY_LINK_OBJS    += --nk  readEdgesCU3:1
BINARY_LINK_OBJS    += --sp  readEdgesCU3_1.edgesHeadArray:HBM[16]
BINARY_LINK_OBJS    += --sp  readEdgesCU3_1.edgesTailArray:HBM[17]
BINARY_LINK_OBJS    += --sp  readEdgesCU3_1.vertexPushinProp:HBM[18]
BINARY_LINK_OBJS    += --sp  readEdgesCU3_1.tmpVertexProp:HBM[19]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS    += --sp  readEdgesCU3_1.edgeProp:HBM[20]
endif
BINARY_LINK_OBJS    += --slr readEdgesCU3_1:SLR$(GS3_SLR)

############# gs 4 #############
$(XCLBIN)/readEdgesCU4.$(TARGET).$(DSA).xo: $(GS_KERNEL_PATH)/scatter_gather_top_4.cpp
	mkdir -p $(XCLBIN)
	$(XOCC) $(CLFLAGS) -c -k readEdgesCU4 -I'$(<D)' -o'$@' '$<'
BINARY_CONTAINER_OBJS += $(XCLBIN)/readEdgesCU4.$(TARGET).$(DSA).xo
BINARY_LINK_OBJS    += --nk  readEdgesCU4:1
BINARY_LINK_OBJS    += --sp  readEdgesCU4_1.edgesHeadArray:HBM[24]
BINARY_LINK_OBJS    += --sp  readEdgesCU4_1.edgesTailArray:HBM[25]
BINARY_LINK_OBJS    += --sp  readEdgesCU4_1.vertexPushinProp:HBM[26]
BINARY_LINK_OBJS    += --sp  readEdgesCU4_1.tmpVertexProp:HBM[27]
ifeq ($(strip $(HAVE_EDGE_PROP)), $(strip $(VAR_TRUE)))
BINARY_LINK_OBJS    += --sp  readEdgesCU4_1.edgeProp:HBM[28]
endif
BINARY_LINK_OBJS    += --slr readEdgesCU4_1:SLR$(GS4_SLR)